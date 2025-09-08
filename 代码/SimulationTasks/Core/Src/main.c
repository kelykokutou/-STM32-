/* USER CODE BEGIN Header */
/**
	******************************************************************************
	* @file           : main.c
	* @brief          : Main program body
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
	/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "dma.h"
#include "i2c.h"
#include "tim.h"
#include "usart.h"
#include "gpio.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "oled.h"
#include "bmp.h"
#include "BH1750.h"
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */



/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/

/* USER CODE BEGIN PV */

uint32_t relay_off_value = 400;
uint32_t relay_on_value = 200;
uint8_t relay_state = 0;
uint8_t relay_refresh_flag = 0;

uint32_t curtain_off_value = 800;
uint32_t curtain_on_value = 100;
uint8_t curtain_state = 0;
uint8_t curtain_refresh_flag = 0;
uint8_t motor_on_flag = 0;
int64_t motor_time = 0;;

uint32_t off_value_cache = 0;
uint32_t on_value_cache = 0;
int8_t bit_value = 0;
int8_t bit_value_hishorty = 0;
uint8_t value_cache_string [] = "00000";

uint8_t test[10];
uint16_t brightness = 0;
uint16_t brightness_buffer = 0;
uint8_t brightness_refresh_flag = 0;
uint8_t LUX_string [] = "LUX:  ";
uint8_t space [] = "  ";
uint8_t Brightness_string [] = "000000";

uint8_t OLED_refresh_flag = 0;
uint8_t OLED_refresh_lock = 0;
uint8_t OLED_content_refresh = 0;

uint16_t timer_value = 0;

uint8_t UI_page = 0;
uint8_t UI_page_change = 1;
int8_t config_page1_pos = 0;
int8_t config_page1_pos_hishorty = 0;
int8_t config_page2_pos = 0;
int8_t config_page2_pos_hishorty = 0;

uint8_t manual = 0;

uint8_t key_scan_flag = 0;
uint8_t key_value_change = 0;
uint16_t key_value_cache = 0;
uint16_t key_value = 0;
uint16_t key_value_history1 = 0;
uint16_t key_value_history2 = 0;
int8_t press_key_value = -1;
uint8_t input_mode = 0;

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
/* USER CODE BEGIN PFP */
void Button_Input();
void Trans_Int_to_String(uint32_t intarray, uint8_t* num_string, uint16_t lenght);
void Key_Scan();
/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */

/* USER CODE END 0 */

/**
	* @brief  The application entry point.
	* @retval int
	*/
