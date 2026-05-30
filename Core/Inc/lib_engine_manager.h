#ifndef _ENGINE_MANAGER_H
#define _ENGINE_MANAGER_H

#include <stdio.h>
#include "Ring_Buffer.h"
//Struct of ringBuffer


#define THRESH_HOLD_STREAM 24

typedef enum {
    ERROR_ENGINER,
    IDLE,
    BUSY,
    OK
}ENGINE_STATE;

typedef struct
{
    ENGINE_STATE state_machine;
    DMA_HandleTypeDef s_DMA;
    ringBufS    s_Ring_Buffer;
    int retry_count;
}Engine_Manager;

static State_Ring Check_Free_Space();
void Init_subSystem(Engine_Manager* em);
void push(char* data, int lenghth);

#endif