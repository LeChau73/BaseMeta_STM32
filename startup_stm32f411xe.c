#include "stdio.h"
#include "stdint.h"
#include "core_m4.h"

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

#define GPIOD_BASE     0x40020C00UL
#define RCC_AHB1EN      (*(volatile unsigned int *)(RCC_BASE + 0x30))
#define GPIOD_MODER    (*(volatile unsigned int *)(GPIOD_BASE + 0x00))
#define GPIOD_OTYPER   (*(volatile unsigned int *)(GPIOD_BASE + 0x04))
#define GPIOD_OSPEEDR  (*(volatile unsigned int *)(GPIOD_BASE + 0x08))
#define GPIOD_PUPDR    (*(volatile unsigned int *)(GPIOD_BASE + 0x0C))
#define GPIOD_ODR      (*(volatile unsigned int *)(GPIOD_BASE + 0x14))

// Default handler for interrupts
void Default_Handler(void) {
    
    while(1);
}

/* Cortex-M4 Processor Exceptions */
__attribute__((weak, alias("Default_Handler"))) void NMI_Handler(void);
__attribute__((weak, alias("Default_Handler"))) void HardFault_Handler(void);
__attribute__((weak, alias("Default_Handler"))) void MemManage_Handler(void);
__attribute__((weak, alias("Default_Handler"))) void BusFault_Handler(void);
__attribute__((weak, alias("Default_Handler"))) void UsageFault_Handler(void);
__attribute__((weak, alias("Default_Handler"))) void SVC_Handler(void);
__attribute__((weak, alias("Default_Handler"))) void DebugMon_Handler(void);
__attribute__((weak, alias("Default_Handler"))) void PendSV_Handler(void);
__attribute__((weak, alias("Default_Handler"))) void SysTick_Handler(void);

