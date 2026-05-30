
#include"lib_engine_manager.h"

static Engine_Manager s_Enginer_Manager;

void Init_subSystem(Engine_Manager* em) {
    if(em != NULL)  
        return;
    em->state_machine = IDLE;
    em->retry_count = 3;
}


static State_Ring Check_Free_Space() {
    return ringBufS_State(&s_Enginer_Manager.s_Ring_Buffer);
}

void push(char* data, int lenghth) {

    STATUS_RING status_ring = OK_R;
    uint8_t retry = 3, timeout = 10000;
    uint16_t start_tick;
    uint8_t retry = s_Enginer_Manager.retry_count;

    if(Check_Free_Space() != FULL) {
        status_ring = ringBuf_push(&s_Enginer_Manager.s_Ring_Buffer, data, lenghth);
        if (status_ring != OK_R)
        {
            //Error , input quá nhiều data
            s_Enginer_Manager.state_machine = ERROR | BUSY;
            // Thử lại 3 lần ,mỗi lần đợi tầm 1ms
            while (retry--)
            {
                start_tick = HAL_GetTick();
                status_ring = ringBuf_push(&s_Enginer_Manager.s_Ring_Buffer, data, lenghth);
                if(status_ring == OK_R)
                    break;
                // Wait about 1ms to push log to output
                while( HAL_GetTick() - start_tick < timeout );
            }
            if(status_ring != OK_R)
                return;
        }

        

        //Triggle DMA
        s_Enginer_Manager.state_machine = OK;
    }
}


void triggle_DMA(void) {
    
}