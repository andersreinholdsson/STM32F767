/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.h
  * @brief          : Header for main.c file.
  *                   This file contains the common defines of the application.
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; Copyright (c) 2021 STMicroelectronics.
  * All rights reserved.</center></h2>
  *
  * This software component is licensed by ST under BSD 3-Clause license,
  * the "License"; You may not use this file except in compliance with the
  * License. You may obtain a copy of the License at:
  *                        opensource.org/licenses/BSD-3-Clause
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
#include "stm32f7xx_hal.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include <stdio.h>
#include <stdarg.h>
#include <stddef.h>
#include <ctype.h>
#include <string.h>

#include <KTItask.h>
#include <message.h>
/* USER CODE END Includes */

/* Exported types ------------------------------------------------------------*/
/* USER CODE BEGIN ET */

/* USER CODE END ET */

/* Exported constants --------------------------------------------------------*/
/* USER CODE BEGIN EC */

/* USER CODE END EC */

/* Exported macro ------------------------------------------------------------*/
/* USER CODE BEGIN EM */
typedef uint8_t BOOL;
#define FALSE 0
#define TRUE (!0)

// byte 3 is 0, byte21 major, byte 1 minor, byte 0 revision
#define FIRMWARE	0x00000101

/* USER CODE END EM */

/* Exported functions prototypes ---------------------------------------------*/
void Error_Handler(void);

/* USER CODE BEGIN EFP */
void DfuRunBootLoader(void);

/* USER CODE END EFP */