int main(void) {
	/* USER CODE BEGIN 1 */

	/* USER CODE END 1 */

	/* MCU Configuration--------------------------------------------------------*/

	/* Reset of all peripherals, Initializes the Flash interface and the Systick. */
	HAL_Init();

	/* USER CODE BEGIN Init */

	/* USER CODE END Init */

	/* Configure the system clock */
	SystemClock_Config();

	/* USER CODE BEGIN SysInit */

	/* USER CODE END SysInit */

	/* Initialize all configured peripherals */
	MX_GPIO_Init();
	MX_DMA_Init();
	MX_I2C1_Init();
	MX_USART1_UART_Init();
	MX_TIM2_Init();
	/* USER CODE BEGIN 2 */
	reset_DVI;
	HAL_Delay(100);
	set_DVI;
	/* USER CODE END 2 */

	/* Infinite loop */
	/* USER CODE BEGIN WHILE */
	OLED_Init();
	OLED_ColorTurn(0);
	OLED_DisplayTurn(0);

	OLED_Splash_Screens(1000);

	BH1750_Init();

	set_L293_EN_34;
	reset_L293_3;
	reset_L293_4;

	HAL_TIM_Base_Start_IT(&htim2);
	HAL_Delay(200);
	while (1) {
		/* USER CODE END WHILE */

		/* USER CODE BEGIN 3 */
		if (UI_page == 0) {
			if (UI_page_change == 1) {//如果是从其他页面切换来
				UI_page_change = 0;//清零切换标志位
				OLED_Main_interface_Pre_layout();//预布局函数，将UI中不会变化的内容填入缓存
				if (manual == 0) {//如果是自动模式
					OLED_ShowString(80, 0, LUX_string, 16, 1);//显示“LUX：”字符串
				}
				else {//如果是手动模式
					OLED_ShowChinese(80, 0, 28, 16, 1);//显示“手动  ”字符串
					OLED_ShowChinese(96, 0, 29, 16, 1);
					OLED_ShowString(112, 0, space, 16, 1);
				}
			}

			if (brightness_refresh_flag == 1) {
				brightness_refresh_flag = 0;
				brightness = BH1750_GetValue();
				Trans_Int_to_String(brightness, Brightness_string, 6);
				OLED_ShowString(80, 16, Brightness_string, 16, 1);
			}

			if (manual == 0) {
				if (relay_state == 0 && brightness < relay_on_value) {
					set_Relay;
					relay_state = 1;
					OLED_ShowChinese(32, 0, 4, 16, 1);
					OLED_ShowChinese(48, 0, 5, 16, 1);
				}
				if (relay_state != 0 && brightness > relay_off_value) {
					reset_Relay;
					relay_state = 0;
					OLED_ShowChinese(32, 0, 6, 16, 1);
					OLED_ShowChinese(48, 0, 7, 16, 1);
				}
				if (curtain_state == 0 && brightness < curtain_on_value) {
					curtain_state = 1;
					curtain_refresh_flag = 1;
					OLED_ShowChinese(32, 16, 26, 16, 1);
					OLED_ShowChinese(48, 16, 27, 16, 1);
				}
				if (curtain_state != 0 && brightness > curtain_off_value) {
					curtain_state = 0;
					curtain_refresh_flag = 1;
					OLED_ShowChinese(32, 16, 24, 16, 1);
					OLED_ShowChinese(48, 16, 25, 16, 1);
				}
			}

			if (key_value_change != 0) {
				key_value_change = 0;
				switch (press_key_value) {
					case 0:
						break;
					case 1:
						if (manual == 0) {
							manual = 1;
							OLED_ShowChinese(80, 0, 28, 16, 1);
							OLED_ShowChinese(96, 0, 29, 16, 1);
							OLED_ShowString(112, 0, space, 16, 1);
						}
						else {
							manual = 0;
							OLED_ShowString(80, 0, LUX_string, 16, 1);
						}
						break;
					case 2:
						UI_page = 1;
						UI_page_change = 1;
						break;
					case 3:
						if (manual != 0) {
							if (relay_state == 0) {
								set_Relay;
								relay_state = 1;
								OLED_ShowChinese(32, 0, 4, 16, 1);
								OLED_ShowChinese(48, 0, 5, 16, 1);

							}
							else {
								reset_Relay;
								relay_state = 0;
								OLED_ShowChinese(32, 0, 6, 16, 1);
								OLED_ShowChinese(48, 0, 7, 16, 1);
							}
						}
						break;
					case 4:
						if (manual != 0) {
							if (curtain_state == 0) {
								curtain_state = 1;
								curtain_refresh_flag = 1;
								OLED_ShowChinese(32, 16, 26, 16, 1);
								OLED_ShowChinese(48, 16, 27, 16, 1);
							}
							else {
								curtain_state = 0;
								curtain_refresh_flag = 1;
								OLED_ShowChinese(32, 16, 24, 16, 1);
								OLED_ShowChinese(48, 16, 25, 16, 1);

							}
						}
						break;

					default:
						break;
				}
			}
			if (relay_state == 0)
				reset_LED_blue;
			else
				set_LED_blue;
			if (curtain_state == 0)
				set_LED_yellow;
			else
				reset_LED_yellow;
		}
		if (UI_page == 1) {

			if (UI_page_change == 1) {
				UI_page_change = 0;
				OLED_Config_interface1_Pre_layout();
				config_page1_pos = 0;
				config_page2_pos = 0;
			}

			if (key_value_change != 0) {
				key_value_change = 0;
				switch (press_key_value) {
					case 0:
						config_page1_pos--;
						if (config_page1_pos < 0)
							config_page1_pos = 0;
						break;
					case 1:
						switch (config_page1_pos) {
							case 0:
								UI_page = 0;
								UI_page_change = 1;
								break;
							case 1:
								UI_page = 2;
								UI_page_change = 1;
								break;
							case 2:
								UI_page = 2;
								UI_page_change = 1;
								break;

							default:
								break;
						}
						break;
					case 2:
						config_page1_pos++;
						if (config_page1_pos > 2)
							config_page1_pos = 2;
						break;
					default:
						break;
				}

			}

			if (config_page1_pos != config_page1_pos_hishorty) {
				config_page1_pos_hishorty = config_page1_pos;
				switch (config_page1_pos) {
					case 0:
						OLED_ShowChinese(0, 0, 20, 16, 0);
						OLED_ShowChinese(16, 0, 21, 16, 0);
						OLED_ShowChinese(32, 0, 0, 16, 1);
						OLED_ShowChinese(48, 0, 1, 16, 1);
						OLED_ShowChinese(64, 0, 2, 16, 1);
						OLED_ShowChinese(80, 0, 3, 16, 1);
						break;
					case 1:
						Trans_Int_to_String(relay_on_value, value_cache_string, 6);
						OLED_ShowString(16, 16, &(value_cache_string[1]), 16, 1);
						Trans_Int_to_String(relay_off_value, value_cache_string, 6);
						OLED_ShowString(16 + 72, 16, &(value_cache_string[1]), 16, 1);
						OLED_ShowChinese(0, 0, 20, 16, 1);
						OLED_ShowChinese(16, 0, 21, 16, 1);
						OLED_ShowChinese(32, 0, 0, 16, 0);
						OLED_ShowChinese(48, 0, 1, 16, 0);
						OLED_ShowChinese(64, 0, 2, 16, 1);
						OLED_ShowChinese(80, 0, 3, 16, 1);
						OLED_ShowChinese(0, 16, 4, 16, 1);
						OLED_ShowChinese(72, 16, 6, 16, 1);
						break;
					case 2:
						Trans_Int_to_String(curtain_on_value, value_cache_string, 6);
						OLED_ShowString(16, 16, &(value_cache_string[1]), 16, 1);
						Trans_Int_to_String(curtain_off_value, value_cache_string, 6);
						OLED_ShowString(16 + 72, 16, &(value_cache_string[1]), 16, 1);
						OLED_ShowChinese(0, 0, 20, 16, 1);
						OLED_ShowChinese(16, 0, 21, 16, 1);
						OLED_ShowChinese(32, 0, 0, 16, 1);
						OLED_ShowChinese(48, 0, 1, 16, 1);
						OLED_ShowChinese(64, 0, 2, 16, 0);
						OLED_ShowChinese(80, 0, 3, 16, 0);
						OLED_ShowChinese(0, 16, 26, 16, 1);
						OLED_ShowChinese(72, 16, 25, 16, 1);
						break;

					default:
						break;
				}

			}

		}
		if (UI_page >= 2) {

			if (UI_page_change == 1) {
				UI_page_change = 0;
				OLED_Config_interface2_Pre_layout();
				if (config_page1_pos == 1) {
					on_value_cache = relay_on_value;
					off_value_cache = relay_off_value;
				}
				if (config_page1_pos == 2) {
					on_value_cache = curtain_on_value;
					off_value_cache = curtain_off_value;
					OLED_ShowChinese(0, 0, 26, 16, 1);
					OLED_ShowChinese(72, 0, 25, 16, 1);
				}
				config_page2_pos = 0;
				Trans_Int_to_String(on_value_cache, value_cache_string, 5);
				OLED_ShowString(16, 0, value_cache_string, 16, 1);
				Trans_Int_to_String(off_value_cache, value_cache_string, 5);
				OLED_ShowString(88, 0, value_cache_string, 16, 1);
				bit_value = (on_value_cache / 10000) % 10;
				OLED_ShowChar(16, 0, (0x30 + bit_value), 16, 0);
			}

			if (key_value_change != 0) {
				key_value_change = 0;
				if (input_mode == 0) {
					switch (press_key_value) {
						case 0:
							config_page2_pos--;
							if (config_page2_pos < 0)
								config_page2_pos = 11;

							break;
						case 1:
							switch (config_page2_pos) {
								case 0:
									input_mode = 1;
									bit_value = (on_value_cache / 10000) % 10;
									break;
								case 1:
									input_mode = 1;
									bit_value = (on_value_cache / 1000) % 10;
									break;
								case 2:
									input_mode = 1;
									bit_value = (on_value_cache / 100) % 10;
									break;
								case 3:
									input_mode = 1;
									bit_value = (on_value_cache / 10) % 10;
									break;
								case 4:
									input_mode = 1;
									bit_value = (off_value_cache / 1) % 10;
									break;
								case 5:
									input_mode = 1;
									bit_value = (off_value_cache / 10000) % 10;
									break;
								case 6:
									input_mode = 1;
									bit_value = (off_value_cache / 1000) % 10;
									break;
								case 7:
									input_mode = 1;
									bit_value = (off_value_cache / 100) % 10;
									break;
								case 8:
									input_mode = 1;
									bit_value = (off_value_cache / 10) % 10;
									break;
								case 9:
									input_mode = 1;
									bit_value = (off_value_cache / 1) % 10;
									break;
								case 10:
									UI_page = 1;
									UI_page_change = 1;
									break;
								case 11:
									UI_page = 1;
									UI_page_change = 1;
									if (config_page1_pos == 1) {
										relay_on_value = on_value_cache;
										relay_off_value = off_value_cache;
									}
									if (config_page1_pos == 2) {
										curtain_on_value = on_value_cache;
										curtain_off_value = off_value_cache;
									}
									break;
								default:
									break;
							}
							break;
						case 2:
							config_page2_pos++;
							if (config_page2_pos > 11)
								config_page2_pos = 0;
							break;

						default:
							break;
					}
				}
				else {
					switch (press_key_value) {
						case 0:
							bit_value--;
							if (bit_value < 0)
								bit_value = 9;
							break;
						case 1:
							input_mode = 0;
							switch (config_page2_pos) {
								case 0:
									on_value_cache = (on_value_cache % 10000) + bit_value * 10000;
									break;
								case 1:
									on_value_cache = ((on_value_cache / 10000) * 10000) + (on_value_cache % 1000) + bit_value * 1000;
									break;
								case 2:
									on_value_cache = ((on_value_cache / 1000) * 1000) + (on_value_cache % 100) + bit_value * 100;
									break;
								case 3:
									on_value_cache = ((on_value_cache / 100) * 100) + (on_value_cache % 10) + bit_value * 10;
									break;
								case 4:
									on_value_cache = ((on_value_cache / 10) * 10) + bit_value * 1;
									break;
								case 5:
									off_value_cache = (off_value_cache % 10000) + bit_value * 10000;
									break;
								case 6:
									off_value_cache = ((off_value_cache / 10000) * 10000) + (off_value_cache % 1000) + bit_value * 1000;
									break;
								case 7:
									off_value_cache = ((off_value_cache / 1000) * 1000) + (off_value_cache % 100) + bit_value * 100;
									break;
								case 8:
									off_value_cache = ((off_value_cache / 100) * 100) + (off_value_cache % 10) + bit_value * 10;
									break;
								case 9:
									off_value_cache = ((off_value_cache / 10) * 10) + bit_value * 1;
									break;
								default:
									break;
							}
							break;
						case 2:
							bit_value++;
							if (bit_value > 9)
								bit_value = 0;
							break;

						default:
							break;
					}
				}
			}

			if ((config_page2_pos != config_page2_pos_hishorty) || (bit_value != bit_value_hishorty)) {
				config_page2_pos_hishorty = config_page2_pos;
				bit_value_hishorty = bit_value;
				Trans_Int_to_String(on_value_cache, value_cache_string, 5);
				OLED_ShowString(16, 0, value_cache_string, 16, 1);
				Trans_Int_to_String(off_value_cache, value_cache_string, 5);
				OLED_ShowString(88, 0, value_cache_string, 16, 1);
				switch (config_page2_pos) {
					case 0:
						if (input_mode == 0)
							bit_value = (on_value_cache / 10000) % 10;
						OLED_ShowChar(16, 0, (0x30 + bit_value), 16, 0);
						OLED_ShowChinese(80, 16, 22, 16, 1);
						OLED_ShowChinese(96, 16, 23, 16, 1);
						OLED_ShowChinese(16, 16, 20, 16, 1);
						OLED_ShowChinese(32, 16, 21, 16, 1);
						break;
					case 1:
						if (input_mode == 0)
							bit_value = (on_value_cache / 1000) % 10;
						OLED_ShowChar(24, 0, (0x30 + bit_value), 16, 0);
						break;
					case 2:
						if (input_mode == 0)
							bit_value = (on_value_cache / 100) % 10;
						OLED_ShowChar(32, 0, (0x30 + bit_value), 16, 0);
						break;
					case 3:
						if (input_mode == 0)
							bit_value = (on_value_cache / 10) % 10;
						OLED_ShowChar(40, 0, (0x30 + bit_value), 16, 0);
						break;
					case 4:
						if (input_mode == 0)
							bit_value = (on_value_cache / 1) % 10;
						OLED_ShowChar(48, 0, (0x30 + bit_value), 16, 0);
						break;
					case 5:
						if (input_mode == 0)
							bit_value = (off_value_cache / 10000) % 10;
						OLED_ShowChar(88, 0, (0x30 + bit_value), 16, 0);
						break;
					case 6:
						if (input_mode == 0)
							bit_value = (off_value_cache / 1000) % 10;
						OLED_ShowChar(96, 0, (0x30 + bit_value), 16, 0);
						break;
					case 7:
						if (input_mode == 0)
							bit_value = (off_value_cache / 100) % 10;
						OLED_ShowChar(104, 0, (0x30 + bit_value), 16, 0);
						break;
					case 8:
						if (input_mode == 0)
							bit_value = (off_value_cache / 10) % 10;
						OLED_ShowChar(112, 0, (0x30 + bit_value), 16, 0);
						break;
					case 9:
						if (input_mode == 0)
							bit_value = (off_value_cache / 1) % 10;
						OLED_ShowChar(120, 0, (0x30 + bit_value), 16, 0);
						OLED_ShowChinese(80, 16, 22, 16, 1);
						OLED_ShowChinese(96, 16, 23, 16, 1);
						OLED_ShowChinese(16, 16, 20, 16, 1);
						OLED_ShowChinese(32, 16, 21, 16, 1);
						break;
					case 10:
						OLED_ShowChinese(16, 16, 20, 16, 0);
						OLED_ShowChinese(32, 16, 21, 16, 0);
						OLED_ShowChinese(80, 16, 22, 16, 1);
						OLED_ShowChinese(96, 16, 23, 16, 1);
						break;
					case 11:
						OLED_ShowChinese(16, 16, 20, 16, 1);
						OLED_ShowChinese(32, 16, 21, 16, 1);
						OLED_ShowChinese(80, 16, 22, 16, 0);
						OLED_ShowChinese(96, 16, 23, 16, 0);
						break;

					default:
						break;
				}
			}
		}

		if (curtain_refresh_flag != 0) {//监测窗帘的标志变量
			curtain_refresh_flag = 0;//清除标志变量
			if (curtain_state == 0) {//如果是0状态
				set_L293_EN_34;//使能L293芯片
				set_L293_3;//设定L293的3通道为高电平
				reset_L293_4;//设定L293的4通道为低电平
			}
			else {//如果是其他状态
				set_L293_EN_34;//使能L293芯片
				reset_L293_3;//设定L293的3通道为低电平
				set_L293_4;//设定L293的4通道为高电平
			}
			motor_on_flag = 1;//设定电机开启标志位
			motor_time = 3000;//设定电机运转时间为3000ms
		}
		if (motor_on_flag != 0 && motor_time == 0) {//如果电机开启，且运转时间已到0
			motor_on_flag = 0;//清除电机开启标志位
			set_L293_EN_34;//使能L293芯片
			reset_L293_3;//设定L293的3通道为低电平
			reset_L293_4;//设定L293的4通道为低电平
		}

		if (OLED_refresh_flag == 1 && OLED_content_refresh != 0) {
			OLED_refresh_flag = 0;
			OLED_Refresh();
			OLED_content_refresh = 0;
		}

		if (key_scan_flag != 0) {
			key_scan_flag = 0;
			Key_Scan();
		}
	}

}

