#include "main.h"
#include <stdarg.h>

#define SET_BIT(reg, bit)    (reg |= (1 << bit))

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

int main(void) 
{
    ITM_Init(false);
    SEGGER_RTT_Init();
    RTT_printf("=====Hello RTT!=====\n");

    // Vòng lặp chính
    char buffer[32];
    volatile int counter = 1;

    configGpio();

    
    while (1) {
        counter++;
        
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

    GPIO_Pin_t gpio = { GPIOD , GPIO_PIN_12 | GPIO_PIN_13 | GPIO_PIN_14 | GPIO_PIN_15};
    gpio.port->gpiox_OSPEEDR = (uint32_t)0x0000C000; //Case 1: không ảnh hưởng cũ


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
    configEXTI.mode = EXTI_IT | TRIGGER_RISING;
    GPIO_Pin_t gpioEXTI = { GPIOD, GPIO_PIN_2 | GPIO_PIN_11 };
    GPIO_Init(&gpioEXTI , &configEXTI);


    //Expected :
        //nhảy vào hander tương ứng
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