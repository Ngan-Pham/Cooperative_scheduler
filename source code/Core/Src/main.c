/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; Copyright (c) 2026 STMicroelectronics.
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
/* Includes ------------------------------------------------------------------*/
#include "main.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "scheduler.h"
#include <stdio.h>
#include <string.h>
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
TIM_HandleTypeDef htim2;

UART_HandleTypeDef huart1;

/* USER CODE BEGIN PV */
uint32_t task_led1_id = NO_TASK_ID;
uint32_t system_time_ms = 0;
/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_TIM2_Init(void);
static void MX_USART1_UART_Init(void);
/* USER CODE BEGIN PFP */
uint32_t get_time(void);
void UART_Print(char *str);

int __io_putchar(int ch) {
    HAL_UART_Transmit(&huart1, (uint8_t *)&ch, 1, 100);
    return ch;
}

void Led_Toggle_PA3(void);
void Led_Toggle_PA4(void);
void Led_Toggle_PA5(void);
void Led_Toggle_PA6(void);
void Led_Toggle_PA7(void);

void Task_Demo_Concurrent_800ms(void);

// Callbacks cho yeu cau: delay ban dau dai thuc day in timestamp
void Task_Delayed_Wakeup_1(void);
void Task_Delayed_Wakeup_2(void);

// Task Demo
void Task_Stop_Led1(void);
void Task_Print_Timestamp_10ms(void);
/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */

/* USER CODE END 0 */

/**
  * @brief  The application entry point.
  * @retval int
  */
int main(void)
{
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
  MX_TIM2_Init();
  MX_USART1_UART_Init();
  /* USER CODE BEGIN 2 */
  HAL_GPIO_WritePin(GPIOA, GPIO_PIN_3 | GPIO_PIN_4 | GPIO_PIN_5 | GPIO_PIN_6 | GPIO_PIN_7, GPIO_PIN_SET);

  UART_Print("\r\n=== SCHEDULER SYSTEM STARTING ===\r\n");

  SCH_Init();
  system_time_ms = 0;
  HAL_TIM_Base_Start_IT(&htim2);

  task_led1_id = SCH_Add_Task(Led_Toggle_PA3, 50, 50);  // LED 1 (PA3): 500ms
  SCH_Add_Task(Led_Toggle_PA4, 100, 100);                 // LED 2 (PA4): 1000ms
  SCH_Add_Task(Led_Toggle_PA5, 150, 150);                 // LED 3 (PA5): 1500ms
  SCH_Add_Task(Led_Toggle_PA6, 200, 200);                 // LED 4 (PA6): 2000ms
  SCH_Add_Task(Led_Toggle_PA7, 250, 250);                 // LED 5 (PA7): 2500ms

  SCH_Add_Task(Task_Demo_Concurrent_800ms, 0, 80);        // Concurrent 800ms

  SCH_Add_Task(Task_Stop_Led1, 500, 0);                   // Stop LED 1 after 6000ms
  SCH_Add_Task(Task_Delayed_Wakeup_1, 150, 0);            // Wakeup after 1500ms (150 ticks)
//  SCH_Add_Task(Task_Delayed_Wakeup_2, 350, 0);            // Wakeup after 3500ms (350 ticks)

  SCH_Add_Task(Task_Print_Timestamp_10ms, 0, 1);          // Timestamp nhip 10ms
/* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
	  SCH_Dispatch_Tasks();
    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */
  }
  /* USER CODE END 3 */
}

/**
  * @brief System Clock Configuration
  * @retval None
  */
void SystemClock_Config(void)
{
  RCC_OscInitTypeDef RCC_OscInitStruct = {0};
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};

  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.HSICalibrationValue = RCC_HSICALIBRATION_DEFAULT;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_NONE;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }
  /** Initializes the CPU, AHB and APB buses clocks
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_HSI;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV1;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_0) != HAL_OK)
  {
    Error_Handler();
  }
}

/**
  * @brief TIM2 Initialization Function
  * @param None
  * @retval None
  */