/* USER CODE END 3 */

/**
	* @brief System Clock Configuration
	* @retval None
	*/
void SystemClock_Config(void) {
	RCC_OscInitTypeDef RCC_OscInitStruct = { 0 };
	RCC_ClkInitTypeDef RCC_ClkInitStruct = { 0 };

	/** Initializes the RCC Oscillators according to the specified parameters
	* in the RCC_OscInitTypeDef structure.
	*/
	RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
	RCC_OscInitStruct.HSEState = RCC_HSE_ON;
	RCC_OscInitStruct.HSEPredivValue = RCC_HSE_PREDIV_DIV1;
	RCC_OscInitStruct.HSIState = RCC_HSI_ON;
	RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
	RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
	RCC_OscInitStruct.PLL.PLLMUL = RCC_PLL_MUL9;
	if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK) {
		Error_Handler();
	}

	/** Initializes the CPU, AHB and APB buses clocks
	*/
	RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK | RCC_CLOCKTYPE_SYSCLK
		| RCC_CLOCKTYPE_PCLK1 | RCC_CLOCKTYPE_PCLK2;
	RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
	RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
	RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV2;
	RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

	if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_2) != HAL_OK) {
		Error_Handler();
	}
}

/* USER CODE BEGIN 4 */

void Button_Input() {
	input_mode = 1;
	switch (press_key_value) {
		case 0:
			bit_value--;
			if (bit_value < 0)
				bit_value = 9;
			break;
		case 1:
			input_mode = 0;
			switch (config_page2_pos) {
				case 0:
					on_value_cache += bit_value * 10000;
					break;
				case 1:
					on_value_cache += bit_value * 1000;
					break;
				case 2:
					on_value_cache += bit_value * 100;
					break;
				case 3:
					on_value_cache += bit_value * 10;
					break;
				case 4:
					on_value_cache += bit_value;
					break;
				case 5:
					off_value_cache += bit_value * 10000;
					break;
				case 6:
					off_value_cache += bit_value * 1000;
					break;
				case 7:
					off_value_cache += bit_value * 100;
					break;
				case 8:
					off_value_cache += bit_value * 10;
					break;
				case 9:
					off_value_cache += bit_value;
					break;
				default:
					break;
			}
			break;
		case 2:
			bit_value++;
			if (bit_value > 9)
				bit_value = 0;
			break;

		default:
			break;
	}
}

