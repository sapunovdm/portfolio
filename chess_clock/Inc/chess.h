#ifndef CHESS_H
#define CHESS_H

#include "main.h"
#include "lcd1602.h"

/* --- Types and Enums --- */


typedef struct {
	uint32_t IS_MOVING;
	uint32_t TIME;
} player_t;

typedef enum {
    MODE_BULLET1 ,
    MODE_BLITZ3,
    MODE_BLITZ5,
	MODE_RAPID10,
    MODE_RAPID15,
    MODE_RAPID30,
    MODE_CLASSIC60,
    MODE_COUNT
} game_mode_t;

typedef enum {
	ZERO,
    ONE,
	TWO,
	THREE,
	FIVE,
	TEN,
	TWENTY,
    INC_COUNT
} inc_t;

typedef enum {
    APP_CHESS,
    APP_CLOCK
} app_mode_t;

/* --- Defines --- */
//chess clock modes in seconds
#define bullet1 60
#define blitz3 180
#define blitz5 300
#define rapid10 600
#define rapid15 900
#define rapid30 1800
#define classic60 3600

#define BUZZER_PORT GPIOB
#define BUZZER_PIN  GPIO_PIN_0

/* --- Extern variables --- */
extern TIM_HandleTypeDef htim2;

extern volatile buttons_t buttons;
extern volatile player_t player1;
extern volatile player_t player2;
extern volatile app_mode_t app_mode;
extern volatile uint8_t tick;
extern volatile uint8_t MOV;

/* --- Prototypes --- */
void menu(void);
void chess(void);
void Buzzer_On(void);
void Buzzer_Off(void);
void Buzzer_Beep(uint16_t duration);
void Buzzer_Melody(void);
void LED_player1_ON();
void LED_player2_ON();
void LED_player1_OFF();
void LED_player2_OFF();
void LED_Off();

#endif /* CHESS_H */
