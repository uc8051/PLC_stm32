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
    uint16_t adc;
    uint16_t ma;
} CalPoint;

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
#define out17_Pin GPIO_PIN_3
#define out17_GPIO_Port GPIOE
#define out16_Pin GPIO_PIN_13
#define out16_GPIO_Port GPIOC
#define out03_Pin GPIO_PIN_0
#define out03_GPIO_Port GPIOA
#define out11_Pin GPIO_PIN_2
#define out11_GPIO_Port GPIOA
#define out10_Pin GPIO_PIN_6
#define out10_GPIO_Port GPIOA
#define out07_Pin GPIO_PIN_7
#define out07_GPIO_Port GPIOA
#define in16_Pin GPIO_PIN_0
#define in16_GPIO_Port GPIOB
#define in17_Pin GPIO_PIN_1
#define in17_GPIO_Port GPIOB
#define in14_Pin GPIO_PIN_7
#define in14_GPIO_Port GPIOE
#define in15_Pin GPIO_PIN_8
#define in15_GPIO_Port GPIOE
#define in12_Pin GPIO_PIN_9
#define in12_GPIO_Port GPIOE
#define in13_Pin GPIO_PIN_10
#define in13_GPIO_Port GPIOE
#define in10_Pin GPIO_PIN_11
#define in10_GPIO_Port GPIOE
#define in11_Pin GPIO_PIN_12
#define in11_GPIO_Port GPIOE
#define in06_Pin GPIO_PIN_13
#define in06_GPIO_Port GPIOE
#define in07_Pin GPIO_PIN_14
#define in07_GPIO_Port GPIOE
#define in04_Pin GPIO_PIN_15
#define in04_GPIO_Port GPIOE
#define in05_Pin GPIO_PIN_10
#define in05_GPIO_Port GPIOB
#define in02_Pin GPIO_PIN_11
#define in02_GPIO_Port GPIOB
#define in03_Pin GPIO_PIN_12
#define in03_GPIO_Port GPIOB
#define in00_Pin GPIO_PIN_13
#define in00_GPIO_Port GPIOB
#define in01_Pin GPIO_PIN_14
#define in01_GPIO_Port GPIOB
#define out06_Pin GPIO_PIN_15
#define out06_GPIO_Port GPIOB
#define out05_Pin GPIO_PIN_12
#define out05_GPIO_Port GPIOD
#define out01_Pin GPIO_PIN_8
#define out01_GPIO_Port GPIOC
#define out00_Pin GPIO_PIN_9
#define out00_GPIO_Port GPIOC
#define out02_Pin GPIO_PIN_8
#define out02_GPIO_Port GPIOA
#define rs232_tx1_Pin GPIO_PIN_9
#define rs232_tx1_GPIO_Port GPIOA
#define rs232_rx1_Pin GPIO_PIN_10
#define rs232_rx1_GPIO_Port GPIOA
#define i2c_SDA_Pin GPIO_PIN_13
#define i2c_SDA_GPIO_Port GPIOA
#define rs485_Pin GPIO_PIN_14
#define rs485_GPIO_Port GPIOA
#define i2c_SCL_Pin GPIO_PIN_15
#define i2c_SCL_GPIO_Port GPIOA
#define rs485_TX_Pin GPIO_PIN_10
#define rs485_TX_GPIO_Port GPIOC
#define rs485_RX_Pin GPIO_PIN_11
#define rs485_RX_GPIO_Port GPIOC
#define out04_Pin GPIO_PIN_3
#define out04_GPIO_Port GPIOB

/* USER CODE BEGIN Private defines */

/* USER CODE END Private defines */

#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */
