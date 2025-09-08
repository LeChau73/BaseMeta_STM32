#ifndef __STM32F4xx_ITM_h
#define __STM32F4xx_ITM_h

#include "core_m4.h"

#ifdef __cplusplus
extern "C"
{
#endif

/* Reference
typedef struct tskTaskControlBlock
{
    volatile StackType_t *pxTopOfStack;        // Con trỏ đến đỉnh ngăn xếp
    ListItem_t xStateListItem;                 // Mục danh sách để quản lý trạng thái task
    ListItem_t xEventListItem;                 // Mục danh sách để quản lý sự kiện
    UBaseType_t uxPriority;                    // Mức độ ưu tiên
    StackType_t *pxStack;                      // Con trỏ đến vùng ngăn xếp
    char pcTaskName[configMAX_TASK_NAME_LEN];  // Tên task
    TickType_t xTicksToDelay;                  // Thời gian delay (nếu có)
    // Các trường khác như thông tin timeout, tài nguyên, v.v.
} TCB_t;
*/

typedef struct tskTaskControlBlock
{
    uint16_t ID;            //ID của task
    


};







#ifdef __cplusplus
extern "C"
}
#endif

#endif
