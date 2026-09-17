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
#define out3_Pin GPIO_PIN_0
#define out3_GPIO_Port GPIOA
#define out11_Pin GPIO_PIN_2
#define out11_GPIO_Port GPIOA
#define out10_Pin GPIO_PIN_6
#define out10_GPIO_Port GPIOA
#define out7_Pin GPIO_PIN_7
#define out7_GPIO_Port GPIOA
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
#define in6_Pin GPIO_PIN_13
#define in6_GPIO_Port GPIOE
#define in7_Pin GPIO_PIN_14
#define in7_GPIO_Port GPIOE
#define in4_Pin GPIO_PIN_15
#define in4_GPIO_Port GPIOE
#define in5_Pin GPIO_PIN_10
#define in5_GPIO_Port GPIOB
#define in2_Pin GPIO_PIN_11
#define in2_GPIO_Port GPIOB
#define in3_Pin GPIO_PIN_12
#define in3_GPIO_Port GPIOB
#define in0_Pin GPIO_PIN_13
#define in0_GPIO_Port GPIOB
#define in1_Pin GPIO_PIN_14
#define in1_GPIO_Port GPIOB
#define out6_Pin GPIO_PIN_15
#define out6_GPIO_Port GPIOB
#define out5_Pin GPIO_PIN_12
#define out5_GPIO_Port GPIOD
#define out1_Pin GPIO_PIN_8
#define out1_GPIO_Port GPIOC
#define out0_Pin GPIO_PIN_9
#define out0_GPIO_Port GPIOC
#define out2_Pin GPIO_PIN_8
#define out2_GPIO_Port GPIOA
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
#define out4_Pin GPIO_PIN_3
#define out4_GPIO_Port GPIOB

/* USER CODE BEGIN Private defines */

/* USER CODE END Private defines */

#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */
