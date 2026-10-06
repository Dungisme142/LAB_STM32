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

/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */
  #define STATE_RED    0
  #define STATE_YELLOW 1
  #define STATE_GREEN  2
/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */
/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/

/* USER CODE BEGIN PV */

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MX_GPIO_Init(void);
/* USER CODE BEGIN PFP */

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
  /* USER CODE BEGIN 2 */
  void display7SEG_Horizontal(int num);
  void display7SEG_Vertical(int num);

  int vert_status = STATE_RED; // Xuất phát: Đỏ 5 giây
  int count_vert = 5;

  // --- TRẠNG THÁI VÀ BIẾN ĐẾM HƯỚNG NGANG (HORIZONTAL) ---
  int horiz_status = STATE_GREEN; // Xuất phát: Xanh 3 giây
  int count_horiz = 3;
  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
    while (1)
    {
    /* USER CODE END WHILE */
      // 1. MÁY TRẠNG THÁI ĐIỀU KHIỂN HƯỚNG DỌC (VERTICAL FSM) - ACTIVE HIGH
      // =========================================================================
      switch (vert_status) {
          case STATE_RED:
              HAL_GPIO_WritePin(GPIOA, GPIO_PIN_5, GPIO_PIN_SET);   // RED ON
              HAL_GPIO_WritePin(GPIOA, GPIO_PIN_6, GPIO_PIN_RESET); // YELLOW OFF
              HAL_GPIO_WritePin(GPIOA, GPIO_PIN_7, GPIO_PIN_RESET); // GREEN OFF
              break;

          case STATE_YELLOW:
              HAL_GPIO_WritePin(GPIOA, GPIO_PIN_5, GPIO_PIN_RESET); // RED OFF
              HAL_GPIO_WritePin(GPIOA, GPIO_PIN_6, GPIO_PIN_SET);   // YELLOW ON
              HAL_GPIO_WritePin(GPIOA, GPIO_PIN_7, GPIO_PIN_RESET); // GREEN OFF
              break;

          case STATE_GREEN:
              HAL_GPIO_WritePin(GPIOA, GPIO_PIN_5, GPIO_PIN_RESET); // RED OFF
              HAL_GPIO_WritePin(GPIOA, GPIO_PIN_6, GPIO_PIN_RESET); // YELLOW OFF
              HAL_GPIO_WritePin(GPIOA, GPIO_PIN_7, GPIO_PIN_SET);   // GREEN ON
              break;
      }

      // =========================================================================
      // 2. MÁY TRẠNG THÁI ĐIỀU KHIỂN HƯỚNG NGANG (HORIZONTAL FSM) - ACTIVE HIGH
      // =========================================================================
      switch (horiz_status) {
          case STATE_RED:
              HAL_GPIO_WritePin(GPIOA, GPIO_PIN_4, GPIO_PIN_SET);   // RED ON
              HAL_GPIO_WritePin(GPIOA, GPIO_PIN_8, GPIO_PIN_RESET); // YELLOW OFF
              HAL_GPIO_WritePin(GPIOA, GPIO_PIN_9, GPIO_PIN_RESET); // GREEN OFF
              break;

          case STATE_YELLOW:
              HAL_GPIO_WritePin(GPIOA, GPIO_PIN_4, GPIO_PIN_RESET); // RED OFF
              HAL_GPIO_WritePin(GPIOA, GPIO_PIN_8, GPIO_PIN_SET);   // YELLOW ON
              HAL_GPIO_WritePin(GPIOA, GPIO_PIN_9, GPIO_PIN_RESET); // GREEN OFF
              break;

          case STATE_GREEN:
              HAL_GPIO_WritePin(GPIOA, GPIO_PIN_4, GPIO_PIN_RESET); // RED OFF
              HAL_GPIO_WritePin(GPIOA, GPIO_PIN_8, GPIO_PIN_RESET); // YELLOW OFF
              HAL_GPIO_WritePin(GPIOA, GPIO_PIN_9, GPIO_PIN_SET);   // GREEN ON
              break;
      }

      // =========================================================================
      // 3. HIỂN THỊ ĐẾM NGƯỢC LÊN 2 LED 7 ĐOẠN
      // =========================================================================
      display7SEG_Vertical(count_vert);
      display7SEG_Horizontal(count_horiz);

      // =========================================================================
      // 4. ĐỘ TRỄ 1 GIÂY DUY NHẤT (HEARTBEAT)
      // =========================================================================
      HAL_Delay(1000);

      // =========================================================================
      // 5. CẬP NHẬT BIẾN ĐẾM VÀ CHUYỂN TRẠNG THÁI ĐỘC LẬP TỪNG HƯỚNG
      // =========================================================================

      // --- Giảm đếm ngược mỗi giây ---
      count_vert--;
      count_horiz--;

      // --- Chuyển trạng thái Độc Lập cho Hướng Dọc: RED(5s) -> YELLOW(2s) -> GREEN(3s) ---
      if (count_vert <= 0) {
          switch (vert_status) {
              case STATE_RED:      // Hết Đỏ 5s -> Sang Vàng 2s
                  vert_status = STATE_YELLOW;
                  count_vert = 2;
                  break;

              case STATE_YELLOW:   // Hết Vàng 2s -> Sang Xanh 3s
                  vert_status = STATE_GREEN;
                  count_vert = 3;
                  break;

              case STATE_GREEN:    // Hết Xanh 3s -> Sang Đỏ 5s
                  vert_status = STATE_RED;
                  count_vert = 5;
                  break;
          }
      }

      // --- Chuyển trạng thái Độc Lập cho Hướng Ngang: GREEN(3s) -> YELLOW(2s) -> RED(5s) ---
      if (count_horiz <= 0) {
          switch (horiz_status) {
              case STATE_GREEN:    // Hết Xanh 3s -> Sang Vàng 2s
                  horiz_status = STATE_YELLOW;
                  count_horiz = 2;
                  break;

              case STATE_YELLOW:   // Hết Vàng 2s -> Sang Đỏ 5s
                  horiz_status = STATE_RED;
                  count_horiz = 5;
                  break;

              case STATE_RED:      // Hết Đỏ 5s -> Sang Xanh 3s
                  horiz_status = STATE_GREEN;
                  count_horiz = 3;
                  break;
          }
      }
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
  * @brief GPIO Initialization Function
  * @param None
  * @retval None
  */
static void MX_GPIO_Init(void)
{
  GPIO_InitTypeDef GPIO_InitStruct = {0};

  /* GPIO Ports Clock Enable */
  __HAL_RCC_GPIOA_CLK_ENABLE();
  __HAL_RCC_GPIOB_CLK_ENABLE();

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOA, GPIO_PIN_4|LED_RED_Pin|LED_YELLOW_Pin|LED_GREEN_Pin
                          |GPIO_PIN_8|GPIO_PIN_9|GPIO_PIN_10|GPIO_PIN_11
                          |GPIO_PIN_12|GPIO_PIN_13|GPIO_PIN_14|GPIO_PIN_15, GPIO_PIN_RESET);

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOB, GPIO_PIN_0|GPIO_PIN_1|GPIO_PIN_2|GPIO_PIN_10
                          |GPIO_PIN_11|GPIO_PIN_12|GPIO_PIN_13|GPIO_PIN_3
                          |GPIO_PIN_4|GPIO_PIN_5|GPIO_PIN_6|GPIO_PIN_7
                          |GPIO_PIN_8|GPIO_PIN_9, GPIO_PIN_RESET);

  /*Configure GPIO pins : PA4 LED_RED_Pin LED_YELLOW_Pin LED_GREEN_Pin
                           PA8 PA9 PA10 PA11
                           PA12 PA13 PA14 PA15 */
  GPIO_InitStruct.Pin = GPIO_PIN_4|LED_RED_Pin|LED_YELLOW_Pin|LED_GREEN_Pin
                          |GPIO_PIN_8|GPIO_PIN_9|GPIO_PIN_10|GPIO_PIN_11
                          |GPIO_PIN_12|GPIO_PIN_13|GPIO_PIN_14|GPIO_PIN_15;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

  /*Configure GPIO pins : PB0 PB1 PB2 PB10
                           PB11 PB12 PB13 PB3
                           PB4 PB5 PB6 PB7
                           PB8 PB9 */
  GPIO_InitStruct.Pin = GPIO_PIN_0|GPIO_PIN_1|GPIO_PIN_2|GPIO_PIN_10
                          |GPIO_PIN_11|GPIO_PIN_12|GPIO_PIN_13|GPIO_PIN_3
                          |GPIO_PIN_4|GPIO_PIN_5|GPIO_PIN_6|GPIO_PIN_7
                          |GPIO_PIN_8|GPIO_PIN_9;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

}

/* USER CODE BEGIN 4 */
unsigned char seg_code[10] = {
    0x40, 0x79, 0x24, 0x30, 0x19, 
    0x12, 0x02, 0x78, 0x00, 0x10
};

// 1. Hiển thị đếm ngược cho Hướng D�?c (LED trên: PB0 - PB6)
void display7SEG_Vertical(int num) {
    if (num >= 0 && num <= 9) {
        unsigned char code = seg_code[num];
        for (int i = 0; i < 7; i++) {
            HAL_GPIO_WritePin(GPIOB, (GPIO_PIN_0 << i), (code & (1 << i)) ? GPIO_PIN_SET : GPIO_PIN_RESET);
        }
    }
}

// 2. Hiển thị đếm ngược cho Hướng Ngang (LED dưới: PB7 - PB13)
void display7SEG_Horizontal(int num) {
    if (num >= 0 && num <= 9) {
        unsigned char code = seg_code[num];
        for (int i = 0; i < 7; i++) {
            HAL_GPIO_WritePin(GPIOB, (GPIO_PIN_7 << i), (code & (1 << i)) ? GPIO_PIN_SET : GPIO_PIN_RESET);
        }
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
