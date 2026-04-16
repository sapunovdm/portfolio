#ifndef INC_LCD1602_H_
#define INC_LCD1602_H_

#include "main.h"

/* ── Конфигурация одного дисплея ────────────────────────────────── */
typedef struct {
    GPIO_TypeDef *rs_port; uint16_t rs_pin;
    GPIO_TypeDef *en_port; uint16_t en_pin;
    GPIO_TypeDef *d4_port; uint16_t d4_pin;
    GPIO_TypeDef *d5_port; uint16_t d5_pin;
    GPIO_TypeDef *d6_port; uint16_t d6_pin;
    GPIO_TypeDef *d7_port; uint16_t d7_pin;
} LCD_t;

/* ── Готовые конфиги для двух дисплеев ──────────────────────────── */
/*    LCD1: RS=PB12, EN=PB11, D7=PB10, D6=PB1, D5=PB0, D4=PC5     */
/*    LCD2: RS=PA11, EN=PA10, D7=PA9,  D6=PA8, D5=PC9, D4=PC8     */
extern const LCD_t LCD1;
extern const LCD_t LCD2;

/* ── API ────────────────────────────────────────────────────────── */
void LCD_Init(const LCD_t *lcd);
void LCD_Clear(const LCD_t *lcd);
void LCD_SetCursor(const LCD_t *lcd, uint8_t col, uint8_t row);
void LCD_WriteChar(const LCD_t *lcd, char c);
void LCD_WriteString(const LCD_t *lcd, const char *str);

#endif /* INC_LCD1602_H_ */
