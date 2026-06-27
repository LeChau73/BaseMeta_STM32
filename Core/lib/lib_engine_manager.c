
#include"lib_engine_manager.h"

static Engine_Manager s_Enginer_Manager;
static fn_send_to_hardware fn_send = NULL;

void Init_subSystem(Engine_Manager* em) {
    if(em != NULL)  
        return;
    em->state_machine = IDLE;
    em->retry_count = 3;
    
}


void register_interface(fn_send_to_hardware fn) {
    if(fn_send != NULL)
        return;
    fn_send = fn;
}


State_Ring Check_Free_Space() {
    return ringBufS_State(&s_Enginer_Manager.s_Ring_Buffer);
}

void push(char* data, int lenghth) {

    STATUS_RING status_ring = OK_R;
    uint8_t timeout = 10000;
    uint16_t start_tick;
    uint8_t retry = s_Enginer_Manager.retry_count;
    uint16_t number_data_ring;

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

        number_data_ring = caculate_number_data_avalable(&s_Enginer_Manager.s_Ring_Buffer);
        if(number_data_ring > THRESH_HOLD_STREAM) {
            //TODO: pop multil ở ring ,che giấu implement ở manager
            uint16_t tail = s_Enginer_Manager.s_Ring_Buffer.tail;
            char* buf = s_Enginer_Manager.s_Ring_Buffer.buffer;
            buf += tail;
            fn_send(buf, number_data_ring);
            s_Enginer_Manager.s_Ring_Buffer.tail += number_data_ring;
        }

        //Triggle DMA
        s_Enginer_Manager.state_machine = OK;
    }
}