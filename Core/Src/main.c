#include "main.h"
#include <stdarg.h>

// Configuage clock
#define RCC_CR              (*(volatile uint32_t*)0x40023800)
#define RCC_PLLCFGR         (*(volatile uint32_t*)0x40023804)
#define RCC_CFGR            (*(volatile uint32_t*)0x40023808)

#define RTT_printf(...)  SEGGER_RTT_printf(0,__VA_ARGS__)
#define LOG_REG(name) SEGGER_RTT_printf(0, #name " = 0x%08X\n", (unsigned int)(name))  //for register
//%08X là một định dạng cho printf:
//%X : in số nguyên không dấu dưới dạng thập lục phân chữ IN HOA (A..F).
//8 : chiều rộng tối thiểu là 8 ký tự.
//0 : đệm bằng ký tự 0 (nếu độ dài thực tế nhỏ hơn 8).


//TODO: check xem đang dùng clock nào
//RCC_CFGR 0x08



#define ROM_M4_PID4   (*(volatile uint32_t*)0xE00FFFD0) // Peripheral ID4 in ROM table (Cortex-M4)

#define ROM_M4_CPM3   (*(volatile uint32_t*)0xE00FFFFC) // Peripheral ID4 in ROM table (Cortex-M4)

#define ROM_M4_SCS    (*(volatile uint32_t*)0xE00FF000) // Peripheral ID4 in ROM table (Cortex-M4)



void DWT_DataMaching(int value,volatile void* addrOfValue)
{
    *(volatile uint32_t *)0xE0000FB0 = 0xC5ACCE55; // DWT_LAR: Unlock DWT
    *(volatile uint32_t *)0xE0001000 |= (1 << 0); // DWT_CTRL: CYCCNTENA


    
    *(volatile uint32_t *)0xE0001014 = (uint32_t)addrOfValue; // DWT_COMP1: Địa chỉ sensor_data
    *(volatile uint32_t *)0xE0001018 = (value) | (0b10 << 0) | (1 << 2); // Data value match + EMITRANGE
    *(volatile uint32_t *)0xE0000E00 |= (1 << 1); // Enable ITM port 1 for DWT
    *(volatile uint32_t *)0xE0001024 |= (1 << 24); // Emit ITM event
}

void configGpio();

void valdicFunc(const char* fmt, ...) {
    va_list va;
    va_start(va, fmt); //fmt đánh dấu

    if ( *fmt == 'c')
    {
        
    }

    va_arg(va, int );

}
#define TEXT 7
const int val = 5;
int globaldsadsa = 6;
static int uninit;


void EXTI0_IRQHandler(void) {
    __asm volatile ("SVC 0x3");

    EXTI->PR &= ~(1 << 0);
}

uint32_t __get_MSP() {
    uint32_t msp = 0;
    __asm volatile("MOV R1, #0");
    __asm volatile ("MRS %0, MSP": "=r"(msp));
}

//TODO: với các cái này
void SVC_Handler(void) {
    uint32_t *sp = (uint32_t *)__get_MSP();  // Hoặc PSP nếu User mode
    uint32_t pc = sp[6];                     // PC khi gọi SVC
    uint8_t svc_number = *((uint8_t *)(pc - 1));  // Đọc byte cuối lệnh SVC (imm là byte 0)
    switch (svc_number & 0xFF) {             // Mask để lấy imm (0-255)
        case 3:  // Xử lý cho SVC #3, ví dụ: custom function
            // Gọi hàm tương ứng, ví dụ: my_custom_syscall();
            break;
        // Các case khác...
    }
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

    // In thêm SP hiện tại (MSP hoặc PSP tùy mode)
    RTT_printf("Stacked SP  = 0x%08X  (tức là địa chỉ pStack)\n", (uint32_t)pStack);

    // In các thanh ghi fault
    RTT_printf("HFSR = 0x%08X\n", SCB->HFSR);
    RTT_printf("CFSR = 0x%08X\n", SCB->CFSR);
    RTT_printf("BFAR = 0x%08X\n", SCB->BFAR);
    RTT_printf("MMFAR= 0x%08X\n", SCB->MMFAR);
    //TODO: implement lưu vào flash
    
    while(1) {};
}
void EXTI9_5_IRQHandler(void) {
    LOG_REG_COLOR1(EXTI->PR);

    EXTI->PR &= ~(1 << 5);
    EXTI->PR &= ~(1 << 6);

    LOG_REG_COLOR1(EXTI->PR);
}



