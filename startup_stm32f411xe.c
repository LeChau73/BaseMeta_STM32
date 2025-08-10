#include "stdio.h"
#include "stdint.h"

#define C1

#ifdef C1
#define START_RAM 0x20000000
#define LENGTH_RAM 128*1024
#define END_RAM START_RAM + LENGTH_RAM
#define START_STACK END_RAM
#else
extern uint32_t _estack; /* Stack pointer from linker script*/
#endif

// Khai báo các biến từ linker script
extern uint32_t _sidata;    // Địa chỉ .data trong Flash (nguồn)
extern uint32_t _sdata;     // Địa chỉ bắt đầu .data trong RAM (đích)
extern uint32_t _edata;     // Địa chỉ kết thúc .data trong RAM
extern uint32_t _sbss;      // Địa chỉ bắt đầu .bss
extern uint32_t _ebss;      // Địa chỉ kết thúc .bss

// Khai báo hàm main
extern int main(void);
extern void __libc_init_array(void);

#define RCC_BASE       0x40023800UL
#define GPIOD_BASE     0x40020C00UL
#define RCC_AHB1ENR    (*(volatile unsigned int *)(RCC_BASE + 0x30))
#define GPIOD_MODER    (*(volatile unsigned int *)(GPIOD_BASE + 0x00))
#define GPIOD_OTYPER   (*(volatile unsigned int *)(GPIOD_BASE + 0x04))
#define GPIOD_OSPEEDR  (*(volatile unsigned int *)(GPIOD_BASE + 0x08))
#define GPIOD_PUPDR    (*(volatile unsigned int *)(GPIOD_BASE + 0x0C))
#define GPIOD_ODR      (*(volatile unsigned int *)(GPIOD_BASE + 0x14))

// Default handler for interrupts
void Default_Handler(void) {
    while(1);
}

void Reset_Handler(void);

// Weak handler implementations
__attribute__((weak, alias("Default_Handler"))) void NMI_Handler(void);
__attribute__((weak, alias("Default_Handler"))) void HardFault_Handler(void);

// Vector Table
__attribute__((section(".isr_vector")))
void (* const g_pfnVectors[])(void) = {
    (void (*)(void))(START_STACK),  /* Stack pointer */
    Reset_Handler,               /* Reset */
    NMI_Handler,                 /* Non-Maskable Interrupt */
    HardFault_Handler,           /* Hard Fault */
};


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



// Reset Handler Implementation
void Reset_Handler(void)
{
    // 1. Copy .data từ Flash sang RAM
    uint32_t *pSrc = &_sidata;
    uint32_t *pDest = &_sdata;
    
    while (pDest < &_edata)
    {
        *pDest++ = *pSrc++;
    }

    // 2. Khởi tạo vùng .bss về 0
    pDest = &_sbss;
    while (pDest < &_ebss)
    {
        *pDest++ = 0;
    }

    //Khởi tạo các thành phần cần thiết của thư viện C (ví dụ: newlib).
    //Đảm bảo môi trường runtime C/C++ sẵn sàng trước khi vào main().
    __libc_init_array();

    gpio_init();

    // 3. Gọi main
    main();

    // 4. Vòng lặp vô hạn nếu main return
    while(1);
}

