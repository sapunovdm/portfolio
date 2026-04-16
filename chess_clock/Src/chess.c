#include "chess.h"
#include "stdio.h"

/* --- Variables --- */
static volatile uint16_t mode_seconds = 0;
volatile uint8_t game_started = 0;
volatile uint8_t tick = 0; // flag 1 sec passed


volatile buttons_t buttons = {0};
volatile player_t player1 = {0};
volatile player_t player2 = {0};

volatile uint8_t setup_stage = 0;
volatile game_mode_t current_mode = MODE_BLITZ5;
// 0 = выбор времени
// 1 = выбор инкремента
// 2 = игра

volatile inc_t current_inc = ZERO;
volatile uint8_t increment = 0;

const uint16_t mode_times[MODE_COUNT] = {
    bullet1,
    blitz3,
    blitz5,
	rapid10,
    rapid15,
    rapid30,
    classic60
};

const char* mode_names[MODE_COUNT] = {
    "1  min",
    "3  min",
    "5  min",
	"10 min",
    "15 min",
    "30 min",
    "60 min"
};

const char* inc_names[INC_COUNT] = {
    "+0  s",
    "+1  s",
    "+2  s",
    "+3  s",
    "+5  s",
    "+10 s",
	"+20 s"
};

const uint8_t inc_values[INC_COUNT] = {
    0, 1, 2, 3, 5, 10, 20
};

uint8_t clear = 0;
volatile uint8_t paused = 0;
volatile uint8_t MOV = {0};

volatile app_mode_t app_mode = APP_CHESS;
volatile uint8_t mode_selected = 0;


void LED_player1_ON()
{
	HAL_GPIO_WritePin(LED2_GPIO_Port, LED2_Pin, GPIO_PIN_RESET);
	HAL_GPIO_WritePin(LED1_GPIO_Port, LED1_Pin, GPIO_PIN_SET);
}

void LED_player2_ON()
{
	HAL_GPIO_WritePin(LED1_GPIO_Port, LED1_Pin, GPIO_PIN_RESET);
	HAL_GPIO_WritePin(LED2_GPIO_Port, LED2_Pin, GPIO_PIN_SET);
}

void LED_player1_OFF()
{
	HAL_GPIO_WritePin(LED1_GPIO_Port, LED1_Pin, GPIO_PIN_RESET);
}

void LED_player2_OFF()
{
	HAL_GPIO_WritePin(LED2_GPIO_Port, LED2_Pin, GPIO_PIN_RESET);
}

void LED_Off()
{
	void LED_player1_OFF();
	void LED_player2_OFF();
}

void Buzzer_On(void)
{
    HAL_GPIO_WritePin(BUZ_GPIO_Port, BUZ_Pin, GPIO_PIN_SET);
}

void Buzzer_Off(void)
{
    HAL_GPIO_WritePin(BUZ_GPIO_Port, BUZ_Pin, GPIO_PIN_RESET);
}

void Buzzer_Beep(uint16_t duration)
{
    Buzzer_On();
    HAL_Delay(duration);
    Buzzer_Off();
}

void Buzzer_Melody(void)
{
    Buzzer_Beep(100);
    HAL_Delay(100);

    Buzzer_Beep(100);
    HAL_Delay(100);

    Buzzer_Beep(200);
}
void LCD_ShowMode(void)
{
    if (setup_stage == 0)
    {
        LCD_SetCursor(&LCD1, 0, 0);
        LCD_WriteString(&LCD1, "Time:");

        LCD_SetCursor(&LCD1, 0, 1);
        LCD_WriteString(&LCD1, mode_names[current_mode]);
    }
    else if (setup_stage == 1)
    {
    	if (clear == 0)
    	{
        	LCD_Clear(&LCD1);
        	LCD_Clear(&LCD2);
        	clear++;
    	}
        LCD_SetCursor(&LCD1, 0, 0);
        LCD_WriteString(&LCD1, "Increment:");

        LCD_SetCursor(&LCD1, 0, 1);
        LCD_WriteString(&LCD1, inc_names[current_inc]);
    }

    LCD_SetCursor(&LCD2, 0, 0);
    LCD_WriteString(&LCD2, "Central button");
    LCD_SetCursor(&LCD2, 0, 1);
    LCD_WriteString(&LCD2, "to confirm");
}

void LCD_ShowAppMenu(void)
{
    LCD_SetCursor(&LCD1, 0, 0);
    LCD_WriteString(&LCD1, "Select mode:");

    LCD_SetCursor(&LCD1, 0, 1);

    if (app_mode == APP_CHESS)
        LCD_WriteString(&LCD1, ">Chess Timer ");
    else
        LCD_WriteString(&LCD1, ">Clock       ");
    LCD_SetCursor(&LCD2, 0, 0);
    LCD_WriteString(&LCD2, "Central button");
    LCD_SetCursor(&LCD2, 0, 1);
    LCD_WriteString(&LCD2, "to confirm");
}

