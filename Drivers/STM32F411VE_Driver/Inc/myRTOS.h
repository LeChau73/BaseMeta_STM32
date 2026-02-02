#ifndef MY_RTOS_H
#define MY_RTOS_H

#include<stdio.h>
#include<stdint.h>
#include<string.h>

// designed task stack
extern uint32_t _estack;
#define END_STACK_KERNEL    &_estack - (2*1024U)

#define START_HEAD_CUSTOM     END_STACK_KERNEL - (4*1024U)

#define END_HEAP              END_STACK_KERNEL

#define START_TASK            START_HEAD_CUSTOM

typedef void (*task_function_t)(void* parameter);
typedef struct t_TCB t_TCB;


typedef enum {
    BLOCKED = -1,
    READY,
    RUNNING,
    SUSPENDED
} statusTask;

typedef enum {
    STATUS_ERROR = -1,
    STATUS_OK
} StatusCode;

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


StatusCode CreateTask(const char* nameTask, uint8_t priority, uint32_t leghtTask, task_function_t taskExcu, void *parameter);
StatusCode StartSchedule();
void switch_MSP_To_PSP();
void InitSysTick(uint8_t timeout);

void PendSV_Handler(void);
void SysTick_Handler(void);

void* mallocCustom(size_t size);




#endif