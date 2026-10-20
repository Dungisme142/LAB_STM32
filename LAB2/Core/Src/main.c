/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
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

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */

/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
TIM_HandleTypeDef htim2;

/* USER CODE BEGIN PV */
// Chuỗi Font: Chữ 'A' (8 cột) + 8 cột trống (0x00)
const uint8_t font_sequence[] = {
    0x18, 0x3C, 0x66, 0x66, 0x7E, 0x66, 0x66, 0x00, // Chữ 'A'
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00  // Khoảng trống 8 cột
};

#define SEQUENCE_LEN (sizeof(font_sequence) / sizeof(font_sequence[0]))

uint8_t matrix_buffer[8] = {0}; // Đệm 8 cột hiển thị LED Matrix
int shift_offset = 0;           // Chỉ số trượt khung ảnh

// Mảng tra cứu chân quét Cột ENM0 -> ENM7 (PA2-PA3, PA10-PA15)
const uint16_t ENM_PIN[8] = {
    GPIO_PIN_2,  GPIO_PIN_3,  GPIO_PIN_10, GPIO_PIN_11,
    GPIO_PIN_12, GPIO_PIN_13, GPIO_PIN_14, GPIO_PIN_15
};

// Mảng tra cứu chân điều khiển PNP Transistor của 4 LED 7 đoạn (PA5, PA6, PA7, PA8)
const uint16_t SEG_EN_PIN[4] = {
    GPIO_PIN_5, GPIO_PIN_6, GPIO_PIN_7, GPIO_PIN_8
};

// Mảng mã 7 đoạn Anode chung (Mức 0 = SÁNG) cho các số 1, 2, 3, 4
const uint8_t seg7_code[10] = {
    0x40, 0x79, 0x24, 0x30, 0x19, 0x12, 0x02, 0x78, 0x00, 0x10
};
int seg7_buffer[4] = {1, 2, 3, 4}; // Hiển thị "1234" lên 4 LED 7 đoạn
int index_7seg = 0;                // Chỉ số quét LED 7 đoạn (0 -> 3)

// Software Timer Global Variables
// flag[0..7]: 8 cột LED Matrix
// flag[8]: Quét 4 LED 7 đoạn
// flag[9]: Chớp tắt LED D2, D3 (chân DOT / PA4)
// flag[10]: Dịch chuyển chữ A
int timer_counter[12] = {0};
int timer_flag[12] = {0};
/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_TIM2_Init(void);

/* USER CODE BEGIN PFP */
void setTimer(int index, int duration);
void timerRun(void);
void updateAnimationBuffer(void);
void updateLEDMatrix(int index);
void update7Segment(int index);
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
  /* MCU Configuration--------------------------------------------------------*/
  HAL_Init();
  SystemClock_Config();

  /* Initialize all configured peripherals */
  MX_GPIO_Init();
  MX_TIM2_Init();

  /* USER CODE BEGIN 2 */
  HAL_TIM_Base_Start_IT(&htim2); // Kích hoạt Hardware Timer 2

  updateAnimationBuffer(); // Khởi tạo dữ liệu đệm ban đầu cho chữ A

  // Khởi tạo các Timer
  setTimer(0, 1);    // Timer 0: Bắt đầu chuỗi quét 8 cờ LED Matrix
  setTimer(8, 25);    // Timer 8: Quét 4 LED 7-segment (mỗi 250ms với TIM2 100Hz/10ms)
  setTimer(9, 50);   // Timer 9: Chớp tắt LED D2, D3 (500ms)
  setTimer(10, 15);  // Timer 10: Dịch chữ A sang phải (150ms)
  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
      // =========================================================================
      // 1. QUÉT MA TRẬN LED BẰNG 8 CỜ (timer_flag[0] -> timer_flag[7])
      // =========================================================================
      for (int i = 0; i < 8; i++) {
          if (timer_flag[i] == 1) {
              updateLEDMatrix(i); // Hiển thị cột thứ i
              timer_flag[i] = 0;  // Xóa cờ

              // Đặt Timer 10ms (1 đơn vị timer với TIM2 100Hz) cho cột tiếp theo
              int next_col = (i + 1) % 8;
              setTimer(next_col, 1);
          }
      }

      // =========================================================================
      // 2. QUÉT HIỂN THỊ 4 LED 7 ĐOẠN (timer_flag[8])
      // =========================================================================
      if (timer_flag[8] == 1) {
          setTimer(8, 25); // Quét liên tục giữa các LED 7 đoạn

          update7Segment(index_7seg);
          index_7seg = (index_7seg + 1) % 4;
      }

      // =========================================================================
      // 3. CHỚP TẮT LED D2, D3 QUA CHÂN DOT (PA4) (timer_flag[9])
      // =========================================================================
      if (timer_flag[9] == 1) {
          setTimer(9, 50); // Lặp lại mỗi 500ms
          HAL_GPIO_TogglePin(GPIOA, GPIO_PIN_4);
      }

      // =========================================================================
      // 4. DỊCH CHỮ 'A' TỪ TRÁI SANG PHẢI (timer_flag[10])
      // =========================================================================
      if (timer_flag[10] == 1) {
          setTimer(10, 15); // Dịch 1 bước mỗi 150ms

          shift_offset++;
          if (shift_offset >= SEQUENCE_LEN) {
              shift_offset = 0; // Quay lại ban đầu sau khi trượt hết
          }

          updateAnimationBuffer(); // Cập nhật khung hình mới
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

  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.HSICalibrationValue = RCC_HSICALIBRATION_DEFAULT;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_NONE;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }

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
  TIM_ClockConfigTypeDef sClockSourceConfig = {0};
  TIM_MasterConfigTypeDef sMasterConfig = {0};

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
  HAL_GPIO_WritePin(GPIOA, GPIO_PIN_2|GPIO_PIN_3|GPIO_PIN_4|GPIO_PIN_5
                          |GPIO_PIN_6|GPIO_PIN_7|GPIO_PIN_8|GPIO_PIN_9
                          |GPIO_PIN_10|GPIO_PIN_11|GPIO_PIN_12|GPIO_PIN_13
                          |GPIO_PIN_14|GPIO_PIN_15, GPIO_PIN_RESET);

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOB, GPIO_PIN_0|GPIO_PIN_1|GPIO_PIN_2|GPIO_PIN_10
                          |GPIO_PIN_11|GPIO_PIN_12|GPIO_PIN_13|GPIO_PIN_14
                          |GPIO_PIN_15|GPIO_PIN_3|GPIO_PIN_4|GPIO_PIN_5
                          |GPIO_PIN_6|GPIO_PIN_8|GPIO_PIN_9, GPIO_PIN_RESET);

  /*Configure GPIO pins : PA2 PA3 PA4 PA5 PA6 PA7 PA8 PA9 PA10 PA11 PA12 PA13 PA14 PA15 */
  GPIO_InitStruct.Pin = GPIO_PIN_2|GPIO_PIN_3|GPIO_PIN_4|GPIO_PIN_5
                          |GPIO_PIN_6|GPIO_PIN_7|GPIO_PIN_8|GPIO_PIN_9
                          |GPIO_PIN_10|GPIO_PIN_11|GPIO_PIN_12|GPIO_PIN_13
                          |GPIO_PIN_14|GPIO_PIN_15;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

  /*Configure GPIO pins : PB0 PB1 PB2 PB3 PB4 PB5 PB6 PB8 PB9 PB10 PB11 PB12 PB13 PB14 PB15 */
  GPIO_InitStruct.Pin = GPIO_PIN_0|GPIO_PIN_1|GPIO_PIN_2|GPIO_PIN_10
                          |GPIO_PIN_11|GPIO_PIN_12|GPIO_PIN_13|GPIO_PIN_14
                          |GPIO_PIN_15|GPIO_PIN_3|GPIO_PIN_4|GPIO_PIN_5
                          |GPIO_PIN_6|GPIO_PIN_8|GPIO_PIN_9;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);
}

