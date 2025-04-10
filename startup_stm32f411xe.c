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
uint32_t *isr_vectors[] = {
    (uint32_t)START_STACK,           /* Stack pointer */
    (uint32_t)Reset_Handler,         /* Reset handler */
    (uint32_t)NMI_Handler,           /* NMI handler */
    (uint32_t)HardFault_Handler,     /* Hard fault handler */
    /* Add more ISR vectors as needed */
};

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

    // 3. Gọi main
    main();

    // 4. Vòng lặp vô hạn nếu main return
    while(1);
}

