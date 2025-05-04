#include "main.h"




int main() {

    Init_ITM(0, 1);

    ITM_SendChar('A', 0);
    ITM_SendChar('B', 0);
    ITM_SendChar('C', 0);

    while (1) {
        ITM_SendString("Hello World", 0);
        for(int i = 0; i < 1000000; i++); // Delay (1s)
    }

    return 0;
}