/* STM32F411 Specific Interrupts */
__attribute__((weak, alias("Default_Handler"))) void WWDG_IRQHandler(void);
__attribute__((weak, alias("Default_Handler"))) void PVD_IRQHandler(void);
__attribute__((weak, alias("Default_Handler"))) void TAMP_STAMP_IRQHandler(void);
__attribute__((weak, alias("Default_Handler"))) void RTC_WKUP_IRQHandler(void);
__attribute__((weak, alias("Default_Handler"))) void FLASH_IRQHandler(void);
__attribute__((weak, alias("Default_Handler"))) void RCC_IRQHandler(void);
__attribute__((weak, alias("Default_Handler"))) void EXTI0_IRQHandler(void);
__attribute__((weak, alias("Default_Handler"))) void EXTI1_IRQHandler(void);
__attribute__((weak, alias("Default_Handler"))) void EXTI2_IRQHandler(void);
__attribute__((weak, alias("Default_Handler"))) void EXTI3_IRQHandler(void);
__attribute__((weak, alias("Default_Handler"))) void EXTI4_IRQHandler(void);
__attribute__((weak, alias("Default_Handler"))) void DMA1_Stream0_IRQHandler(void);
__attribute__((weak, alias("Default_Handler"))) void DMA1_Stream1_IRQHandler(void);
__attribute__((weak, alias("Default_Handler"))) void DMA1_Stream2_IRQHandler(void);
__attribute__((weak, alias("Default_Handler"))) void DMA1_Stream3_IRQHandler(void);
__attribute__((weak, alias("Default_Handler"))) void DMA1_Stream4_IRQHandler(void);
__attribute__((weak, alias("Default_Handler"))) void DMA1_Stream5_IRQHandler(void);
__attribute__((weak, alias("Default_Handler"))) void DMA1_Stream6_IRQHandler(void);
__attribute__((weak, alias("Default_Handler"))) void ADC_IRQHandler(void);
__attribute__((weak, alias("Default_Handler"))) void EXTI9_5_IRQHandler(void);
__attribute__((weak, alias("Default_Handler"))) void TIM1_BRK_TIM9_IRQHandler(void);
__attribute__((weak, alias("Default_Handler"))) void TIM1_UP_TIM10_IRQHandler(void);
__attribute__((weak, alias("Default_Handler"))) void TIM1_TRG_COM_TIM11_IRQHandler(void);
__attribute__((weak, alias("Default_Handler"))) void TIM1_CC_IRQHandler(void);
__attribute__((weak, alias("Default_Handler"))) void TIM2_IRQHandler(void);
__attribute__((weak, alias("Default_Handler"))) void TIM3_IRQHandler(void);
__attribute__((weak, alias("Default_Handler"))) void TIM4_IRQHandler(void);
__attribute__((weak, alias("Default_Handler"))) void I2C1_EV_IRQHandler(void);
__attribute__((weak, alias("Default_Handler"))) void I2C1_ER_IRQHandler(void);
__attribute__((weak, alias("Default_Handler"))) void I2C2_EV_IRQHandler(void);
__attribute__((weak, alias("Default_Handler"))) void I2C2_ER_IRQHandler(void);
__attribute__((weak, alias("Default_Handler"))) void SPI1_IRQHandler(void);
__attribute__((weak, alias("Default_Handler"))) void SPI2_IRQHandler(void);
__attribute__((weak, alias("Default_Handler"))) void USART1_IRQHandler(void);
__attribute__((weak, alias("Default_Handler"))) void USART2_IRQHandler(void);
__attribute__((weak, alias("Default_Handler"))) void EXTI15_10_IRQHandler(void);
__attribute__((weak, alias("Default_Handler"))) void RTC_Alarm_IRQHandler(void);
__attribute__((weak, alias("Default_Handler"))) void OTG_FS_WKUP_IRQHandler(void);
__attribute__((weak, alias("Default_Handler"))) void DMA1_Stream7_IRQHandler(void);
__attribute__((weak, alias("Default_Handler"))) void SDIO_IRQHandler(void);
__attribute__((weak, alias("Default_Handler"))) void TIM5_IRQHandler(void);
__attribute__((weak, alias("Default_Handler"))) void SPI3_IRQHandler(void);
__attribute__((weak, alias("Default_Handler"))) void DMA2_Stream0_IRQHandler(void);
__attribute__((weak, alias("Default_Handler"))) void DMA2_Stream1_IRQHandler(void);
__attribute__((weak, alias("Default_Handler"))) void DMA2_Stream2_IRQHandler(void);
__attribute__((weak, alias("Default_Handler"))) void DMA2_Stream3_IRQHandler(void);
__attribute__((weak, alias("Default_Handler"))) void DMA2_Stream4_IRQHandler(void);
__attribute__((weak, alias("Default_Handler"))) void OTG_FS_IRQHandler(void);
__attribute__((weak, alias("Default_Handler"))) void DMA2_Stream5_IRQHandler(void);
__attribute__((weak, alias("Default_Handler"))) void DMA2_Stream6_IRQHandler(void);
__attribute__((weak, alias("Default_Handler"))) void DMA2_Stream7_IRQHandler(void);
__attribute__((weak, alias("Default_Handler"))) void USART6_IRQHandler(void);
__attribute__((weak, alias("Default_Handler"))) void I2C3_EV_IRQHandler(void);
__attribute__((weak, alias("Default_Handler"))) void I2C3_ER_IRQHandler(void);
__attribute__((weak, alias("Default_Handler"))) void FPU_IRQHandler(void);

// Vector Table
#include "stm32f4xx.h"

extern unsigned long _estack;

/* Forward declaration of the default handlers */
void Reset_Handler(void);
void NMI_Handler(void);
void HardFault_Handler(void);
void MemManage_Handler(void);
void BusFault_Handler(void);
void UsageFault_Handler(void);
void SVC_Handler(void);
void DebugMon_Handler(void);
void PendSV_Handler(void);
void SysTick_Handler(void);

