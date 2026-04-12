#include "Ring_Buffer.h"




void ringBufS_Init(ringBufS *ring) {
    if(ring == NULL)
        return;

    memset(ring, 0, sizeof(ringBufS));
}


State_Ring ringBufS_State(ringBufS *ring) {
    if(ring == NULL)
        return -1;

    State_Ring state_current;

    if(ring->head == ring->tail) {
        state_current = EMPTY;
    } else if ( ((ring->head + 1) % MAX_SIZE ) == ring->tail ) {
        state_current = FULL;
    } else {
        state_current = AVALABLE;
    }

    return state_current;
}


char ringBuf_get(ringBufS *ring) {
    if(ring == NULL)
        return -1;

    char data;

    if(ringBufS_State(ring) != EMPTY) {
        return data = ring->buffer[ring->tail++];
    }

    return -1;
}


STATUS_RING ringBuf_push(ringBufS *ring, char buf[], uint16_t length) {
    if(ring == NULL)
        return 0;

    uint16_t number_cap = MAX_SIZE - ring->head + ring->tail;

    if (number_cap >= length)
    {
        memcpy(ring->buffer + ring->head + 1, buf, length);
        ring->head += length;    
        return OK_R;
    } 
    
    // Không đủ chỗ
    return ERROR_R;
}


STATUS_RING ringBuf_put(ringBufS *ring,char data) {
    if(ring == NULL)
        return 0;

    if(ringBufS_State(ring) != FULL) {
        ring->buffer[ring->head++] = data;
        return OK_R;
    }

    return ERROR_R;
}