static void MX_TIM2_Init(void)
{

  /* USER CODE BEGIN TIM2_Init 0 */

  /* USER CODE END TIM2_Init 0 */

  TIM_ClockConfigTypeDef sClockSourceConfig = {0};
  TIM_MasterConfigTypeDef sMasterConfig = {0};

  /* USER CODE BEGIN TIM2_Init 1 */

  /* USER CODE END TIM2_Init 1 */
  htim2.Instance = TIM2;
  htim2.Init.Prescaler = 7999;
  htim2.Init.CounterMode = TIM_COUNTERMODE_UP;
  htim2.Init.Period = 9;
  htim2.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
  htim2.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;
  if (HAL_TIM_Base_Init(&htim2) != HAL_OK)
  {
    Error_Handler();
  }
  sClockSourceConfig.ClockSource = TIM_CLOCKSOURCE_INTERNAL;
  if (HAL_TIM_ConfigClockSource(&htim2, &sClockSourceConfig) != HAL_OK)
  {
    Error_Handler();
  }
  sMasterConfig.MasterOutputTrigger = TIM_TRGO_RESET;
  sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
  if (HAL_TIMEx_MasterConfigSynchronization(&htim2, &sMasterConfig) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN TIM2_Init 2 */

  /* USER CODE END TIM2_Init 2 */

}

/**
  * @brief USART1 Initialization Function
  * @param None
  * @retval None
  */
static void MX_USART1_UART_Init(void)
{

  /* USER CODE BEGIN USART1_Init 0 */

  /* USER CODE END USART1_Init 0 */

  /* USER CODE BEGIN USART1_Init 1 */

  /* USER CODE END USART1_Init 1 */
  huart1.Instance = USART1;
  huart1.Init.BaudRate = 115200;
  huart1.Init.WordLength = UART_WORDLENGTH_8B;
  huart1.Init.StopBits = UART_STOPBITS_1;
  huart1.Init.Parity = UART_PARITY_NONE;
  huart1.Init.Mode = UART_MODE_TX_RX;
  huart1.Init.HwFlowCtl = UART_HWCONTROL_NONE;
  huart1.Init.OverSampling = UART_OVERSAMPLING_16;
  if (HAL_UART_Init(&huart1) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN USART1_Init 2 */

  /* USER CODE END USART1_Init 2 */

}

/**
  * @brief GPIO Initialization Function
  * @param None
  * @retval None
  */
static void MX_GPIO_Init(void)
{
  GPIO_InitTypeDef GPIO_InitStruct = {0};

  /* GPIO Ports Clock Enable */
  __HAL_RCC_GPIOA_CLK_ENABLE();

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOA, GPIO_PIN_3|GPIO_PIN_4|GPIO_PIN_5|GPIO_PIN_6
                          |GPIO_PIN_7, GPIO_PIN_RESET);

  /*Configure GPIO pins : PA3 PA4 PA5 PA6
                           PA7 */
  GPIO_InitStruct.Pin = GPIO_PIN_3|GPIO_PIN_4|GPIO_PIN_5|GPIO_PIN_6
                          |GPIO_PIN_7;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

}

/* USER CODE BEGIN 4 */
uint32_t get_time(void) {
    return system_time_ms;
}

void UART_Print(char *str) {
    HAL_UART_Transmit(&huart1, (uint8_t *)str, strlen(str), 50);
}

void Led_Toggle_PA3(void) {
    HAL_GPIO_TogglePin(GPIOA, GPIO_PIN_3);
    char msg[64];
    sprintf(msg, "[%lu ms] Toggle LED 1\r\n", get_time());
    UART_Print(msg);
}

void Led_Toggle_PA4(void) {
    HAL_GPIO_TogglePin(GPIOA, GPIO_PIN_4);
    char msg[64];
    sprintf(msg, "[%lu ms] Toggle LED 2\r\n", get_time());
    UART_Print(msg);
}

void Led_Toggle_PA5(void) {
    HAL_GPIO_TogglePin(GPIOA, GPIO_PIN_5);
    char msg[64];
    sprintf(msg, "[%lu ms] Toggle LED 3\r\n", get_time());
    UART_Print(msg);
}

void Led_Toggle_PA6(void) {
    HAL_GPIO_TogglePin(GPIOA, GPIO_PIN_6);
    char msg[64];
    sprintf(msg, "[%lu ms] Toggle LED 4\r\n", get_time());
    UART_Print(msg);
}

void Led_Toggle_PA7(void) {
    HAL_GPIO_TogglePin(GPIOA, GPIO_PIN_7);
    char msg[64];
    sprintf(msg, "[%lu ms] Toggle LED 5\r\n", get_time());
    UART_Print(msg);
}

// Ham thuc hien xoa LED 1 sau 6s
void Task_Stop_Led1(void) {
    char msg[80];
    sprintf(msg, ">>> [%lu ms] 5s EXPIRED! DELETING LED 1 (ID: %lu) <<<\r\n", get_time(), task_led1_id);
    UART_Print(msg);
    SCH_Delete_Task(task_led1_id);
    // Tat han LED 1 de co dinh trang thai
    HAL_GPIO_WritePin(GPIOA, GPIO_PIN_3, GPIO_PIN_SET);
}

// In nhip 10ms (Demo theo yeu cau muc 5: print get_time every 10ms callback)
void Task_Print_Timestamp_10ms(void) {
    char msg[32];
    sprintf(msg, "%lu\r\n", get_time());
    UART_Print(msg);
}

void Task_Demo_Concurrent_800ms(void) {
    char msg[64];
    sprintf(msg, "[CONCURRENT] Task 800ms fired at: %lu ms\r\n", get_time());
    UART_Print(msg);
}

// Callback wake up sau 1500ms
void Task_Delayed_Wakeup_1(void) {
    char msg[80];
    sprintf(msg, ">>> [WAKEUP 1] Initial Delay 1500ms expired at: %lu ms <<<\r\n", get_time());
    UART_Print(msg);
}

// Callback wake up sau 3500ms
void Task_Delayed_Wakeup_2(void) {
    char msg[80];
    sprintf(msg, ">>> [WAKEUP 2] Initial Delay 3500ms expired at: %lu ms <<<\r\n", get_time());
    UART_Print(msg);
}

// Timer ISR Callback goi SCH_Update
void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim) {
    if (htim->Instance == TIM2) {
        system_time_ms += 10; // Timer tick = 10ms
        SCH_Update();
    }
}
/* USER CODE END 4 */

/**
  * @brief  This function is executed in case of error occurrence.
  * @retval None
  */
void Error_Handler(void)
{
  /* USER CODE BEGIN Error_Handler_Debug */
  /* User can add his own implementation to report the HAL error return state */
  __disable_irq();
  while (1)
  {
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
void assert_failed(uint8_t *file, uint32_t line)
{
  /* USER CODE BEGIN 6 */
  /* User can add his own implementation to report the file name and line number,
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