void Trans_Int_to_String(uint32_t intarray, uint8_t* num_string, uint16_t length) {
	uint32_t data;//
	data = intarray;//
	for (int j = (length - 1); j >= 0; j--) {//
		*(num_string + j) = (data % 10) + 0x30;//
		data /= 10;//
	}
	num_string[length] = '\0';//
}

void Key_Scan() {
	key_value = GPIOA->IDR;//
	key_value &= 0xFF;//
	key_value >>= 3;//
	key_value_cache = ((~key_value_history1) & key_value_history2);//
	key_value_cache = (~key_value) & key_value_cache;//
	for (int i = 0; i < 8; i++) {//
		if ((key_value_cache & 0x01) != 0) {//
			press_key_value = i;//
			key_value_change = 1;//
			goto end;//
		}
		key_value_cache >>= 1;//
	}
	press_key_value = -1;//
end:key_value_history2 = key_value_history1;//
	key_value_history1 = key_value;//
}

void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef* htim) {
	if (htim->Instance == TIM2) {//
		timer_value++;//
		if (motor_on_flag != 0 && motor_time != 0) {//
			motor_time--;//
		}
	}
	if (timer_value > 1000)//
		timer_value = 0;//
	if ((timer_value % 10) == 0)//
		key_scan_flag = 1;//
	if ((timer_value % 100) == 0)//
		OLED_refresh_flag = 1;//
	if ((timer_value % 200) == 0)//
		brightness_refresh_flag = 1;//
}

void HAL_I2C_TxCpltCallback(I2C_HandleTypeDef* hi2c) {
	;
}

/* USER CODE END 4 */

/**
	* @brief  This function is executed in case of error occurrence.
	* @retval None
	*/
void Error_Handler(void) {
	/* USER CODE BEGIN Error_Handler_Debug */
	/* User can add his own implementation to report the HAL error return state */
	__disable_irq();
	while (1) {
	}
	/* USER CODE END Error_Handler_Debug */
}

#ifdef  USE_FULL_ASSERT
/**
	* @brief  Reports the name of the source file and the source line number
	*         where the assert_param error has occurred.
	* @param  file: pointer to the source file name
	* @param  line: assert_param error line source number
	* @retval None
	*/
void assert_failed(uint8_t* file, uint32_t line) {
	/* USER CODE BEGIN 6 */
	/* User can add his own implementation to report the file name and line number,
		 ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
		 /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */
