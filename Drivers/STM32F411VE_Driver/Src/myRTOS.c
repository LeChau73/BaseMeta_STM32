#include "myRTOS.h"

//

// Only life and access in this file
// Mỗi file .c include sẽ có copy riêng
// KHÔNG nên khai báo biến static ở file .h
static t_TCB* headTask = NULL;


StatusCode CreateTask(const char* nameTask, uint8_t priority, uint32_t leghtTask, task_function_t taskExcu, void *parameter)
{
    StatusCode result = ERROR;
    static void* EndStackCurrent = NULL;

    t_TCB* newTask = (t_TCB*)mallocCustom(sizeof(t_TCB));
    if( !newTask )
        return result;

    strcpy(newTask->task_name, nameTask);

    if ( EndStackCurrent == NULL ) {
        newTask->start_stack = START_TASK;

    } else {
        newTask->start_stack = EndStackCurrent;
    }
    // Update EndStack current
    EndStackCurrent = newTask->start_stack + leghtTask;
    
    newTask->priority = priority;
    newTask->task_entry = taskExcu;
    newTask->task_parameter = parameter;
    newTask->status = BLOCKED;
    newTask->p_psp = newTask->start_stack;
    newTask->timeout = 1000; //TODO : Tính toán lại khi implement systick
    newTask->next_tcb = NULL;

    // Add new task into list
    if(headTask == NULL) {
        headTask = newTask;
    } else {
        t_TCB* itr = headTask;
        while(itr != NULL)
            itr = itr->next_tcb;
        itr = newTask;
    }

    //Init for task
    uint32_t* temp_psp = newTask->p_psp;
    *temp_psp = (0x1 << 24); //xPSR
    --temp_psp;
    *temp_psp = newTask->task_entry; //PC //TODO: Check lại
    --temp_psp;
    *temp_psp = 0; //LR             //TODO: Check lại
    --temp_psp;
    *temp_psp = 0; //R12
    --temp_psp;
    *temp_psp = 0; //R2
    --temp_psp;
    *temp_psp = 0; //R1
    --temp_psp;
    *temp_psp = newTask->task_parameter; //R0
    newTask->p_psp = temp_psp;

}

void* mallocCustom(size_t size) {
    // Lưu lại end của heap là được
    // địa chỉ cuối head hiện tại
    static void* endHeap;
    if( endHeap == NULL )
        endHeap = START_HEAD_CUSTOM;
        
    void *beforeHeap = endHeap;

    if( endHeap + size > END_HEAP )
        return NULL;

    endHeap += size;

    return beforeHeap;
}