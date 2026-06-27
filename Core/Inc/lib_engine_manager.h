#ifndef _ENGINE_MANAGER_H
#define _ENGINE_MANAGER_H

#include <stdio.h>
#include "Ring_Buffer.h"
//Struct of ringBuffer


#define THRESH_HOLD_STREAM 24



extern uint32_t HAL_GetTick(void);



// interface to connect with hardware
//typedef uint8_t (*fn_send_to_hardware)(char* data, int lenghth);
//Struct of ringBuffer
//typedef struct ty_ringBufS ringBufS;

// interface to connect with hardware
typedef uint8_t (*fn_send_to_hardware)(char* data, int lenghth);
//Struct of ringBuffer
typedef struct ty_ringBufS ringBufS;
//fn_send_to_hardware equaivale uint8_t (*)(char*, int)

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
    UART_HandleTypeDef *huart;
}Engine_Manager;

State_Ring Check_Free_Space();
void Init_subSystem(Engine_Manager* em);
void push(char* data, int lenghth);
void register_interface(fn_send_to_hardware fn);




inline static void Signal_DMA(uint16_t number_data);

#endif