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



void ITM_init(void) {
    // Kích hoạt trace I/O trong DBGMCU_CR (địa chỉ 0xE0042004)
    *(volatile uint32_t *)0xE0042004 |= (1 << 5); // Bit 5: TRACE_IOEN
    *(volatile uint32_t *)0xE0042004 |= (0 << 6); // Bit 6-7: TRACE_MODE, chọn asynchronous trace (00)

    // Cấu hình ITM
    *(volatile uint32_t *)0xE0000FB0 = 0xC5ACCE55; // ITM_LAR: Mở khóa ITM
    *(volatile uint32_t *)0xE0000E80 = (1 << 0) |  // ITM_TCR: Bit 0 - ITMENA, kích hoạt ITM
                               (1 << 3) |  // Bit 3 - SWOENA, kích hoạt SWO
                               (1 << 16);  // Bit 16-22 - TRACEBUSID, đặt ID cho trace
    *(volatile uint32_t *)0xE0000E00 = 0x1; // ITM_TER: Kích hoạt port 0
}


void ITM_initt(void) {
    // Enable trace I/O and set asynchronous mode
    *(volatile uint32_t *)0xE0042004 |= (1 << 5); // DBGMCU_CR: TRACE_IOEN
    *(volatile uint32_t *)0xE0042004 |= (0 << 6); // DBGMCU_CR: TRACE_MODE (asynchronous)
    // Configure ITM
    *(volatile uint32_t *)0xE0000FB0 = 0xC5ACCE55; // ITM_LAR: Unlock ITM
    *(volatile uint32_t *)0xE0000E80 = (1 << 0) | (1 << 3) | (1 << 16); // ITM_TCR: ITMENA, SWOENA, TRACEBUSID
    *(volatile uint32_t *)0xE0000E00 = 0x1; // ITM_TER: Enable port 0
}

void ITM_SenddChar(uint8_t ch) {
    while (!(ITM->PORT[0].u32)); // Wait until port is ready
    ITM->PORT[0].u8 = ch; // Send character
}

int _write(int file, char *ptr, int len) {
    for (int i = 0; i < len; i++) {
        ITM_SenddChar(ptr[i]);
    }
    return len;
}


#define ROM_M4_PID4   (*(volatile uint32_t*)0xE00FFFD0) // Peripheral ID4 in ROM table (Cortex-M4)

#define ROM_M4_CPM3   (*(volatile uint32_t*)0xE00FFFFC) // Peripheral ID4 in ROM table (Cortex-M4)

#define ROM_M4_SCS    (*(volatile uint32_t*)0xE00FF000) // Peripheral ID4 in ROM table (Cortex-M4)


// Hàm chuyển đổi int sang chuỗi (cơ số 10)
char *int_to_string(int num, char *buffer) {
    if (num == 0) {
        buffer[0] = '0';
        buffer[1] = '\0';
        return buffer;
    }

    int is_negative = 0;
    if (num < 0) {
        is_negative = 1;
        num = -num;
    }

    // Tính số chữ số
    int temp = num;
    int len = 0;
    while (temp > 0) {
        temp /= 10;
        len++;
    }

    // Thêm ký tự kết thúc chuỗi
    buffer[len + is_negative] = '\0';

    // Chuyển đổi từng chữ số
    int i = len + is_negative - 1;
    while (num > 0) {
        buffer[i--] = (num % 10) + '0';
        num /= 10;
    }

    // Thêm dấu trừ nếu là số âm
    if (is_negative) {
        buffer[0] = '-';
    }

    return buffer;
}




int main(void) 
{

    // Khởi tạo ITM
    //Init_ITM(0, 32);
    gpio_init();

    ITM_initt();
    // Vòng lặp chính
    char buffer[32];
    uint32_t counter = 0;
    printf("Hello, ITM!\n");
    uint32_t val = (uint32_t)ROM_M4_SCS;
    while( !(ITM->PORT[0].u32) );
    ITM->PORT[0].u32 = val;


    // Dùng sprintf
    int_to_string(val, buffer);

    _write(1, buffer, 32);
    printf((const char*) buffer); // Kết quả: Chuỗi: 12345


    printf("Val = %d\n", val);
    // Back up
    //ITM_SenddChar(buffer);


    while (1) {

       

        printf("Hello, ITM!\n");

        

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