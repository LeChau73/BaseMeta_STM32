#include "main.h"
#include <stdarg.h>

UART_HandleTypeDef huart2;
DMA_HandleTypeDef hdma_usart2_tx;

#define MAX_BUFFER 1024

char source[MAX_BUFFER];
char destination[MAX_BUFFER];




void DMA1_Stream6_IRQHandler() {

    HAL_DMA_IRQHandler(&hdma_usart2_tx);
    LOG_Message("Hiiii DMA ISR Stream 0");
}


void test_load_and_exclusive(int* a_modify, int a) {
    int value_read; // Biến C để nhận kết quả
    int result;
    // operand 2 (%1) phải là 1 địa chỉ(tức là phải là 1 con trỏ)             //clobbers
    // output sẽ được đặt vào register với = là ràng buộc input phải được load và register, và output write và thanh ghi
    __asm__ volatile ("ldrex %0, [%1]" : "=r" (value_read) : "r" (a_modify) : "memory"); // __asm__ volatile ("strex %0, %1, [%2]" : "=r" (result) : "r" (a) , "r"(a_modify) : "memory");


    //LOG_Message("value_read = %d\n", value_read); //BUG: gọi ISR của UART store fail
    value_read = a;// chỉ gọi các phép toán học

    __asm__ volatile ("strex %0, %1, [%2]" : "=&r" (result) : "r" (value_read) , "r"(a_modify) : "memory");

    LOG_Message("Result = %d | a_modify value = %d\n", result,  *a_modify);

}

// Adapter for uart to connected with interface of higher layer
static void transport_uart(char* data,int len) {
    HAL_UART_Transmit(&huart2, data, len, 500);
}

int main(void) 
{
  /* For log */
    //ITM_Init(false);
    SEGGER_RTT_Init();
    HAL_Init();
    SystemClock_Config();
    MX_GPIO_Init();
    RTT_printf("=====Hello RTT!=====\n");     //Không dùng đượmakec do dump thanh ghi của SVD
    MX_DMA_Init();
    MX_USART2_UART_Init();

    

    // HAL_SetSpecific_Time(2, TimeUnit.mS);

    LOG_Register(transport_uart); //Register type of output log

    LOG_Message("------------ Init log UART --------------\n");   //No user DMA

    int a = 5;
    test_load_and_exclusive(&a, 7);

    //Test UART
    ringBufS ring1;
    
    ringBufS_Init(&ring1);

    STATUS_RING status_r = ringBuf_put(&ring1, 'h');

    char temp_buff[14] = "le hong chau";

    ringBuf_push(&ring1, temp_buff, 14);

    for(int i = 0; i < 10; i++) {
        char c = ringBuf_get(&ring1);
       LOG_Message("ring mes = %c", c);
    }


    ATOMIC_SET_BIT(huart2.Instance->CR3, USART_CR3_DMAT);

    SET_BIT(huart2.Instance->CR3, USART_CR3_DMAT);


    char test[] = "Dummy DMA";
    HAL_UART_Transmit_DMA(&huart2, test, 8);

    //Chỗ này sau khi DMA chưa gửi dữ liệu qua cho UART được
    uint32_t *regs = (uint32_t *)hdma_usart2_tx.StreamBaseAddress;
    LOG_Message("Value of %x\n", *regs);




  /* end */
    DMA_Config_Mem_to_Mem();

    dma_mem_copy(source, destination, MAX_BUFFER);
    LOG_Message("Time Systick =  %d\n", HAL_GetTick());
    while (1) {
        for (volatile int i = 0; i < 1000000; i++); // Delay giả lập
        led_on(12);
        for (volatile int i = 0; i < 1000000; i++); // Delay giả lập
        led_off(12);
        //SEGGER_RTT_WriteString(0, "Hello RTTsdsad!\n");
    }

    return 0;
}


void SysTick_Handler(void) {
    HAL_IncTick();
}


