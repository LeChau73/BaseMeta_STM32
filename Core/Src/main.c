#include "main.h"
#include <stdarg.h>

UART_HandleTypeDef huart2;
DMA_HandleTypeDef hdma_usart2_rx;


int main(void) 
{
    HAL_Init();
    SystemClock_Config();

    MX_GPIO_Init();
    MX_USART2_UART_Init();

    MX_DMA_Init();

    SEGGER_RTT_Init();
    RTT_LOG_BRIGHT_RED("=====Hello RTT!=====\n");

    while (1) {


        for (volatile int i = 0; i < 1000000; i++); // Delay giả lập
        led_on(12);
        for (volatile int i = 0; i < 1000000; i++); // Delay giả lập
        led_off(12);
        //SEGGER_RTT_WriteString(0, "Hello RTTsdsad!\n");
    }

    return 0;
}
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
  //huart2.RxCpltCallback = dma_handler_callback;
  
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
  //HAL_UART_Receive_DMA(&huart2, rx_buffer, RX_BUF_SIZE);
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


void HardFault_Handler(uint32_t *pStack)
{
    RTT_LOG_RED("------- HARD FAULT ------\n");

    uint32_t stacked_r0  = pStack[0];
    uint32_t stacked_r1  = pStack[1];
    uint32_t stacked_r2  = pStack[2];
    uint32_t stacked_r3  = pStack[3];
    uint32_t stacked_r12 = pStack[4];
    uint32_t stacked_lr  = pStack[5];   // LR cũ (thường là EXC_RETURN)
    uint32_t stacked_pc  = pStack[6];   // ← Đây mới là PC gây lỗi
    uint32_t stacked_psr = pStack[7];

    RTT_printf("R0  = 0x%08X\n", stacked_r0);
    RTT_printf("R1  = 0x%08X\n", stacked_r1);
    RTT_printf("R2  = 0x%08X\n", stacked_r2);
    RTT_printf("R3  = 0x%08X\n", stacked_r3);
    RTT_printf("R12 = 0x%08X\n", stacked_r12);
    RTT_printf("LR  = 0x%08X\n", stacked_lr);
    RTT_printf("PC  = 0x%08X  ←←← LỆNH GÂY LỖI Ở ĐÂY\n", stacked_pc);
    RTT_printf("PSR = 0x%08X\n", stacked_psr);

    myPrintf("R0  = 0x%x\n", stacked_r0);
    myPrintf("R1  = 0x%x\n\n", stacked_r1);
    myPrintf("R2  = 0x%x\n\n", stacked_r2);
    myPrintf("R3  = 0x%x\n", stacked_r3);
    myPrintf("R12 = 0x%x\n", stacked_r12);
    myPrintf("LR  = 0x%x\n", stacked_lr);
    myPrintf("PC  = 0x%x  ←←← LỆNH GÂY LỖI Ở ĐÂY\n", stacked_pc);
    myPrintf("PSR = 0x%x\n", stacked_psr);

    // In thêm SP hiện tại (MSP hoặc PSP tùy mode)
    RTT_printf("Stacked SP  = 0x%08X  (tức là địa chỉ pStack)\n", (uint32_t)pStack);

    // In các thanh ghi fault
    RTT_printf("HFSR = 0x%08X\n", SCB->HFSR);
    RTT_printf("CFSR = 0x%08X\n", SCB->CFSR);
    RTT_printf("BFAR = 0x%08X\n", SCB->BFAR);
    RTT_printf("MMFAR= 0x%08X\n", SCB->MMFAR);

    myPrintf("HFSR = 0x%x\n", SCB->HFSR);
    myPrintf("CFSR = 0x%x\n", SCB->CFSR);
    myPrintf("BFAR = 0x%x\n", SCB->BFAR);
    myPrintf("MMFAR= 0x%x\n", SCB->MMFAR);
    //TODO: implement lưu vào flash
    
    while(1) {};
}

void EXTI9_5_IRQHandler(void) {
    LOG_REG_COLOR1(EXTI->PR);

    EXTI->PR &= ~(1 << 5);
    EXTI->PR &= ~(1 << 6);

    LOG_REG_COLOR1(EXTI->PR);
}