/* --- Main Logic Functions --- */
void menu(void)
{
    while (!mode_selected)
    {
        LCD_ShowAppMenu();

        // BTN1 назад
        if (buttons.BTN1)
        {
            buttons.BTN1 = 0;

            if (app_mode == 0)
                app_mode = 1;
            else
                app_mode = 0;

            HAL_Delay(150);
        }

        // BTN3 вперёд
        if (buttons.BTN3)
        {
            buttons.BTN3 = 0;

            if (app_mode == 0)
                app_mode = 1;
            else
                app_mode = 0;

            HAL_Delay(150);
        }

        // BTN2 выбрать
        if (buttons.BTN2)
        {
            buttons.BTN2 = 0;

            mode_selected = 1;

            LCD_Clear(&LCD1);
            LCD_Clear(&LCD2);

            HAL_Delay(200);
        }
    }
}

void chess(void)
{
    while (setup_stage < 2)
    {
        LCD_ShowMode();

        // BTN1 назад
        if (buttons.BTN1)
        {
            buttons.BTN1 = 0;

            if (setup_stage == 0)
            {
                if (current_mode == 0)
                    current_mode = MODE_COUNT - 1;
                else
                    current_mode--;
            }
            else if (setup_stage == 1)
            {
                if (current_inc == 0)
                    current_inc = INC_COUNT - 1;
                else
                    current_inc--;
            }

            HAL_Delay(150);
        }

        // BTN3 вперёд
        if (buttons.BTN3)
        {
            buttons.BTN3 = 0;

            if (setup_stage == 0)
            {
                current_mode++;
                if (current_mode >= MODE_COUNT)
                    current_mode = 0;
            }
            else if (setup_stage == 1)
            {
                current_inc++;
                if (current_inc >= INC_COUNT)
                    current_inc = 0;
            }

            HAL_Delay(150);
        }

        // BTN2 → подтвердить
        if (buttons.BTN2)
        {
            buttons.BTN2 = 0;

            setup_stage++;

            HAL_Delay(200);
        }
        mode_seconds = mode_times[current_mode];
        increment = inc_values[current_inc];

        player1.TIME = mode_seconds;
        player2.TIME = mode_seconds;
    }

    if (clear == 1)
    {
        LCD_Clear(&LCD1);
        LCD_Clear(&LCD2);
        clear++;
    }

    // --- старт игры ---
    if (!game_started)
    {
        LCD_SetCursor(&LCD1, 0, 0);
        LCD_WriteString(&LCD1, "Press to start");

        LCD_SetCursor(&LCD2, 0, 0);
        LCD_WriteString(&LCD2, "Press to start");
        if (buttons.BTN1)
        {
            buttons.BTN1 = 0;

            LED_player1_ON();
            player1.IS_MOVING = 1;
            player2.IS_MOVING = 0;

            game_started = 1;
        }

        if (buttons.BTN3)
        {
            buttons.BTN3 = 0;


            LED_player2_ON();
            player2.IS_MOVING = 1;
            player1.IS_MOVING = 0;

            game_started = 1;
        }
        if (game_started)
        {
        	Buzzer_Melody();
            LCD_Clear(&LCD1);
            LCD_Clear(&LCD2);
        }
    }
    // --- пауза ---
    if (buttons.BTN2)
    {
        LCD_SetCursor(&LCD1, 0, 0);
        LCD_WriteString(&LCD1, "     Paused");

        LCD_SetCursor(&LCD2, 0, 0);
        LCD_WriteString(&LCD2, "     Paused");
        buttons.BTN2 = 0;

        if (game_started)
        {
            paused = !paused; // переключение паузы
        }
        if (!paused)
        {
            LCD_Clear(&LCD1);
            LCD_Clear(&LCD2);
        }
    }

    if (tick && game_started && !paused) //counter overflow in 1s
    {
        if (player1.IS_MOVING && player1.TIME > 0)
        {
            player1.TIME--;
        }
        else if (player2.IS_MOVING && player2.TIME > 0)
        {
            player2.TIME--;
        }

        char buf_1[17];
        uint8_t m1 = player1.TIME / 60;
        uint8_t s1 = player1.TIME % 60;
        snprintf(buf_1, sizeof(buf_1), "     %02d:%02d", m1, s1);
        LCD_SetCursor(&LCD1, 0, 1);
        LCD_WriteString(&LCD1, buf_1);

        char buf_2[17];
        uint8_t m2 = player2.TIME / 60;
        uint8_t s2 = player2.TIME % 60;
        snprintf(buf_2, sizeof(buf_2), "     %02d:%02d", m2, s2);
        LCD_SetCursor(&LCD2, 0, 1);
        LCD_WriteString(&LCD2, buf_2);

        if (MOV && game_started && !paused)
        {
            if (player1.IS_MOVING)
            {
                player1.TIME += increment;
                player1.IS_MOVING = 0;
                player2.IS_MOVING = 1;
            }
            else if (player2.IS_MOVING)
            {
                player2.TIME += increment;
                player2.IS_MOVING = 0;
                player1.IS_MOVING = 1;
            }
            MOV = 0;
        }
        if (player1.IS_MOVING) LED_player1_ON();
        else LED_player2_ON();

        tick = 0;

        if (player1.TIME == 0 || player2.TIME == 0)
        {
            LCD_SetCursor(&LCD1, 0, 1);
            LCD_WriteString(&LCD1, "   Time is up!  ");
            Buzzer_Melody();
            HAL_TIM_Base_Stop_IT(&htim2);
        }
    }
}
