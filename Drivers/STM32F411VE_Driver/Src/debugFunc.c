#include "debugFunc.h"
#include "log.h"
#include "core_m4.h"

void initTimer() 
{
    *(volatile uint32_t *)0xE0000FB0 = 0xC5ACCE55; // DWT_LAR: Unlock DWT
    DWT->CTRL |= (1 << 0); // DWT_CTRL: CYCCNTENA: Bắt đầu đếm
}

uint32_t timeStart()
{
    volatile uint32_t start_cycle;
    myPrintf("---- Time start ----\n");
    initTimer();

    // reset bộ đếm
    DWT->CYCCNT = 0;
    return start_cycle = DWT->CYCCNT;
}

uint32_t timeEnd()
{
    volatile uint32_t end_cycle;
   
    myPrintf("---- End time ----\n");
    initTimer();
    DWT->CTRL |= ~(1 << 0); // ngừng đếm
    // reset bộ đếm
    end_cycle = DWT->CYCCNT;
    
    return end_cycle;
}

void conculateTime(uint32_t start, uint32_t end)
{
    uint32_t total_cycle;
    total_cycle = end - start;
    myPrintf("Total cycle = %d\n", total_cycle);
}