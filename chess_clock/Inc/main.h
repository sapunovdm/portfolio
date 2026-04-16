/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.h
  * @brief          : Header for main.c file.
  *                   This file contains the common defines of the application.
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2026 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */
/* USER CODE END Header */

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __MAIN_H
#define __MAIN_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "stm32f1xx_hal.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */

/* USER CODE END Includes */

/* Exported types ------------------------------------------------------------*/
/* USER CODE BEGIN ET */

typedef struct {
	uint8_t BTN1;
	uint8_t BTN2;
	uint8_t BTN3;
} buttons_t;
extern volatile buttons_t buttons;


extern volatile uint8_t MOV;
/* USER CODE END ET */

/* Exported constants --------------------------------------------------------*/
/* USER CODE BEGIN EC */

/* USER CODE END EC */

/* Exported macro ------------------------------------------------------------*/
/* USER CODE BEGIN EM */

/* USER CODE END EM */

/* Exported functions prototypes ---------------------------------------------*/
void Error_Handler(void);

/* USER CODE BEGIN EFP */

/* USER CODE END EFP */

/* Private defines -----------------------------------------------------------*/
#define BTN1_Pin GPIO_PIN_0
#define BTN1_GPIO_Port GPIOA
#define BTN1_EXTI_IRQn EXTI0_IRQn
#define BTN2_Pin GPIO_PIN_1
#define BTN2_GPIO_Port GPIOA
#define BTN2_EXTI_IRQn EXTI1_IRQn
#define BTN3_Pin GPIO_PIN_2
#define BTN3_GPIO_Port GPIOA
#define BTN3_EXTI_IRQn EXTI2_IRQn
#define MOV_Pin GPIO_PIN_3
#define MOV_GPIO_Port GPIOA
#define MOV_EXTI_IRQn EXTI3_IRQn
#define LED1_Pin GPIO_PIN_6
#define LED1_GPIO_Port GPIOA
#define LED2_Pin GPIO_PIN_7
#define LED2_GPIO_Port GPIOA
#define D4_1_Pin GPIO_PIN_0
#define D4_1_GPIO_Port GPIOB
#define D5_1_Pin GPIO_PIN_1
#define D5_1_GPIO_Port GPIOB
#define D6_1_Pin GPIO_PIN_2
#define D6_1_GPIO_Port GPIOB
#define D7_1_Pin GPIO_PIN_10
#define D7_1_GPIO_Port GPIOB
#define EN_1_Pin GPIO_PIN_11
#define EN_1_GPIO_Port GPIOB
#define RS_1_Pin GPIO_PIN_12
#define RS_1_GPIO_Port GPIOB
#define D4_2_Pin GPIO_PIN_8
#define D4_2_GPIO_Port GPIOC
#define D5_2_Pin GPIO_PIN_9
#define D5_2_GPIO_Port GPIOC
#define D6_2_Pin GPIO_PIN_8
#define D6_2_GPIO_Port GPIOA
#define D7_2_Pin GPIO_PIN_9
#define D7_2_GPIO_Port GPIOA
#define EN_2_Pin GPIO_PIN_10
#define EN_2_GPIO_Port GPIOA
#define RS_2_Pin GPIO_PIN_11
#define RS_2_GPIO_Port GPIOA
#define BUZ_Pin GPIO_PIN_12
#define BUZ_GPIO_Port GPIOC
#define DS1302_RST_Pin GPIO_PIN_4
#define DS1302_RST_GPIO_Port GPIOB
#define DS1302_DAT_Pin GPIO_PIN_5
#define DS1302_DAT_GPIO_Port GPIOB
#define DS1302_CLK_Pin GPIO_PIN_6
#define DS1302_CLK_GPIO_Port GPIOB

/* USER CODE BEGIN Private defines */

/* USER CODE END Private defines */

#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */
