#include "lcd1602.h"

/* ── Конфигурации двух дисплеев ─────────────────────────────────── */
const LCD_t LCD1 = {
    .rs_port = GPIOB, .rs_pin = GPIO_PIN_12,
    .en_port = GPIOB, .en_pin = GPIO_PIN_11,
    .d4_port = GPIOB, .d4_pin = GPIO_PIN_0,
    .d5_port = GPIOB, .d5_pin = GPIO_PIN_1,
    .d6_port = GPIOB, .d6_pin = GPIO_PIN_2,
    .d7_port = GPIOB, .d7_pin = GPIO_PIN_10,
};

const LCD_t LCD2 = {
    .rs_port = GPIOA, .rs_pin = GPIO_PIN_11,
    .en_port = GPIOA, .en_pin = GPIO_PIN_10,
    .d4_port = GPIOC, .d4_pin = GPIO_PIN_8,
    .d5_port = GPIOC, .d5_pin = GPIO_PIN_9,
    .d6_port = GPIOA, .d6_pin = GPIO_PIN_8,
    .d7_port = GPIOA, .d7_pin = GPIO_PIN_9,
};

/* ── Внутренние функции ─────────────────────────────────────────── */

static void pulse(const LCD_t *lcd)
{
    HAL_GPIO_WritePin(lcd->en_port, lcd->en_pin, GPIO_PIN_SET);
    HAL_Delay(1);
    HAL_GPIO_WritePin(lcd->en_port, lcd->en_pin, GPIO_PIN_RESET);
    HAL_Delay(1);
}

static void send_nibble(const LCD_t *lcd, uint8_t n)
{
    HAL_GPIO_WritePin(lcd->d4_port, lcd->d4_pin, (n >> 0) & 1);
    HAL_GPIO_WritePin(lcd->d5_port, lcd->d5_pin, (n >> 1) & 1);
    HAL_GPIO_WritePin(lcd->d6_port, lcd->d6_pin, (n >> 2) & 1);
    HAL_GPIO_WritePin(lcd->d7_port, lcd->d7_pin, (n >> 3) & 1);
    pulse(lcd);
}

static void send_byte(const LCD_t *lcd, uint8_t byte, uint8_t is_data)
{
    HAL_GPIO_WritePin(lcd->rs_port, lcd->rs_pin,
                      is_data ? GPIO_PIN_SET : GPIO_PIN_RESET);
    send_nibble(lcd, byte >> 4);
    send_nibble(lcd, byte & 0x0F);
    HAL_Delay(2);
}

static void cmd(const LCD_t *lcd, uint8_t c)  { send_byte(lcd, c, 0); }
static void dat(const LCD_t *lcd, uint8_t d)  { send_byte(lcd, d, 1); }

/* ── Публичные функции ──────────────────────────────────────────── */

void LCD_Init(const LCD_t *lcd)
{
    HAL_Delay(50);
    HAL_GPIO_WritePin(lcd->rs_port, lcd->rs_pin, GPIO_PIN_RESET);
    HAL_GPIO_WritePin(lcd->en_port, lcd->en_pin, GPIO_PIN_RESET);

    send_nibble(lcd, 0x03); HAL_Delay(5);
    send_nibble(lcd, 0x03); HAL_Delay(1);
    send_nibble(lcd, 0x03); HAL_Delay(1);
    send_nibble(lcd, 0x02); HAL_Delay(1);

    cmd(lcd, 0x28); /* 4-bit, 2 строки, 5x8   */
    cmd(lcd, 0x0C); /* дисплей вкл, курсор выкл */
    cmd(lcd, 0x06); /* сдвиг вправо             */
    cmd(lcd, 0x01); /* очистка                  */
    HAL_Delay(2);
}

void LCD_Clear(const LCD_t *lcd)
{
    cmd(lcd, 0x01);
    HAL_Delay(2);
}

void LCD_SetCursor(const LCD_t *lcd, uint8_t col, uint8_t row)
{
    cmd(lcd, 0x80 | (col + (row ? 0x40 : 0x00)));
}

void LCD_WriteChar(const LCD_t *lcd, char c)
{
    dat(lcd, (uint8_t)c);
}

void LCD_WriteString(const LCD_t *lcd, const char *str)
{
    while (*str)
        dat(lcd, (uint8_t)*str++);
}