void SystemClock_Config(void)
{
  RCC_OscInitTypeDef RCC_OscInitStruct = {0};
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};

  __HAL_RCC_PWR_CLK_ENABLE();
  __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE1);

  // PLL config
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.HSICalibrationValue = RCC_HSICALIBRATION_DEFAULT;

  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSI;

  RCC_OscInitStruct.PLL.PLLM = 16;
  RCC_OscInitStruct.PLL.PLLN = 336;
  RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV4; // 84 MHz
  RCC_OscInitStruct.PLL.PLLQ = 7;

  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
    Error_Handler();

  // Bus config
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK |
                                RCC_CLOCKTYPE_SYSCLK |
                                RCC_CLOCKTYPE_PCLK1 |
                                RCC_CLOCKTYPE_PCLK2;

  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;

  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;   // 84 MHz
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV2;    // 42 MHz
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;    // 84 MHz

  // ⚠️ QUAN TRỌNG
  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_2) != HAL_OK)
    Error_Handler();
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
  __HAL_RCC_USART2_CLK_ENABLE();
  huart2.Instance = USART2;
  huart2.Init.BaudRate = 9600;
  huart2.Init.WordLength = UART_WORDLENGTH_8B;
  huart2.Init.StopBits = UART_STOPBITS_1;
  huart2.Init.Parity = UART_PARITY_NONE;
  huart2.Init.Mode = UART_MODE_TX_RX;      // Bật cả truyền và nhận
  huart2.Init.HwFlowCtl = UART_HWCONTROL_NONE;
  huart2.Init.OverSampling = UART_OVERSAMPLING_16;
  huart2.hdmatx = &hdma_usart2_tx;
  huart2.gState = HAL_UART_STATE_READY;
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
  hdma_usart2_tx.Instance = DMA1_Stream6;            // Ví dụ Stream cho UART2 RX trên dòng F4
  hdma_usart2_tx.Init.Channel = DMA_CHANNEL_4;        // Channel tương ứng với UART2
  

  // Hướng truyền: Từ Ngoại vi đến Bộ nhớ (Peripheral to Memory)
  hdma_usart2_tx.Init.Direction = DMA_MEMORY_TO_PERIPH; 
  
  // Không tự động tăng địa chỉ ngoại vi (vì thanh ghi DR của UART là cố định)
  hdma_usart2_tx.Init.PeriphInc = DMA_PINC_DISABLE;   
  
  // Tự động tăng địa chỉ bộ nhớ (để lưu dữ liệu vào mảng/buffer)
  hdma_usart2_tx.Init.MemInc = DMA_MINC_ENABLE;       
  
  // Định dạng dữ liệu: 8-bit (Byte)
  hdma_usart2_tx.Init.PeriphDataAlignment = DMA_PDATAALIGN_BYTE;
  hdma_usart2_tx.Init.MemDataAlignment = DMA_MDATAALIGN_BYTE; 
  
  // Chế độ: DMA_MODE_NORMAL (dừng khi đủ) hoặc DMA_MODE_CIRCULAR (vòng lặp liên tục)
  hdma_usart2_tx.Init.Mode = DMA_NORMAL;       
  
  // Ưu tiên: Thấp/Trung bình/Cao
  hdma_usart2_tx.Init.Priority = DMA_PRIORITY_VERY_HIGH;    
  
  // Chế độ FIFO (thường tắt để tiết kiệm năng lượng cho các tác vụ đơn giản)
  hdma_usart2_tx.Init.FIFOMode = DMA_FIFOMODE_DISABLE;

  hdma_usart2_tx.Parent = &huart2;


  HAL_NVIC_SetPriority(DMA1_Stream6_IRQn, 0, 0); 
  HAL_NVIC_EnableIRQ(DMA1_Stream6_IRQn);

  /* 4. Gọi hàm Init để áp dụng cấu hình */
  if (HAL_DMA_Init(&hdma_usart2_tx) != HAL_OK)
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

__attribute__((naked))
void HardFault_Handler(void)
{
  // Xác định đang dùng PSP hay MSP
  // tst : là lệnh and lr với 0x4 ,và cập nhập Z (Zero flag) : nếu sau and = 0 thì Z = 1.
  __asm volatile (
        " tst lr, #4              \n" // Kiểm tra bit 2 của EXC_RETURN (LR), nếu bằng 0 thì Z flag bằng 1
        " ite eq                  \n" // Nếu bit 2 = 0 (đang dùng MSP) , so sánh với Z llag
        " mrseq r0, msp           \n" // nạp MSP vào R0 làm tham số thứ 1 , nếu Z lag bằng 1,tức là bit 2 của lr = 0 thì nap msp vào r0
        " mrsne r0, psp           \n" // Nếu bit 2 = 1 (đang dùng PSP), nạp PSP vào R0, ngược lại
        " ldr r1, =HardFault_Decoder \n" // Nạp địa chỉ hàm xử lý logic
        " bx r1                   \n" // Nhảy đến hàm xử lý
    );
}


// 1. Hàm C xử lý logic (nhận tham số từ R0)
void HardFault_Decoder(uint32_t *pStack) {
    //RTT_LOG_RED("------- HARD FAULT ------\n");

    uint32_t stacked_r0  = pStack[0];
    uint32_t stacked_r1  = pStack[1];
    uint32_t stacked_r2  = pStack[2];
    uint32_t stacked_r3  = pStack[3];
    uint32_t stacked_r12 = pStack[4];
    uint32_t stacked_lr  = pStack[5];   // LR cũ (thường là EXC_RETURN)
    uint32_t stacked_pc  = pStack[6];   // ← Đây mới là PC gây lỗi
    uint32_t stacked_psr = pStack[7];

    //RTT_printf("R0  = 0x%08X\n", stacked_r0);
    //RTT_printf("R1  = 0x%08X\n", stacked_r1);
    //RTT_printf("R2  = 0x%08X\n", stacked_r2);
    //RTT_printf("R3  = 0x%08X\n", stacked_r3);
    //RTT_printf("R12 = 0x%08X\n", stacked_r12);
    //RTT_printf("LR  = 0x%08X\n", stacked_lr);
    //RTT_printf("PC  = 0x%08X  ←←← LỆNH GÂY LỖI Ở ĐÂY\n", stacked_pc);
    //RTT_printf("PSR = 0x%08X\n", stacked_psr);

    

    LOG_Message("R0  = %x\n", stacked_r0);
    LOG_Message("R1  = %x\n", stacked_r1);
    LOG_Message("R2  = %x\n", stacked_r2);
    LOG_Message("R3  = %x\n", stacked_r3);
    LOG_Message("R12 = %x\n", stacked_r12);
    LOG_Message("LR  = %x\n", stacked_lr);
    LOG_Message("PC  = %x  Instruction error this here\n", stacked_pc);
    LOG_Message("PSR = %x\n", stacked_psr);

    LOG_Message("HFSR = %x\n", SCB->HFSR);
    LOG_Message("CFSR = %x\n", SCB->CFSR);
    LOG_Message("BFAR = %x\n", SCB->BFAR);
    LOG_Message("MMFAR= %x\n", SCB->MMFAR);
    //TODO: implement lưu vào flash
    
    while(1);
}

void EXTI9_5_IRQHandler(void) {
    LOG_REG_COLOR1(EXTI->PR);

    EXTI->PR &= ~(1 << 5);
    EXTI->PR &= ~(1 << 6);

    LOG_REG_COLOR1(EXTI->PR);
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