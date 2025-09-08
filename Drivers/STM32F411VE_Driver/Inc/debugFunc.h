#ifndef DEBUGFUNC_H
#define DEBUGFUNC_H


#include<stdio.h>
#include<stdint.h>

/* Timer để tính toán thời gian chạy của hàm dùng DWT */
void initTimer();
uint32_t timeStart();
uint32_t timeEnd();
void conculateTime(uint32_t start, uint32_t end);

#endif