/* USER CODE BEGIN 4 */
// 1. Cập nhật đệm hiển thị chữ 'A' trượt từ Trái sang Phải
void updateAnimationBuffer(void) {
    for (int i = 0; i < 8; i++) {
        int font_idx = (SEQUENCE_LEN - shift_offset + i) % SEQUENCE_LEN;
        matrix_buffer[i] = font_sequence[font_idx];
    }
}

// 2. Quét Ma trận LED 8x8 (Cột PA, Hàng PB8-PB15)
void updateLEDMatrix(int index) {
    // Tắt tất cả cột chống bóng mờ
    for (int i = 0; i < 8; i++) {
        HAL_GPIO_WritePin(GPIOA, ENM_PIN[i], GPIO_PIN_RESET);
    }

    // Xuất dữ liệu Hàng ra PB8 -> PB15
    uint8_t row_data = matrix_buffer[index];
    for (int bit = 0; bit < 8; bit++) {
        GPIO_PinState pin_state = (row_data & (1 << bit)) ? GPIO_PIN_SET : GPIO_PIN_RESET;
        HAL_GPIO_WritePin(GPIOB, (GPIO_PIN_8 << bit), pin_state);
    }

    // Bật Cột hiện tại (Ngõ vào ULN2803 = HIGH)
    HAL_GPIO_WritePin(GPIOA, ENM_PIN[index], GPIO_PIN_SET);
}

// 3. Quét 4 LED 7-Segment qua PNP Transistor (EN0-EN3: PA5-PA8)
void update7Segment(int index) {
    // Tắt tất cả PNP Transistor (Kéo HIGH chân PA5 -> PA8) để chống nhòe
    for (int i = 0; i < 4; i++) {
        HAL_GPIO_WritePin(GPIOA, SEG_EN_PIN[i], GPIO_PIN_SET);
    }

    // Xuất mã 7 đoạn ra PB0 -> PB6
    uint8_t code = seg7_code[seg7_buffer[index]];
    for (int bit = 0; bit < 7; bit++) {
        GPIO_PinState pin_state = (code & (1 << bit)) ? GPIO_PIN_SET : GPIO_PIN_RESET;
        HAL_GPIO_WritePin(GPIOB, (GPIO_PIN_0 << bit), pin_state);
    }

    // Bật PNP Transistor tương ứng (Kéo LOW ngõ ra PA5/PA6/PA7/PA8)
    HAL_GPIO_WritePin(GPIOA, SEG_EN_PIN[index], GPIO_PIN_RESET);
}

// 4. Các hàm Software Timer
void setTimer(int index, int duration) {
    timer_counter[index] = duration;
    timer_flag[index] = 0;
}

void timerRun(void) {
    for (int i = 0; i < 12; i++) {
        if (timer_counter[i] > 0) {
            timer_counter[i]--;
            if (timer_counter[i] <= 0) {
                timer_flag[i] = 1;
            }
        }
    }
}

void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim) {
    if (htim->Instance == TIM2) {
        timerRun();
    }
}
/* USER CODE END 4 */

void Error_Handler(void)
{
  __disable_irq();
  while (1)
  {
  }
}
