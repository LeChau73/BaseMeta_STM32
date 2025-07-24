#include "stm32f4xx_itm.h"
#include <stdio.h>
#include <stdarg.h>
#include <stdint.h>

void ITM_Init(bool enable_timestamp)
{
    // DBGMCU_CR 
    // Enable for trace asynchronous mode
    *(uint32_t*)0xE0042004 |= (1 << 5);

    // Unlock write access to other ITM register
    *(uint32_t*)0xE0000FB0 = 0xC5ACCE55; 

    if ( enable_timestamp == true )
    {
        //setting for timestamp
        *(uint32_t*)0xE0000E80 |= (1 << 1);
    }

    // Control registers
    *(uint32_t*)0xE0000E80 |= ( (1 << 3) | (1 << 16) | (1 << 0) );

    // ITM_TER: Enable port 0
    *(volatile uint32_t *)0xE0000E00 = (1 << 0) | (1 << 1); // Enable port 0 và 1

}




void ITM_SendChar(uint32_t c) {
    // Wait until port is available for transmission
        // 0 = full
        // 1 = not full.
    while (!(ITM->PORT[0].u32 & (1UL << 0))) {
        // Wait until port is ready
    }

    // Send char
    ITM->PORT[0].u8 = (uint8_t)c;
}


int _write(int file, char *ptr, int len) {
    for (int i = 0; i < len; i++) {
        ITM_SendChar(ptr[i]);
    }
    return len;
}


void ITM_SendString(const char* str,uint8_t port)
{
    while( *str != '\0' )
    {
        ITM_SendChar(*str++);
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
void ITM_SendHalfWord(uint16_t data) {
    while (!(ITM->PORT[0].u32 & (1UL << 0))) {
        // Wait until port is ready
    }
    ITM->PORT[0].u16 = data;
}

// Send a 32-bit value via ITM
void ITM_SendWord(uint32_t data) {
    while (!(ITM->PORT[0].u32 & 1)) {
        // Wait until stimulus port is ready (not full)
    }
    ITM->PORT[0].u32 = data;
}