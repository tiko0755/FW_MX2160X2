/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.h
  * @brief          : Header for main.c file.
  *                   This file contains the common defines of the application.
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2023 STMicroelectronics.
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
#include "stm32g0xx_hal.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */

/* USER CODE END Includes */

/* Exported types ------------------------------------------------------------*/
/* USER CODE BEGIN ET */

/* USER CODE END ET */

/* Exported constants --------------------------------------------------------*/
/* USER CODE BEGIN EC */

/* USER CODE END EC */

/* Exported macro ------------------------------------------------------------*/
/* USER CODE BEGIN EM */

/* USER CODE END EM */

void HAL_TIM_MspPostInit(TIM_HandleTypeDef *htim);

/* Exported functions prototypes ---------------------------------------------*/
void Error_Handler(void);

/* USER CODE BEGIN EFP */

/* USER CODE END EFP */

/* Private defines -----------------------------------------------------------*/
#define LED0_Pin GPIO_PIN_0
#define LED0_GPIO_Port GPIOF
#define LED1_Pin GPIO_PIN_1
#define LED1_GPIO_Port GPIOF
#define DET_Pin GPIO_PIN_0
#define DET_GPIO_Port GPIOA
#define DE_Pin GPIO_PIN_1
#define DE_GPIO_Port GPIOA
#define REFR1_EXT4_Pin GPIO_PIN_4
#define REFR1_EXT4_GPIO_Port GPIOA
#define SPI_CS0_Pin GPIO_PIN_0
#define SPI_CS0_GPIO_Port GPIOB
#define SPI_CS1_Pin GPIO_PIN_1
#define SPI_CS1_GPIO_Port GPIOB
#define M0_EN_Pin GPIO_PIN_2
#define M0_EN_GPIO_Port GPIOB
#define M1_EN_Pin GPIO_PIN_10
#define M1_EN_GPIO_Port GPIOB
#define M0_DIA0_Pin GPIO_PIN_11
#define M0_DIA0_GPIO_Port GPIOB
#define M0_DIA1_Pin GPIO_PIN_12
#define M0_DIA1_GPIO_Port GPIOB
#define M1_DIA0_Pin GPIO_PIN_13
#define M1_DIA0_GPIO_Port GPIOB
#define M1_DIA1_Pin GPIO_PIN_14
#define M1_DIA1_GPIO_Port GPIOB
#define M0_DIR_Pin GPIO_PIN_15
#define M0_DIR_GPIO_Port GPIOB
#define M1_DIR_Pin GPIO_PIN_8
#define M1_DIR_GPIO_Port GPIOA
#define REFL1_EXT7_Pin GPIO_PIN_7
#define REFL1_EXT7_GPIO_Port GPIOC
#define SWD_LED_Pin GPIO_PIN_13
#define SWD_LED_GPIO_Port GPIOA
#define SWC_BOOT_FX_Pin GPIO_PIN_14
#define SWC_BOOT_FX_GPIO_Port GPIOA
#define SCL_Pin GPIO_PIN_15
#define SCL_GPIO_Port GPIOA
#define SDA_Pin GPIO_PIN_0
#define SDA_GPIO_Port GPIOD
#define OUT3_Pin GPIO_PIN_1
#define OUT3_GPIO_Port GPIOD
#define OUT2_Pin GPIO_PIN_2
#define OUT2_GPIO_Port GPIOD
#define OUT1_Pin GPIO_PIN_3
#define OUT1_GPIO_Port GPIOD
#define OUT0_Pin GPIO_PIN_3
#define OUT0_GPIO_Port GPIOB
#define IN2_Pin GPIO_PIN_4
#define IN2_GPIO_Port GPIOB
#define IN0_Pin GPIO_PIN_5
#define IN0_GPIO_Port GPIOB
#define REFR2_EXT6_Pin GPIO_PIN_6
#define REFR2_EXT6_GPIO_Port GPIOB
#define IN3_Pin GPIO_PIN_7
#define IN3_GPIO_Port GPIOB
#define T16CH1_IN1_Pin GPIO_PIN_8
#define T16CH1_IN1_GPIO_Port GPIOB
#define REFL2_EXT9_Pin GPIO_PIN_9
#define REFL2_EXT9_GPIO_Port GPIOB
/* USER CODE BEGIN Private defines */
extern SPI_HandleTypeDef hspi1;
extern TIM_HandleTypeDef htim1;
extern TIM_HandleTypeDef htim3;
extern TIM_HandleTypeDef htim16;
extern UART_HandleTypeDef huart1;
extern UART_HandleTypeDef huart2;
/* USER CODE END Private defines */

#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */
