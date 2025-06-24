#include "main.h"

#define SET_BIT(reg, bit)    (reg |= (1 << bit))


// Định nghĩa thanh ghi ITM
#define ITM_STIM0           (*(volatile uint32_t*)0xE0000000) // ITM Stimulus Port 0
#define ITM_TER             (*(volatile uint32_t*)0xE0000E00)   // ITM Trace Enable Register
#define ITM_TCR             (*(volatile uint32_t*)0xE0000E80)   // ITM Trace Control Register
#define DEMCR               (*(volatile uint32_t*)0xE000EDFC)     // Debug Exception and Monitor Control Register

// Configuage clock
#define RCC_CR              (*(volatile uint32_t*)0x40023800)
#define RCC_PLLCFGR         (*(volatile uint32_t*)0x40023804)
#define RCC_CFGR            (*(volatile uint32_t*)0x40023808)



void log_printf(const char *fmt, ...)
{

    fprintf("dsdsd", )

    FILE *fp = fopen("log.txt", "a+");
    va_list args;
    va_start(args, fmt);
    if(fp)
    {
        vfprintf(fp, fmt, args);
    }
    vprintf(fmt, args);
    va_end(args);

    fclose(fp);

}

int main(void) 
{

    initialise_monitor_handles();  // Bắt buộc để bật semihosting printf

    log_printf("Hello from STM32 semihosting!\n");

    // Khởi tạo ITM
    //ITM_Init();
    gpio_init();
    // Vòng lặp chính
    uint32_t counter = 0;
    while (1) {
        //ITM_SendChar('d');
        led_on(12);
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