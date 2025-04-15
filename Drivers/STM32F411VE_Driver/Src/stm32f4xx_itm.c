#include "stm32f4xx_itm.h"

void Init_ITM(uint8_t port, uint8_t prescaler) {
    // This function initializes the Instrumentation Trace Macrocell (ITM) for tracing.
    // It enables the ITM and SWO (Single Wire Output) trace modes.
    // The function configures the specified port and prescaler for trace operations.
    
    // Check if port is valid
    if (port > 31 || port < 0) {
        // Can developer do something else?
        // such as interrupt?
        //TODO: Error handling
        return;
    }
    // Check if prescaler is valid
    if (prescaler > 7) {
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
    // Enable SWO trace :
    //FIXME: Fix Issue
    ITM->TCR |= ITM_TCR_SWOENA_Msk;  // Corrected line
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
