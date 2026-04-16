#include "clock.h"
#include "stdio.h"

/* ── DS1302 низкий уровень ──────────────────────────────────────── */

static void DS1302_DAT_OUT(void) {
    GPIO_InitTypeDef g = {0};
    g.Pin   = DS1302_DAT_Pin;
    g.Mode  = GPIO_MODE_OUTPUT_PP;
    g.Speed = GPIO_SPEED_FREQ_HIGH;
    HAL_GPIO_Init(DS1302_DAT_GPIO_Port, &g);
}

static void DS1302_DAT_IN(void) {
    GPIO_InitTypeDef g = {0};
    g.Pin  = DS1302_DAT_Pin;
    g.Mode = GPIO_MODE_INPUT;
    g.Pull = GPIO_NOPULL;
    HAL_GPIO_Init(DS1302_DAT_GPIO_Port, &g);
}

static void DS1302_WriteByte(uint8_t dat) {
    DS1302_DAT_OUT();
    for (int i = 0; i < 8; i++) {
        HAL_GPIO_WritePin(DS1302_DAT_GPIO_Port, DS1302_DAT_Pin,
                          (dat & 0x01) ? GPIO_PIN_SET : GPIO_PIN_RESET);
        HAL_GPIO_WritePin(DS1302_CLK_GPIO_Port, DS1302_CLK_Pin, GPIO_PIN_SET);
        HAL_GPIO_WritePin(DS1302_CLK_GPIO_Port, DS1302_CLK_Pin, GPIO_PIN_RESET);
        dat >>= 1;
    }
}

static uint8_t DS1302_ReadByte(void) {
    uint8_t dat = 0;
    DS1302_DAT_IN();
    for (int i = 0; i < 8; i++) {
        dat >>= 1;
        if (HAL_GPIO_ReadPin(DS1302_DAT_GPIO_Port, DS1302_DAT_Pin) == GPIO_PIN_SET)
            dat |= 0x80;
        HAL_GPIO_WritePin(DS1302_CLK_GPIO_Port, DS1302_CLK_Pin, GPIO_PIN_SET);
        HAL_GPIO_WritePin(DS1302_CLK_GPIO_Port, DS1302_CLK_Pin, GPIO_PIN_RESET);
    }
    return dat;
}

static void DS1302_WriteReg(uint8_t reg, uint8_t data) {
    HAL_GPIO_WritePin(DS1302_RST_GPIO_Port, DS1302_RST_Pin, GPIO_PIN_SET);
    DS1302_WriteByte(reg);
    DS1302_WriteByte(data);
    HAL_GPIO_WritePin(DS1302_RST_GPIO_Port, DS1302_RST_Pin, GPIO_PIN_RESET);
}

static uint8_t DS1302_ReadReg(uint8_t reg) {
    uint8_t data;
    HAL_GPIO_WritePin(DS1302_RST_GPIO_Port, DS1302_RST_Pin, GPIO_PIN_SET);
    DS1302_WriteByte(reg);
    data = DS1302_ReadByte();
    HAL_GPIO_WritePin(DS1302_RST_GPIO_Port, DS1302_RST_Pin, GPIO_PIN_RESET);
    return data;
}

static uint8_t BCD2DEC(uint8_t bcd) { return (bcd >> 4) * 10 + (bcd & 0x0F); }
static uint8_t DEC2BCD(uint8_t dec) { return ((dec / 10) << 4) | (dec % 10); }

/* ── Публичные функции DS1302 ───────────────────────────────────── */

void DS1302_Init(void) {
    HAL_GPIO_WritePin(DS1302_RST_GPIO_Port, DS1302_RST_Pin, GPIO_PIN_RESET);
    HAL_GPIO_WritePin(DS1302_CLK_GPIO_Port, DS1302_CLK_Pin, GPIO_PIN_RESET);
    DS1302_WriteReg(0x8E, 0x00);
    uint8_t sec = DS1302_ReadReg(0x81);
    if (sec & 0x80)
        DS1302_WriteReg(0x80, sec & 0x7F);
}

