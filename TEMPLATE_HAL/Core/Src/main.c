
#include "main.h"
#include <stdint.h>

#define RX_BUF_SIZE 10



uint8_t rx_buffer[RX_BUF_SIZE]; // Mảng chứa data trong RAM
UART_HandleTypeDef huart2;
DMA_HandleTypeDef hdma_usart2_rx;

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
  MX_USART2_UART_Init();
  /* USER CODE BEGIN 2 */
  MX_DMA_Init();
  Start_UART_DMA_Receive();
  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
      // Truyền chuỗi "chau\r\n" (6 byte tính cả \r\n)
      if (HAL_UART_Transmit(&huart2, (uint8_t*)"chau\r\n", 6, 5000) == HAL_OK) {
          HAL_GPIO_TogglePin(GPIOD, GPIO_PIN_15); // Nháy LED mỗi khi truyền xong
      }
      HAL_Delay(1000); 
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

  /** Cấu hình điện áp đầu ra bộ điều chỉnh nội bộ */
  __HAL_RCC_PWR_CLK_ENABLE();
  __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE1);

  /** 1. Cấu hình HSI và PLL */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.HSICalibrationValue = RCC_HSICALIBRATION_DEFAULT;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSI; // Nguồn là HSI (16MHz)
  RCC_OscInitStruct.PLL.PLLM = 8;                      // 16MHz / 8 = 2MHz
  RCC_OscInitStruct.PLL.PLLN = 100;                    // 2MHz * 100 = 200MHz
  RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV4;          // 200MHz / 4 = 50MHz (SYSCLK)
  RCC_OscInitStruct.PLL.PLLQ = 4;                      // 200MHz / 4 = 50MHz (Cho USB/SDIO)
  
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }

  /** 2. Cấu hình các bus (HCLK, PCLK1, PCLK2) */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK; // Chạy bằng PLL
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;        // HCLK = 50MHz
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV2;         // PCLK1 = 25MHz (Dành cho UART2)
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;         // PCLK2 = 50MHz

  /* QUAN TRỌNG: Với 50MHz, FLASH_LATENCY phải là 1 Wait State */
  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_1) != HAL_OK)
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

  /* 1. Bật Clock cho các Port cần thiết */
  __HAL_RCC_GPIOH_CLK_ENABLE();
  __HAL_RCC_GPIOD_CLK_ENABLE();
  __HAL_RCC_GPIOA_CLK_ENABLE(); // Quan trọng: Phải bật clock cho GPIOA

  /* 2. Cấu hình LED trên PD15 (Code cũ của bạn) */
  HAL_GPIO_WritePin(GPIOD, GPIO_PIN_15, GPIO_PIN_RESET);
  GPIO_InitStruct.Pin = GPIO_PIN_15;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOD, &GPIO_InitStruct);

  /* 3. Cấu hình UART2 trên PA2 (TX) và PA3 (RX) */
  GPIO_InitStruct.Pin = GPIO_PIN_2 | GPIO_PIN_3;
  GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;      // Chế độ Alternate Function
  GPIO_InitStruct.Pull = GPIO_PULLUP;         // Thường dùng Pull-up cho UART để tránh nhiễu khi rảnh
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
  GPIO_InitStruct.Alternate = GPIO_AF7_USART2; // Kết nối chân vật lý với bộ UART2
  HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

  
}




void MX_USART2_UART_Init(void)
{
  huart2.Instance = USART2;
  huart2.Init.BaudRate = 9600;
  huart2.Init.WordLength = UART_WORDLENGTH_8B;
  huart2.Init.StopBits = UART_STOPBITS_1;
  huart2.Init.Parity = UART_PARITY_NONE;
  huart2.Init.Mode = UART_MODE_TX_RX;      // Bật cả truyền và nhận
  huart2.Init.HwFlowCtl = UART_HWCONTROL_NONE;
  huart2.Init.OverSampling = UART_OVERSAMPLING_16;
  huart2.RxCpltCallback = dma_handler_callback;
  
  if (HAL_UART_Init(&huart2) != HAL_OK)
  {
    Error_Handler();
  }
}



void Start_UART_DMA_Receive(void)
{
  /* Lệnh này thực hiện 3 việc:
     1. Gán địa chỉ đích (rx_buffer) vào thanh ghi CMAR của DMA.
     2. Ghi số lượng (RX_BUF_SIZE) vào thanh ghi CNDTR.
     3. Bật bit DMAR trong UART2 để "mở cổng" đẩy data sang DMA.
  */
  HAL_UART_Receive_DMA(&huart2, rx_buffer, RX_BUF_SIZE);
}

static void MX_DMA_Init(void) 
{
  /* 1. Khai báo biến Handle cho DMA */

  /* 2. Bật Clock cho bộ điều khiển DMA */
  __HAL_RCC_DMA1_CLK_ENABLE();

  /* 3. Cấu hình các tham số cho DMA */
  hdma_usart2_rx.Instance = DMA1_Stream5;            // Ví dụ Stream cho UART2 RX trên dòng F4
  hdma_usart2_rx.Init.Channel = DMA_CHANNEL_4;        // Channel tương ứng với UART2
  

  // Hướng truyền: Từ Ngoại vi đến Bộ nhớ (Peripheral to Memory)
  hdma_usart2_rx.Init.Direction = DMA_PERIPH_TO_MEMORY; 
  
  // Không tự động tăng địa chỉ ngoại vi (vì thanh ghi DR của UART là cố định)
  hdma_usart2_rx.Init.PeriphInc = DMA_PINC_DISABLE;   
  
  // Tự động tăng địa chỉ bộ nhớ (để lưu dữ liệu vào mảng/buffer)
  hdma_usart2_rx.Init.MemInc = DMA_MINC_ENABLE;       
  
  // Định dạng dữ liệu: 8-bit (Byte)
  hdma_usart2_rx.Init.PeriphDataAlignment = DMA_PDATAALIGN_BYTE;
  hdma_usart2_rx.Init.MemDataAlignment = DMA_MDATAALIGN_BYTE; 
  
  // Chế độ: DMA_MODE_NORMAL (dừng khi đủ) hoặc DMA_MODE_CIRCULAR (vòng lặp liên tục)
  hdma_usart2_rx.Init.Mode = DMA_CIRCULAR;       
  
  // Ưu tiên: Thấp/Trung bình/Cao
  hdma_usart2_rx.Init.Priority = DMA_PRIORITY_LOW;    
  
  // Chế độ FIFO (thường tắt để tiết kiệm năng lượng cho các tác vụ đơn giản)
  hdma_usart2_rx.Init.FIFOMode = DMA_FIFOMODE_DISABLE;


  HAL_NVIC_SetPriority(DMA1_Stream5_IRQn, 0, 0); 
  HAL_NVIC_EnableIRQ(DMA1_Stream5_IRQn);

  /* 4. Gọi hàm Init để áp dụng cấu hình */
  if (HAL_DMA_Init(&hdma_usart2_rx) != HAL_OK)
  {
    // Xử lý lỗi nếu khởi tạo thất bại
    Error_Handler();
  }

}

void dma_handler_callback( struct __UART_HandleTypeDef * hdma) {
    for (int i = 0; i < RX_BUF_SIZE; i++)
    {
      RTT_printf("Arr = %s", rx_buffer);
    }
    
    RTT_printf("\n");
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
