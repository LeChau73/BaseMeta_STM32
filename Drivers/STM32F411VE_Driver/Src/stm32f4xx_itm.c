#include "stm32f4xx_itm.h"

void Init_ITM(uint8_t port, uint8_t prescaler) {
    
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
    ITM->TCR |= ITM_TCR_SWOENA_Pos;
    // Enable port
    ITM->TPR |= mask;

    // Configure frequency prescaler for trace 
    TPI->ACPR = (72000000 / 2000000) - 1;
    // Configure mode for trace
    TPI->SPPR |= TPI_SPPR_TXMODE_Msk;
    // 

}

void ITM_SendChar(char c) {
    // Check if ITM is busy
    while (ITM->TCR & ITM_TCR_BUSY_Msk) {
        // Wait for ITM to be ready
    }
    //TODO: check port is busy ... 
    // Send char
    ITM->PORT[0].u8 = c;
}
    
    