#ifndef MY_RTOS_H
#define MY_RTOS_H

#include<stdio.h>
#include<stdint.h>

// designed task stack
extern uint32_t _estack;
#define END_STACK_KERNEL    &_estack + (2*1024U)

#define END_HEAD_CUSTOM     END_STACK_KERNEL + (2*1024U)
#define START_TASK          END_HEAD_CUSTOM

typedef void (*task_function_t)(void* parameter);
typedef struct t_TCB t_TCB;
typedef enum {
    BLOCKED = -1,
    READY,
    RUNNING,
    SUSPENDED
} statusTask;

typedef struct t_TCB
{
    uint32_t* p_psp;
    char task_name[10];         //name for task 10byte
    uint8_t priority;           //1byte
    statusTask status;             //1byte STATUS TASK
    // 12BYTE

    uint32_t timeout;
    uint32_t* start_stack;
    uint32_t  leght_stack;
    task_function_t task_entry;
    t_TCB* next_tcb;
    void *task_parameter;
}t_TCB;     //  40byte


#endif