/* External Interrupts (IRQ Handlers) */
void WWDG_IRQHandler(void);
void PVD_IRQHandler(void);
void TAMP_STAMP_IRQHandler(void);
void RTC_WKUP_IRQHandler(void);
void FLASH_IRQHandler(void);
void RCC_IRQHandler(void);
void EXTI0_IRQHandler(void);
void EXTI1_IRQHandler(void);
void EXTI2_IRQHandler(void);
void EXTI3_IRQHandler(void);
void EXTI4_IRQHandler(void);
void DMA1_Stream0_IRQHandler(void);
void DMA1_Stream1_IRQHandler(void);
void DMA1_Stream2_IRQHandler(void);
void DMA1_Stream3_IRQHandler(void);
void DMA1_Stream4_IRQHandler(void);
void DMA1_Stream5_IRQHandler(void);
void DMA1_Stream6_IRQHandler(void);
void ADC_IRQHandler(void);
void EXTI9_5_IRQHandler(void);
void TIM1_BRK_TIM9_IRQHandler(void);
void TIM1_UP_TIM10_IRQHandler(void);
void TIM1_TRG_COM_TIM11_IRQHandler(void);
void TIM1_CC_IRQHandler(void);
void TIM2_IRQHandler(void);
void TIM3_IRQHandler(void);
void TIM4_IRQHandler(void);
void I2C1_EV_IRQHandler(void);
void I2C1_ER_IRQHandler(void);
void I2C2_EV_IRQHandler(void);
void I2C2_ER_IRQHandler(void);
void SPI1_IRQHandler(void);
void SPI2_IRQHandler(void);
void USART1_IRQHandler(void);
void USART2_IRQHandler(void);
void EXTI15_10_IRQHandler(void);
void RTC_Alarm_IRQHandler(void);
void OTG_FS_WKUP_IRQHandler(void);
void DMA1_Stream7_IRQHandler(void);
void SDIO_IRQHandler(void);
void TIM5_IRQHandler(void);
void SPI3_IRQHandler(void);
void DMA2_Stream0_IRQHandler(void);
void DMA2_Stream1_IRQHandler(void);
void DMA2_Stream2_IRQHandler(void);
void DMA2_Stream3_IRQHandler(void);
void DMA2_Stream4_IRQHandler(void);
void OTG_FS_IRQHandler(void);
void DMA2_Stream5_IRQHandler(void);
void DMA2_Stream6_IRQHandler(void);
void DMA2_Stream7_IRQHandler(void);
void USART6_IRQHandler(void);
void I2C3_EV_IRQHandler(void);
void I2C3_ER_IRQHandler(void);
void FPU_IRQHandler(void);