void DS1302_GetTime(RTC_TimeTypeDef *t) {
    t->sec   = BCD2DEC(DS1302_ReadReg(0x81) & 0x7F);
    t->min   = BCD2DEC(DS1302_ReadReg(0x83) & 0x7F);
    t->hour  = BCD2DEC(DS1302_ReadReg(0x85) & 0x3F);
    t->date  = BCD2DEC(DS1302_ReadReg(0x87) & 0x3F);
    t->month = BCD2DEC(DS1302_ReadReg(0x89) & 0x1F);
    t->year  = BCD2DEC(DS1302_ReadReg(0x8D));
}

void DS1302_SetTime(RTC_TimeTypeDef *t) {
    DS1302_WriteReg(0x8E, 0x00);
    DS1302_WriteReg(0x80, DEC2BCD(t->sec));
    DS1302_WriteReg(0x82, DEC2BCD(t->min));
    DS1302_WriteReg(0x84, DEC2BCD(t->hour));
    DS1302_WriteReg(0x86, DEC2BCD(t->date));
    DS1302_WriteReg(0x88, DEC2BCD(t->month));
    DS1302_WriteReg(0x8C, DEC2BCD(t->year));
}

/* ════════════════════════════════════════════════════════════════════
   Вспомогательные функции
   ════════════════════════════════════════════════════════════════════ */

static void inc_field(uint8_t *val, uint8_t max, uint8_t min) {
    *val = (*val + 1 > max) ? min : *val + 1;
}

static void dec_field(uint8_t *val, uint8_t max, uint8_t min) {
    *val = (*val == min) ? max : *val - 1;
}

/* Подсказка на LCD2 в зависимости от состояния */
static const char* hint_top[] = {
    [CLOCK_SHOW]      = "                ",
    [CLOCK_SET_HOUR]  = "  Setting time  ",
    [CLOCK_SET_MIN]   = "  Setting time  ",
    [CLOCK_SET_SEC]   = "  Setting time  ",
    [CLOCK_SET_DATE]  = "  Setting date  ",
    [CLOCK_SET_MONTH] = "  Setting date  ",
    [CLOCK_SET_YEAR]  = "  Setting date  ",
};

static const char* hint_bot[] = {
    [CLOCK_SHOW]      = "                ",
    [CLOCK_SET_HOUR]  = "   set: HOURS   ",
    [CLOCK_SET_MIN]   = "  set: MINUTES  ",
    [CLOCK_SET_SEC]   = "  set: SECONDS  ",
    [CLOCK_SET_DATE]  = "    set: DAY    ",
    [CLOCK_SET_MONTH] = "  set: MONTH    ",
    [CLOCK_SET_YEAR]  = "   set: YEAR    ",
};

/* Отрисовка обоих LCD ──────────────────────────────────────────────
   LCD1 строка 0: HH:MM:SS  (мигает редактируемое поле времени)
   LCD1 строка 1: DD.MM.YY  (мигает редактируемое поле даты)
   LCD2: подсказка                                                   */
static void show_all(RTC_TimeTypeDef *t, clock_state_t state, uint8_t hide)
{
    char hh[3], mm[3], ss[3];
    char dd[3], mo[3], yy[3];

    /* Время */
    snprintf(hh, sizeof(hh), "%02d", t->hour);
    snprintf(mm, sizeof(mm), "%02d", t->min);
    snprintf(ss, sizeof(ss), "%02d", t->sec);
    if (state == CLOCK_SET_HOUR  && hide) snprintf(hh, sizeof(hh), "  ");
    if (state == CLOCK_SET_MIN   && hide) snprintf(mm, sizeof(mm), "  ");
    if (state == CLOCK_SET_SEC   && hide) snprintf(ss, sizeof(ss), "  ");

    /* Дата */
    snprintf(dd, sizeof(dd), "%02d", t->date);
    snprintf(mo, sizeof(mo), "%02d", t->month);
    snprintf(yy, sizeof(yy), "%02d", t->year);
    if (state == CLOCK_SET_DATE  && hide) snprintf(dd, sizeof(dd), "  ");
    if (state == CLOCK_SET_MONTH && hide) snprintf(mo, sizeof(mo), "  ");
    if (state == CLOCK_SET_YEAR  && hide) snprintf(yy, sizeof(yy), "  ");

    char row0[17], row1[17];
    snprintf(row0, sizeof(row0), "   %s:%s:%s     ", hh, mm, ss);
    snprintf(row1, sizeof(row1), "   %s.%s.%s     ", dd, mo, yy);

    LCD_SetCursor(&LCD1, 0, 0); LCD_WriteString(&LCD1, row0);
    LCD_SetCursor(&LCD1, 0, 1); LCD_WriteString(&LCD1, row1);

    /* Подсказка на LCD2 */
    LCD_SetCursor(&LCD2, 0, 0); LCD_WriteString(&LCD2, hint_top[state]);
    LCD_SetCursor(&LCD2, 0, 1); LCD_WriteString(&LCD2, hint_bot[state]);
}

