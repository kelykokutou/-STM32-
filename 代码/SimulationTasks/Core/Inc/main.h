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
#include "stm32f1xx_hal.h"

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

/* Exported functions prototypes ---------------------------------------------*/
	void Error_Handler(void);

	/* USER CODE BEGIN EFP */

	/* USER CODE END EFP */

	/* Private defines -----------------------------------------------------------*/
#define B1_Pin GPIO_PIN_3
#define B1_GPIO_Port GPIOA
#define B2_Pin GPIO_PIN_4
#define B2_GPIO_Port GPIOA
#define B3_Pin GPIO_PIN_5
#define B3_GPIO_Port GPIOA
#define B4_Pin GPIO_PIN_6
#define B4_GPIO_Port GPIOA
#define B5_Pin GPIO_PIN_7
#define B5_GPIO_Port GPIOA
#define LED_blue_Pin GPIO_PIN_0
#define LED_blue_GPIO_Port GPIOB
#define LED_yellow_Pin GPIO_PIN_1
#define LED_yellow_GPIO_Port GPIOB
#define Relay_Pin GPIO_PIN_12
#define Relay_GPIO_Port GPIOB
#define L293_EN_34_Pin GPIO_PIN_13
#define L293_EN_34_GPIO_Port GPIOB
#define L293_3_Pin GPIO_PIN_14
#define L293_3_GPIO_Port GPIOB
#define L293_4_Pin GPIO_PIN_15
#define L293_4_GPIO_Port GPIOB
#define DVI_Pin GPIO_PIN_15
#define DVI_GPIO_Port GPIOA

/* USER CODE BEGIN Private defines */

#define uchar unsigned char
#define uint unsigned int

#define set_DVI		GPIOA->ODR|=0x8000
#define reset_DVI	GPIOA->ODR&=0x7FFF

#define set_LED_blue			GPIOB->ODR&=0xFFFE
#define reset_LED_blue		GPIOB->ODR|=0x0001
#define set_LED_yellow		GPIOB->ODR&=0xFFFD
#define reset_LED_yellow	GPIOB->ODR|=0x0002

#define set_Relay					GPIOB->ODR|=0x1000//将GPIOB的输出寄存器中对应继电器的位置1
#define reset_Relay				GPIOB->ODR&=0xEFFF//将GPIOB的输出寄存器中对应继电器的位置0

#define set_L293_EN_34		GPIOB->ODR|=0x2000
#define reset_L293_EN_34	GPIOB->ODR&=0xDFFF
#define set_L293_3				GPIOB->ODR|=0x4000
#define reset_L293_3			GPIOB->ODR&=0xBFFF
#define set_L293_4				GPIOB->ODR|=0x8000
#define reset_L293_4			GPIOB->ODR&=0x7FFF

#define read_B1		GPIOA->IDR&0x0008
#define read_B2		GPIOA->IDR&0x0010
#define read_B3		GPIOA->IDR&0x0020
#define read_B4		GPIOA->IDR&0x0040
#define read_B5		GPIOA->IDR&0x0080
// #define read_SCL	GPIOB->IDR&0x0040
// #define read_SDA	GPIOB->IDR&0x0080

#define OLED_TX(tx,num) HAL_I2C_Master_Transmit(&hi2c1, 0x78, tx, num, num)
#define OLED_RX(rx,num) HAL_I2C_Master_Receive(&hi2c1, 0x78, rx, num, num)
#define UART_TX(tx,num)	HAL_UART_Transmit(&huart1, tx, num, num)
#define UART_RX(rx,num)	HAL_UART_Receive(&huart1, rx, num, num)

/* USER CODE END Private defines */

#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */
