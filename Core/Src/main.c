#include "main.h"
#include <stdarg.h>


void onCompleteUart(void);

void onCompleteUart(void) {
    RTT_printf("Done write uart\n");
    //USART2->USART_CR1 &= ~(0x1 << 6); // TCIE = 1 bit 6 CR1 -> DISNABLE ISR
}

int main(void) 
{
    //
    func_receiverComplete = HAL_uart_receiver1byte;
    func_transHandler = uart_tran_hander_it;
    func_OverrunError = func_OverrunErrorHander;

    register_callback_write_complete(onCompleteUart);   //Đắng ký hàm call back && init struct uart

    SEGGER_RTT_Init();
    RTT_LOG_BRIGHT_RED("=====Hello RTT!=====\n");

    // Refer todo để check cây test
    //CreateTask("Task1", 4, 500, NULL, NULL);

    // __asm volatile ("SVC #3"); //BUG: Không gọi SVC trong IRQ
    //ITM_Init(false);
    //myPrintf("I am using ITM print for debug\n");
    //ITM_SendString("Hello");
   
    configGpio();

    usart_config config;
    config.length = 8;
    config.baudrate = 9600;
    config.configISR = ENABLE_TRANS | ENABLE_RECIVER;
    //config.configISR = DISNABLE_ALL;
    config.config_mode = ALL;
    char* buffer = "First";


    
    int status;

    status = HAL_uart_Init(&config);
    uart_write_it("Hello", 5, 1);
    //status = HAL_send_break_frame(1);
    //status = HAL_uart_tran1byte('c');
    status = HAL_uart_tranMul(buffer, 5);
    



    if(status == -1)
        RTT_printf("Errorr\n");

    //BUG: Chi enable line 1
    //TRIGGER_INTERRUPT_EVENT(EXTI_LINE_0);
    LOG_REG_COLOR1(EXTI->SWIER);
    LOG_REG_COLOR1(EXTI->PR);
    while (1) {
        //GPIO_Toogle( &(GPIO_Pin_t){GPIOD, GPIO_PIN_12 | GPIO_PIN_13 | GPIO_PIN_14 | GPIO_PIN_15} );

        for (volatile int i = 0; i < 1000000; i++); // Delay giả lập
        led_on(12);
        for (volatile int i = 0; i < 1000000; i++); // Delay giả lập
        led_off(12);
        //SEGGER_RTT_WriteString(0, "Hello RTTsdsad!\n");
    }

    return 0;
}

char buff[20];
char nextData = 'c';

// Handler UART2
void USART2_IRQHandler(void) {
    //Parse bit in register SR to noticed
    volatile uint32_t valueSR = USART2->USART_SR;
    LOG_REG_COLOR(USART2->USART_CR1);
    RTT_printf("CR: 0x%X\n", (1 << 6));
    uart_status status;
    uart_log_level level_log;
    RTT_printf("Mask: 0x%X\n", (1 << 6));
    RTT_printf("SR: 0x%X\n", valueSR);
    RTT_printf("Result: 0x%X\n", valueSR & (1 << 6));
    if  ( ( valueSR & (1 << 6) ) != 0 ) { // Kiểm tra cờ TC (Transmission Complete)

        //Trans comple

        status = func_transHandler();
        //Clear TXE
        if(status_ring_buffer(&uart_core) == EMPTY) {
            call_callback_uart();
            USART2->USART_CR1 &= ~(0x1 << 6); // TCIE = 1 bit 6 CR1 -> Clear Tiggle ISR
        }

    } else if ( (valueSR & (1 << 5) ) != 0  ) {

        status = func_receiverComplete(buff);

    } else if ( (valueSR & (1 << 3) ) != 0 ) {

        func_OverrunError(ERROR_UART_LOG);

    }
    LOG_REG_COLOR(USART2->USART_SR);
}


void configGpio()
{

    t_TCB debugTCB;
    strcpy(debugTCB.task_name,"task1");
    debugTCB.priority = 2;
    debugTCB.status = READY;
    int a = sizeof(debugTCB);





    //Enable clock
    volatile uint32_t* rcc_gpio = (volatile uint32_t*)RCC_AHB1ENR;
    *rcc_gpio |= RCC_GPIODEN;

    __HAL_RCC_GPIOH_CLK_ENABLE();
    __HAL_RCC_GPIOD_CLK_ENABLE();
    __HAL_RCC_GPIOC_CLK_ENABLE();
    __HAL_RCC_GPIOB_CLK_ENABLE();
    __HAL_RCC_GPIOA_CLK_ENABLE();

    GPIO_Pin_t gpio = { GPIOD , GPIO_PIN_12 | GPIO_PIN_13 | GPIO_PIN_14 | GPIO_PIN_15};
    gpio.port->OSPEEDR = (uint32_t)0x0000C000; //Case 1: không ảnh hưởng cũ


    GPIO_Config config = { OUTPUT_PP, 0, MEDIUM_SPEED, 0};
    GPIO_Init(&gpio , &config);

    // Test AF : Config UART 2 : PA2(TX) PA3(RX) AF7
    // DEBUG: đang debug
    GPIO_Config configAF;
    configAF.alternate = AF7; //UART 2
    configAF.mode = MODE_AF;
    configAF.pull = GPIO_NOPULL;
    configAF.speed = HIGH_SPEED;
    GPIO_Pin_t gpioAF = { GPIOA, GPIO_PIN_2 | GPIO_PIN_3 };
    GPIO_Init(&gpioAF , &configAF);
    RTT_printf("Expected uart2 GPIO2 : ");
    LOG_REG_COLOR(GPIOA->AFRL);
    LOG_REG_COLOR(GPIOA->MODER);
    NVIC_EnableIRQ(USART2_IRQn);
    //EXPEC: GPIOA->AFR[0] = 0x00007700;

    //Testcase for exti
    //TODO
    GPIO_Config configEXTI;
    configEXTI.mode = GPIO_MODE_IT_RISING;      
    //configEXTI.mode = PULL_DOWN;                   //HACK: mode EXTI phải cấu hình input cho nó
    GPIO_Pin_t gpioEXTI = { GPIOA, GPIO_PIN_0 | GPIO_PIN_6 };
    GPIO_Init(&gpioEXTI , &configEXTI);

    //BUG: Pending đã enable,nhưng k thể interrupt
    //@day : 12/11
    NVIC_EnableIRQ(EXTI0_IRQn);
    //LOG_REG_COLOR1(GPIOA);
    //LOG_REG_COLOR1(&gpioEXTI.port->PUPDR);
    //LOG_REG_COLOR1(gpioEXTI.port->MODER);
    //LOG_REG_COLOR1(gpioEXTI.port->PUPDR);
    //LOG_REG_COLOR1(gpioEXTI.port->AFRL);
    //LOG_REG_COLOR1(gpioEXTI.port->AFRH);

    
    //Expected :
        //nhảy vào hander tương ứng

    //Testcase for event external interrupt
    GPIO_Config* configEvent;
    configEvent->mode = GPIO_MODE_EVT_RISING;                           //HACK: mode EXTI phải cấu hình input cho nó
    //GPIO_Pin_t gpioEvent = { GPIOC, GPIO_PIN_2 | GPIO_PIN_3 };
    //GPIO_Init(&gpioEvent , configEvent);
}

void BusFault_Handler(void) {

}

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

__attribute((naked))
uint32_t __get_MSP() {
    __asm volatile("MOV R0, #0");
    __asm volatile ("MRS R0, MSP");
    __asm volatile ("BX LR");
}

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