int main(void) 
{

    //ITM_Init(false);
    //myPrintf("I am using ITM print for debug\n");
    //ITM_SendString("Hello");
    SEGGER_RTT_Init();
    RTT_LOG_BRIGHT_RED("=====Hello RTT!=====\n");

    SEGGER_RTT_WriteString(0, RTT_CTRL_CLEAR); // Clear screen

    //Load from memory
    //__asm volatile ("LDR R3, %0"
    //                :
    //                :"m"(counter) );
//
    //__asm volatile ("LDR R0, [R3]");
//
    //__asm volatile ("MRS R0, CONTROL");
    //uint32_t controlVal = 0;
    //__asm volatile ("MOV %0, R0" : "=r"(controlVal));
   
    configGpio();
    //BUG: Chi enable line 1
    TRIGGER_INTERRUPT_EVENT(EXTI_LINE_5 | EXTI_LINE_0);
    LOG_REG_COLOR1(EXTI->SWIER);
    LOG_REG_COLOR1(EXTI->PR);
    while (1) {
        GPIO_Toogle( &(GPIO_Pin_t){GPIOD, GPIO_PIN_12 | GPIO_PIN_13 | GPIO_PIN_14 | GPIO_PIN_15} );

        for (volatile int i = 0; i < 1000000; i++); // Delay giả lập
        //led_on(12);
        for (volatile int i = 0; i < 1000000; i++); // Delay giả lập
        //SEGGER_RTT_WriteString(0, "Hello RTTsdsad!\n");
    }

    return 0;
}


void configGpio()
{

    //Enable clock
    volatile uint32_t* rcc_gpio = (volatile uint32_t*)RCC_AHB1ENR;
    *rcc_gpio |= RCC_GPIODEN;

    __HAL_RCC_GPIOH_CLK_ENABLE();
    __HAL_RCC_GPIOD_CLK_ENABLE();
    __HAL_RCC_GPIOC_CLK_ENABLE();
    __HAL_RCC_GPIOB_CLK_ENABLE();

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
    //EXPEC: GPIOA->AFR[0] = 0x00007700;

    //Testcase for exti
    GPIO_Config configEXTI;
    configEXTI.mode = GPIO_MODE_IT_RISING;      
    //configEXTI.mode = PULL_DOWN;                   //HACK: mode EXTI phải cấu hình input cho nó
    GPIO_Pin_t gpioEXTI = { GPIOA, GPIO_PIN_0 | GPIO_PIN_6 };
    GPIO_Init(&gpioEXTI , &configEXTI);

    //BUG: Pending đã enable,nhưng k thể interrupt
    NVIC_EnableIRQ(EXTI9_5_IRQn | EXTI0_IRQn);
    LOG_REG_COLOR1(GPIOA);
    LOG_REG_COLOR1(&gpioEXTI.port->PUPDR);
    LOG_REG_COLOR1(gpioEXTI.port->MODER);
    LOG_REG_COLOR1(gpioEXTI.port->PUPDR);
    LOG_REG_COLOR1(gpioEXTI.port->AFRL);
    LOG_REG_COLOR1(gpioEXTI.port->AFRH);

    
    //Expected :
        //nhảy vào hander tương ứng

    //Testcase for event external interrupt
    GPIO_Config* configEvent;
    configEvent->mode = GPIO_MODE_EVT_RISING;                           //HACK: mode EXTI phải cấu hình input cho nó
    //GPIO_Pin_t gpioEvent = { GPIOC, GPIO_PIN_2 | GPIO_PIN_3 };
    //GPIO_Init(&gpioEvent , configEvent);
}



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