/* ════════════════════════════════════════════════════════════════════
   clock()
   ════════════════════════════════════════════════════════════════════ */
void clock(void)
{
    static uint8_t       initialized = 0;
    static clock_state_t state       = CLOCK_SHOW;
    static RTC_TimeTypeDef edit;
    static uint32_t      last_read   = 0;
    static uint32_t      blink_timer = 0;
    static uint8_t       blink_hide  = 0;

    RTC_TimeTypeDef display_time;
    uint32_t now = HAL_GetTick();

    if (!initialized) {
        DS1302_Init();
        initialized = 1;
        LCD_Clear(&LCD1);
        LCD_Clear(&LCD2);
    }

    /* ── MOV: переход по полям ───────────────────────────────────── */
    if (MOV)
    {
        MOV = 0;

        if (state == CLOCK_SHOW) {
            DS1302_GetTime(&edit);
            state = CLOCK_SET_HOUR;
        } else if (state == CLOCK_SET_YEAR) {
            DS1302_SetTime(&edit);
            state = CLOCK_SHOW;
            LCD_Clear(&LCD1);
            LCD_Clear(&LCD2);
        } else {
            state++;
        }

        blink_hide  = 0;
        blink_timer = now;
    }

    /* ── BTN3 (+), BTN1 (-) — только в режиме настройки ─────────── */
    if (state != CLOCK_SHOW)
    {
        if (buttons.BTN3) {
            buttons.BTN3 = 0;
            switch (state) {
                case CLOCK_SET_HOUR:  inc_field(&edit.hour,  23, 0);  break;
                case CLOCK_SET_MIN:   inc_field(&edit.min,   59, 0);  break;
                case CLOCK_SET_SEC:   inc_field(&edit.sec,   59, 0);  break;
                case CLOCK_SET_DATE:  inc_field(&edit.date,  31, 1);  break;
                case CLOCK_SET_MONTH: inc_field(&edit.month, 12, 1);  break;
                case CLOCK_SET_YEAR:  inc_field(&edit.year,  99, 0);  break;
                default: break;
            }
            blink_hide = 0;
            blink_timer = now;
        }

        if (buttons.BTN1) {
            buttons.BTN1 = 0;
            switch (state) {
                case CLOCK_SET_HOUR:  dec_field(&edit.hour,  23, 0);  break;
                case CLOCK_SET_MIN:   dec_field(&edit.min,   59, 0);  break;
                case CLOCK_SET_SEC:   dec_field(&edit.sec,   59, 0);  break;
                case CLOCK_SET_DATE:  dec_field(&edit.date,  31, 1);  break;
                case CLOCK_SET_MONTH: dec_field(&edit.month, 12, 1);  break;
                case CLOCK_SET_YEAR:  dec_field(&edit.year,  99, 0);  break;
                default: break;
            }
            blink_hide = 0;
            blink_timer = now;
        }
    }

    /* ── Мигание 500 мс ─────────────────────────────────────────── */
    if (state != CLOCK_SHOW && (now - blink_timer) >= 500) {
        blink_timer = now;
        blink_hide  = !blink_hide;
    }

    /* ── Обновление дисплея ─────────────────────────────────────── */
    if (state == CLOCK_SHOW) {
        if (now - last_read >= 1000) {
            last_read = now;
            DS1302_GetTime(&display_time);
            show_all(&display_time, CLOCK_SHOW, 0);
        }
    } else {
        show_all(&edit, state, blink_hide);
    }
}
