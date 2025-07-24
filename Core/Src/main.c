#include "main.h"

#define SET_BIT(reg, bit)    (reg |= (1 << bit))

// Configuage clock
#define RCC_CR              (*(volatile uint32_t*)0x40023800)
#define RCC_PLLCFGR         (*(volatile uint32_t*)0x40023804)
#define RCC_CFGR            (*(volatile uint32_t*)0x40023808)





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



int main(void) 
{

    // Khởi tạo ITM
    //Init_ITM(0, 32);
    gpio_init();

    ITM_Init(true);
    // Vòng lặp chính
    char buffer[32];
    volatile int counter = 1;
    printf("Hello, ITM!\n");

    int val = 10;
    // Dùng sprintf

    DWT_DataMaching(2, &counter);
    while (1) {

        //: đưa ra số hex gửi 2 byte một lúc
        ITM_SendHalfWord(5658);
    
        led_on(12);
        for (volatile int i = 0; i < 1000000; i++); // Delay giả lập
        led_off(12);
        for (volatile int i = 0; i < 1000000; i++); // Delay giả lập
    }

    return 0;
}




void gpio_init(void)
{
    // Bật clock GPIOD (bit 3 trong RCC_AHB1ENR)
    RCC_AHB1ENR |= (1 << 3);

    // MODER: set PD12 - PD15 là output (01)
    GPIOD_MODER &= ~(0xFF << 24);   // Clear 4 chân (2 bit mỗi chân)
    GPIOD_MODER |=  (0x55 << 24);   // Set 01 cho từng chân

    // OTYPER: push-pull
    GPIOD_OTYPER &= ~(0xF << 12);   // Clear bit 12-15

    // OSPEEDR: tốc độ medium (01)
    GPIOD_OSPEEDR &= ~(0xFF << 24);
    GPIOD_OSPEEDR |=  (0x55 << 24);

    // PUPDR: no pull (00)
    GPIOD_PUPDR &= ~(0xFF << 24);
}


void led_on(uint8_t pin) 
{
    GPIOD_BSRR = (1 << pin);         // Set pin (ON)
}

void led_off(uint8_t pin) 
{
    GPIOD_BSRR = (1 << (pin + 16));  // Reset pin (OFF)
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