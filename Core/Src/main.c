#include "main.h"

#define SET_BIT(reg, bit)    (reg |= (1 << bit))

// Configuage clock
#define RCC_CR              (*(volatile uint32_t*)0x40023800)
#define RCC_PLLCFGR         (*(volatile uint32_t*)0x40023804)
#define RCC_CFGR            (*(volatile uint32_t*)0x40023808)



//TODO: check xem đang dùng clock nào
//RCC_CFGR 0x08

static inline void printValueRegister( uint32_t name_register )
{
    for(int i = 0; i < name_register; i++);
}



static void printValueRegister1( uint32_t name_register )
{
    for(int i = 0; i < name_register; i++);
}




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

int main(void) 
{
    ITM_Init(true);
    

    uint32_t start = timeStart();
    for(int i = 0; i < 100; i++)
        printValueRegister(1000);

    uint32_t end = timeEnd();
    conculateTime(start, end);

    // Vòng lặp chính
    char buffer[32];
    volatile int counter = 1;
    
    // Dùng sprintf


    uint32_t startd = timeStart();
    for(int i = 0; i < 100; i++)
        printValueRegister1(1000);

    uint32_t endd = timeEnd();
    conculateTime(startd, endd);
    configGpio();


    DWT_DataMaching(2, &counter);
    while (1) {

        GPIO_Toogle( &(GPIO_Pin_t){GPIOD, GPIO_PIN_12 | GPIO_PIN_13 | GPIO_PIN_14 | GPIO_PIN_15} ); 
        myPrintf("%x", 434343);
        myPrintf(" Gia tri thanh ghi RCC_CFGR = %x", RCC_CFGR);
        myPrintf(" Gia tri thanh ghi RCC_CFGR = %x", RCC_PLLCFGR);


        for (volatile int i = 0; i < 1000000; i++); // Delay giả lập
        led_on(12);
        for (volatile int i = 0; i < 1000000; i++); // Delay giả lập
    }

    return 0;
}


void configGpio()
{

    //Enable clock
    volatile uint32_t* rcc_gpio = (volatile uint32_t*)RCC_AHB1ENR;
    *rcc_gpio |= RCC_GPIODEN;

    GPIO_Pin_t gpio = { GPIOD , GPIO_PIN_12 | GPIO_PIN_13 | GPIO_PIN_14 | GPIO_PIN_15};
    GPIO_Config config = { OUTPUT_PP, 0, MEDIUM_SPEED, 0};

    GPIO_Init(&gpio , &config);
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