/* Vector Table */
__attribute__((section(".isr_vector")))
void (* const g_pfnVectors[])(void) = {
    (void (*)(void))(&_estack),      /* Initial Stack Pointer */
    Reset_Handler,                   /* Reset Handler */
    NMI_Handler,                     /* NMI Handler */
    HardFault_Handler,               /* Hard Fault Handler */
    MemManage_Handler,               /* MPU Fault Handler */
    BusFault_Handler,                /* Bus Fault Handler */
    UsageFault_Handler,              /* Usage Fault Handler */
    0, 0, 0, 0,                      /* Reserved */
    SVC_Handler,                     /* SVCall Handler */
    DebugMon_Handler,                /* Debug Monitor Handler */
    0,                               /* Reserved */
    PendSV_Handler,                  /* PendSV Handler */
    SysTick_Handler,                 /* SysTick Handler */

    /* External Interrupts */
    WWDG_IRQHandler,                 /* Window WatchDog */
    PVD_IRQHandler,                  /* PVD through EXTI Line detection */
    TAMP_STAMP_IRQHandler,           /* Tamper and TimeStamp */
    RTC_WKUP_IRQHandler,             /* RTC Wakeup */
    FLASH_IRQHandler,                /* FLASH */
    RCC_IRQHandler,                  /* RCC */
    EXTI0_IRQHandler,                /* EXTI Line0 */
    EXTI1_IRQHandler,                /* EXTI Line1 */
    EXTI2_IRQHandler,                /* EXTI Line2 */
    EXTI3_IRQHandler,                /* EXTI Line3 */
    EXTI4_IRQHandler,                /* EXTI Line4 */
    DMA1_Stream0_IRQHandler,         /* DMA1 Stream 0 */
    DMA1_Stream1_IRQHandler,         /* DMA1 Stream 1 */
    DMA1_Stream2_IRQHandler,         /* DMA1 Stream 2 */
    DMA1_Stream3_IRQHandler,         /* DMA1 Stream 3 */
    DMA1_Stream4_IRQHandler,         /* DMA1 Stream 4 */
    DMA1_Stream5_IRQHandler,         /* DMA1 Stream 5 */
    DMA1_Stream6_IRQHandler,         /* DMA1 Stream 6 */
    ADC_IRQHandler,                  /* ADC1 */
    0, 0, 0, 0, 0,                   /* Reserved for STM32F411 */
    EXTI9_5_IRQHandler,              /* External Line[9:5] */
    TIM1_BRK_TIM9_IRQHandler,        /* TIM1 Break / TIM9 */
    TIM1_UP_TIM10_IRQHandler,        /* TIM1 Update / TIM10 */
    TIM1_TRG_COM_TIM11_IRQHandler,   /* TIM1 Trigger/Commutation / TIM11 */
    TIM1_CC_IRQHandler,              /* TIM1 Capture Compare */
    TIM2_IRQHandler,                 /* TIM2 */
    TIM3_IRQHandler,                 /* TIM3 */
    TIM4_IRQHandler,                 /* TIM4 */
    I2C1_EV_IRQHandler,              /* I2C1 Event */
    I2C1_ER_IRQHandler,              /* I2C1 Error */
    I2C2_EV_IRQHandler,              /* I2C2 Event */
    I2C2_ER_IRQHandler,              /* I2C2 Error */
    SPI1_IRQHandler,                 /* SPI1 */
    SPI2_IRQHandler,                 /* SPI2 */
    USART1_IRQHandler,               /* USART1 */
    USART2_IRQHandler,               /* USART2 */
    0, 0,                            /* Reserved */
    EXTI15_10_IRQHandler,            /* External Line[15:10] */
    RTC_Alarm_IRQHandler,            /* RTC Alarm */
    OTG_FS_WKUP_IRQHandler,          /* USB OTG FS Wakeup */
    0, 0, 0, 0,                      /* Reserved */
    DMA1_Stream7_IRQHandler,         /* DMA1 Stream7 */
    0, 0,                            /* Reserved */
    SDIO_IRQHandler,                 /* SDIO */
    TIM5_IRQHandler,                 /* TIM5 */
    SPI3_IRQHandler,                 /* SPI3 */
    0, 0, 0, 0, 0,                   /* Reserved */
    DMA2_Stream0_IRQHandler,         /* DMA2 Stream 0 */
    DMA2_Stream1_IRQHandler,         /* DMA2 Stream 1 */
    DMA2_Stream2_IRQHandler,         /* DMA2 Stream 2 */
    DMA2_Stream3_IRQHandler,         /* DMA2 Stream 3 */
    DMA2_Stream4_IRQHandler,         /* DMA2 Stream 4 */
    0, 0, 0, 0, 0, 0,                /* Reserved */
    OTG_FS_IRQHandler,               /* USB OTG FS */
    DMA2_Stream5_IRQHandler,         /* DMA2 Stream5 */
    DMA2_Stream6_IRQHandler,         /* DMA2 Stream6 */
    DMA2_Stream7_IRQHandler,         /* DMA2 Stream7 */
    USART6_IRQHandler,               /* USART6 */
    I2C3_EV_IRQHandler,              /* I2C3 Event */
    I2C3_ER_IRQHandler,              /* I2C3 Error */
    FPU_IRQHandler,                  /* FPU */
};


void gpio_init(void)
{
    // Bật clock GPIOD (bit 3 trong RCC_AHB1ENR)
    RCC_AHB1EN |= (1 << 3);

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
    //__libc_init_array();

    gpio_init();

    // 3. Gọi main
    main();

    // 4. Vòng lặp vô hạn nếu main return
    while(1);
}