void DWT_DataMaching(int value,volatile void* addrOfValue)
{
    *(volatile uint32_t *)0xE0000FB0 = 0xC5ACCE55; // DWT_LAR: Unlock DWT
    *(volatile uint32_t *)0xE0001000 |= (1 << 0); // DWT_CTRL: CYCCNTENA


    
    *(volatile uint32_t *)0xE0001014 = (uint32_t)addrOfValue; // DWT_COMP1: Địa chỉ sensor_data
    *(volatile uint32_t *)0xE0001018 = (value) | (0b10 << 0) | (1 << 2); // Data value match + EMITRANGE
    *(volatile uint32_t *)0xE0000E00 |= (1 << 1); // Enable ITM port 1 for DWT
    *(volatile uint32_t *)0xE0001024 |= (1 << 24); // Emit ITM event
}

void EXTI0_IRQHandler(void) {
    //__asm volatile ("SVC #1"); //BUG: Không gọi SVC trong IRQ 
    uint32_t reg_pr = EXTI->PR;
    reg_pr &= ~(1 << 0);
    EXTI->PR |= reg_pr;
}

// __get_MSP() is provided by CMSIS headers in core_cm4.h
// Use the CMSIS version directly

//naked :loại bỏ các lời gọi hàm trước cho compiler chèn vào
__attribute((naked)) 
void SVC_Handler(void) {
    uint32_t *sp = (uint32_t *)__get_MSP();  // Hoặc PSP nếu User mode
    uint32_t pc = sp[6];                     // PC khi gọi SVC ~ pc = 0x80000c
    //TODO:
    // Trước khi nhảy vào handler pc lưu lệnh sau đó: Vì PC đang trỏ đến lệnh sau SVC
    // → Lệnh SVC luôn có dạng: 2 bytes = 0xDF xx
    // Byte thấp: chính là svc_number (từ 0 đến 255)
    // SVC #3 → được assembler biên dịch thành: 0xDF 03
    uint32_t aircr = SCB->AIRCR; // kiểm tra MSB hay LSB
    uint8_t svc_number;

    if (aircr & (1 << 15)) {
        // Big-endian (hiếm gặp)
        svc_number = ((uint8_t *)pc) [-1];
    } else {
        // Little-endian (luôn là trường hợp này trên STM32F4)
        svc_number = ((uint8_t *)pc) [-2]; // Đọc byte cuối lệnh SVC (imm là byte 0)
    }

    
    switch (svc_number & 0xFF) {             // Mask để lấy imm (0-255)
        case 3:  // Xử lý cho SVC #3, ví dụ: custom function
            // Gọi hàm tương ứng, ví dụ: my_custom_syscall();
            break;
        // Các case khác...
    }
}

//void SVC_Handler(uint32_t* pStack) {
//
//
//    uint32_t stacked_r0  = pStack[0];
//    uint32_t stacked_r1  = pStack[1];
//    uint32_t stacked_r2  = pStack[2];
//    uint32_t stacked_r3  = pStack[3];
//    uint32_t stacked_r12 = pStack[4];
//    uint32_t stacked_lr  = pStack[5];   // LR cũ (thường là EXC_RETURN)
//    uint32_t stacked_pc  = pStack[6];   // ← Đây mới là PC gây lỗi
//    uint32_t stacked_psr = pStack[7];
//
//}

inline void ConfigClockHSE16MHZ()
{

    // Selector system clock source BIT 1:0
    SET_BIT(RCC_CFGR, 3); // pll select
    //SET_BIT(RCC_CFGR, 1); // SYSTEM CLOCK DIVIDED 


    // input => PLLM => Fvoc(clock) => PLLP
    //• f(VCO clock) = f(PLL clock input) × (PLLN / PLLM)
    //  128 = 8 * (PLLN(64) / PLLM(4))
    // PLLN = 64
    RCC_PLLCFGR |= 0x1000;
    // PLLM = 4
    RCC_PLLCFGR |= 0x4;
    // • f(PLL general clock output) = f(VCO clock) / PLLP
    //  16 = 128 / 8
    // PLLP = 8
    RCC_PLLCFGR |= 0x20000;

    //1: HSE oscillator clock selected as PLL and PLLI2S clock entry
    RCC_PLLCFGR |= 0x400000;

    // READ bit 25: PLLRDY: Main PLL (PLL) clock ready flag
    uint8_t a = (RCC_CR & (1 << 25));

    // HSERDY : status của HSE
    // HSEON : on to selecto HSE source
    //RCC_CR |= 

}