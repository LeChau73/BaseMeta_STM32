#include "stm32f4xx_itm.h"
#include <stdio.h>
#include <stdarg.h>

void Init_ITM(uint8_t port, uint8_t prescaler) {
    // This function initializes the Instrumentation Trace Macrocell (ITM) for tracing.
    // It enables the ITM and SWO (Single Wire Output) trace modes.
    // The function configures the specified port and prescaler for trace operations.
    
    // Check if port is valid (0-31)
    if (port > 31) {
        // Invalid port number, return without initializing
        return;
    }
    // Check if prescaler is valid (0-7)
    if (prescaler > 7) {
        // Invalid prescaler value, return without initializing
        return;
    }

    // Translate port to mask
    uint32_t mask = 1 << port;
    
    // Enable TRCEN
    CoreDebug->DEMCR |= CoreDebug_DEMCR_TRCENA_Msk;
    
    //Unlock ITM
    ITM->LAR = 0xC5ACCE55;
    // Enable ITM trace : Can write to ITM
    ITM->TCR |= ITM_TCR_ITMENA_Msk;
    // Enable SWO trace
    ITM->TCR |= ITM_TCR_SWOENA_Msk;
    // Enable port
    ITM->TPR |= mask;

    // Configure frequency prescaler for trace (assuming 16MHz default clock)
    TPI->ACPR = (16000000 / 4000000) - 1;  // For 4MHz trace clock
    // Configure mode for trace
    TPI->SPPR |= TPI_SPPR_TXMODE_Msk;

}

void ITM_SendChar(char c, uint8_t port) {
    // Check if ITM is enabled
    if (!(ITM->TCR & ITM_TCR_ITMENA_Msk)) {
        return;
    }

    // Check if port is enabled
    if (!(ITM->TER & (1UL << port))) {
        return;
    }

    // Check if ITM is busy
    while (ITM->TCR & ITM_TCR_BUSY_Msk) {
        // Wait for ITM to be ready
    }

    // Wait until port is available for transmission
        // 0 = full
        // 1 = not full.
    while (!(ITM->PORT[port].u32 & (1UL << 0))) {
        // Wait until port is ready
    }

    // Send char
    ITM->PORT[port].u8 = c;
}

void ITM_SendString(const char* str,uint8_t port)
{
    while( *str != '\0' )
    {
        ITM_SendChar(*str++, port);
    }
}

// Send a 32-bit integer as ASCII string via ITM
void ITM_SendInt(int32_t num, uint8_t port) {
    char buf[12]; // Enough for -2147483648\0
    snprintf(buf, sizeof(buf), "%ld", (long)num);
    ITM_SendString(buf, port);
}

// Send a buffer of bytes via ITM
void ITM_SendBuffer(const uint8_t* buffer, uint32_t size, uint8_t port) {
    for (uint32_t i = 0; i < size; i++) {
        ITM_SendChar(buffer[i], port);
    }
}

// Enable a specific ITM port
void ITM_EnablePort(uint8_t port) {
    if (port <= 31) {
        ITM->TER |= (1UL << port);
    }
}

// Disable a specific ITM port
void ITM_DisablePort(uint8_t port) {
    if (port <= 31) {
        ITM->TER &= ~(1UL << port);
    }
}

// Check if a specific ITM port is enabled
uint8_t ITM_IsPortEnabled(uint8_t port) {
    if (port > 31) return 0;
    return (ITM->TER & (1UL << port)) ? 1 : 0;
}

// Send a 16-bit value via ITM
void ITM_SendHalfWord(uint16_t data, uint8_t port) {
    ITM->PORT[port].u16 = data;
}

// Send a 32-bit value via ITM
void ITM_SendWord(uint32_t data, uint8_t port) {
    ITM->PORT[port].u32 = data;
}

// Send formatted string via ITM
void ITM_Printf(uint8_t port, const char* format, ...) {
    char buffer[128]; // Adjust buffer size as needed
    va_list args;
    va_start(args, format);
    vsnprintf(buffer, sizeof(buffer), format, args);
    va_end(args);
    ITM_SendString(buffer, port);
}