/* Private defines -----------------------------------------------------------*/
#define LCD_CTP_nRST_Pin GPIO_PIN_2
#define LCD_CTP_nRST_GPIO_Port GPIOE
#define LCD_RTP_IRQ_Pin GPIO_PIN_3
#define LCD_RTP_IRQ_GPIO_Port GPIOE
#define LCD_RTP_DOUT_Pin GPIO_PIN_4
#define LCD_RTP_DOUT_GPIO_Port GPIOE
#define CASE_FAN_RPM_Pin GPIO_PIN_5
#define CASE_FAN_RPM_GPIO_Port GPIOE
#define nCS2_Pin GPIO_PIN_6
#define nCS2_GPIO_Port GPIOE
#define nCS1_Pin GPIO_PIN_8
#define nCS1_GPIO_Port GPIOI
#define LCD_BL_ON_Pin GPIO_PIN_2
#define LCD_BL_ON_GPIO_Port GPIOF
#define DIAG0_1_Pin GPIO_PIN_6
#define DIAG0_1_GPIO_Port GPIOF
#define DIR1_Pin GPIO_PIN_8
#define DIR1_GPIO_Port GPIOF
#define STEP1_Pin GPIO_PIN_9
#define STEP1_GPIO_Port GPIOF
#define ENA1_Pin GPIO_PIN_10
#define ENA1_GPIO_Port GPIOF
#define TH0_Pin GPIO_PIN_0
#define TH0_GPIO_Port GPIOC
#define TH1_Pin GPIO_PIN_3
#define TH1_GPIO_Port GPIOC
#define STEP2_Pin GPIO_PIN_4
#define STEP2_GPIO_Port GPIOH
#define DIR2_Pin GPIO_PIN_5
#define DIR2_GPIO_Port GPIOH
#define ENA2_Pin GPIO_PIN_3
#define ENA2_GPIO_Port GPIOA
#define LCD_RPT_nCS_Pin GPIO_PIN_4
#define LCD_RPT_nCS_GPIO_Port GPIOA
#define SCK1_Pin GPIO_PIN_5
#define SCK1_GPIO_Port GPIOA
#define SDO1_Pin GPIO_PIN_6
#define SDO1_GPIO_Port GPIOA
#define TH3_Pin GPIO_PIN_4
#define TH3_GPIO_Port GPIOC
#define TH4_Pin GPIO_PIN_5
#define TH4_GPIO_Port GPIOC
#define BOOT1_Pin GPIO_PIN_2
#define BOOT1_GPIO_Port GPIOB
#define DIAG0_2_Pin GPIO_PIN_11
#define DIAG0_2_GPIO_Port GPIOF
#define DIAG1_2_Pin GPIO_PIN_12
#define DIAG1_2_GPIO_Port GPIOF
#define LCD_DnC_Pin GPIO_PIN_1
#define LCD_DnC_GPIO_Port GPIOG
#define LCD_DB4_Pin GPIO_PIN_7
#define LCD_DB4_GPIO_Port GPIOE
#define LCD_DB5_Pin GPIO_PIN_8
#define LCD_DB5_GPIO_Port GPIOE
#define LCD_DB6_Pin GPIO_PIN_9
#define LCD_DB6_GPIO_Port GPIOE
#define LCD_DB7_Pin GPIO_PIN_10
#define LCD_DB7_GPIO_Port GPIOE
#define LCD_DB8_Pin GPIO_PIN_11
#define LCD_DB8_GPIO_Port GPIOE
#define LCD_DB9_Pin GPIO_PIN_12
#define LCD_DB9_GPIO_Port GPIOE
#define LCD_DB10_Pin GPIO_PIN_13
#define LCD_DB10_GPIO_Port GPIOE
#define LCD_DB11_Pin GPIO_PIN_14
#define LCD_DB11_GPIO_Port GPIOE
#define LCD_DB12_Pin GPIO_PIN_15
#define LCD_DB12_GPIO_Port GPIOE
#define LCD_DB13_Pin GPIO_PIN_8
#define LCD_DB13_GPIO_Port GPIOD
#define LCD_DB14_Pin GPIO_PIN_9
#define LCD_DB14_GPIO_Port GPIOD
#define LCD_DB15_Pin GPIO_PIN_10
#define LCD_DB15_GPIO_Port GPIOD
#define LDC_DB0_Pin GPIO_PIN_14
#define LDC_DB0_GPIO_Port GPIOD
#define LCD_DB1_Pin GPIO_PIN_15
#define LCD_DB1_GPIO_Port GPIOD
#define LIMIT_SWITCH_Pin GPIO_PIN_8
#define LIMIT_SWITCH_GPIO_Port GPIOG
#define FAN0_Pin GPIO_PIN_6
#define FAN0_GPIO_Port GPIOC
#define LCD_nCS_Pin GPIO_PIN_7
#define LCD_nCS_GPIO_Port GPIOC
#define FAN1_Pin GPIO_PIN_8
#define FAN1_GPIO_Port GPIOC
#define FAN2_Pin GPIO_PIN_9
#define FAN2_GPIO_Port GPIOC
#define HeatPWM_Pin GPIO_PIN_10
#define HeatPWM_GPIO_Port GPIOA
#define USER_LED_Pin GPIO_PIN_13
#define USER_LED_GPIO_Port GPIOH
#define FAN3_Pin GPIO_PIN_0
#define FAN3_GPIO_Port GPIOI
#define CaseFan_Pin GPIO_PIN_2
#define CaseFan_GPIO_Port GPIOI
#define LCD_RTP_SCK_Pin GPIO_PIN_10
#define LCD_RTP_SCK_GPIO_Port GPIOC
#define LCD_RTP_Din_Pin GPIO_PIN_11
#define LCD_RTP_Din_GPIO_Port GPIOC
#define LCD_RTP_DOUTC12_Pin GPIO_PIN_12
#define LCD_RTP_DOUTC12_GPIO_Port GPIOC
#define LCD_DB2_Pin GPIO_PIN_0
#define LCD_DB2_GPIO_Port GPIOD
#define LCD_DB3_Pin GPIO_PIN_1
#define LCD_DB3_GPIO_Port GPIOD
#define LCD_nRD_Pin GPIO_PIN_4
#define LCD_nRD_GPIO_Port GPIOD
#define LCD_nWE_Pin GPIO_PIN_5
#define LCD_nWE_GPIO_Port GPIOD
#define LCD_nRST_Pin GPIO_PIN_6
#define LCD_nRST_GPIO_Port GPIOD
#define SDO1D7_Pin GPIO_PIN_7
#define SDO1D7_GPIO_Port GPIOD
#define LCD_CTP_SCK_Pin GPIO_PIN_6
#define LCD_CTP_SCK_GPIO_Port GPIOB
#define LCD_CTP_SDA_Pin GPIO_PIN_7
#define LCD_CTP_SDA_GPIO_Port GPIOB
#define LCD_TearEffect_Pin GPIO_PIN_0
#define LCD_TearEffect_GPIO_Port GPIOE
#define LCD_TearEffect_EXTI_IRQn EXTI0_IRQn
#define LCD_CTP_IRQ_Pin GPIO_PIN_1
#define LCD_CTP_IRQ_GPIO_Port GPIOE
#define nCS2I7_Pin GPIO_PIN_7
#define nCS2I7_GPIO_Port GPIOI
/* USER CODE BEGIN Private defines */

/* USER CODE END Private defines */

#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
