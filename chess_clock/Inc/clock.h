#ifndef CLOCK_H
#define CLOCK_H

#include "main.h"
#include "lcd1602.h"

typedef struct {
    uint8_t sec;
    uint8_t min;
    uint8_t hour;
    uint8_t day;
    uint8_t date;
    uint8_t month;
    uint8_t year;
} RTC_TimeTypeDef;

typedef enum {
    CLOCK_SHOW,
    CLOCK_SET_HOUR,
    CLOCK_SET_MIN,
    CLOCK_SET_SEC,
    CLOCK_SET_DATE,
    CLOCK_SET_MONTH,
    CLOCK_SET_YEAR,
} clock_state_t;

void DS1302_Init(void);
void DS1302_SetTime(RTC_TimeTypeDef *time);
void DS1302_GetTime(RTC_TimeTypeDef *time);
void clock(void);

#endif
