#include "Ring_Buffer.h"


#define DIS_ISR(mask)                                                                                         \
    do {                                                                                                  \
        /* Disable interrupt to read ring status safely */                                                \
        __asm volatile (                                                                                  \
            "mrs %0, PRIMASK \n\t"  /* 1. Read current interrupt mask state */                           \
            "cpsid i         \n\t"  /* 2. Disable interrupts immediately */                              \
            "dsb             \n\t"  /* 3. Ensure interrupt disable takes effect */                       \
            "isb             \n\t"  /* 4. Flush pipeline */                                              \
            : "=r" (mask)                                                                                    \
            :                                                                                             \
            : "memory"                                                                                    \
        );                                                                                                \
    } while (0)
    
#define RES_ISR(mask)                           \    
    do {                                    \
        /* Enable lại ngắt  */              \
        __asm volatile (                    \
            "msr PRIMASK, %0 \n\t"          \
            "dsb             \n\t"          \
            "isb"                           \
            : "=r" (mask)                    \
            :                               \
            : "memory"                      \
        );                                  \
    }while(0)


//Status : Done - Thread-safe for single-core + ISR
void ringBufS_Init(ringBufS *ring) {
    if(ring == NULL)
        return;

    memset(ring, 0, sizeof(ringBufS));
}


// Internal function - assumes already in critical section
static State_Ring ringBufS_State_NoLock(ringBufS *ring) {
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


// Thread-safe version with critical section
State_Ring ringBufS_State(ringBufS *ring) {
    if(ring == NULL)
        return -1;

    int a;

    DIS_ISR(a);
    State_Ring state = ringBufS_State_NoLock(ring);
    RES_ISR(a);
    
    return state;
}


// Get a byte from the buffer - THREAD SAFE
char ringBuf_get(ringBufS *ring) {
    if(ring == NULL)
        return -1;

    char data = -1;
    int a;
    DIS_ISR(a);
    if(ringBufS_State_NoLock(ring) != EMPTY) {
        data = ring->buffer[ring->tail];
        ring->tail = (ring->tail + 1) % MAX_SIZE;
    }
    RES_ISR(a);
    
    return data;
}


// Put multiple bytes - THREAD SAFE with proper wrapping
STATUS_RING ringBuf_push(ringBufS *ring, char buf[], uint16_t length) {
    if(ring == NULL || buf == NULL || length == 0)
        return ERROR_R;

    int a;
    DIS_ISR(a);
    // Calculate available space (considering circular buffer)
    uint16_t available = (ring->tail - ring->head - 1 + MAX_SIZE) % MAX_SIZE;
    RES_ISR(a);
    
    if(available >= length) {
        // Copy data with proper circular wrapping
        for(uint16_t i = 0; i < length; i++) {
            ring->head = (ring->head + 1) % MAX_SIZE;
            ring->buffer[ring->head] = buf[i];
        }
        return OK_R;
    }
    

    return ERROR_R;
}


// Put a single byte - THREAD SAFE
STATUS_RING ringBuf_put(ringBufS *ring, char data) {
    if(ring == NULL)
        return ERROR_R;

    int a;
    DIS_ISR(a);
    if(ringBufS_State(ring) != FULL) {
        ring->head = (ring->head + 1) % MAX_SIZE;
        ring->buffer[ring->head] = data;

        return OK_R;
    }
    RES_ISR(a);

    return ERROR_R;
}


inline uint16_t caculate_number_data_avalable(const ringBufS *ring) {
    uint16_t number_byte_contain = 0;

    int a;
    DIS_ISR(a);
    if(ringBufS_State(ring) != FULL) {
        if(ring->head >= ring->tail) {
            number_byte_contain = ring->head - ring->tail;
        } else {
            number_byte_contain = MAX_SIZE - ring->tail + ring->head;
        }
    }
    RES_ISR(a);
    return number_byte_contain;
}