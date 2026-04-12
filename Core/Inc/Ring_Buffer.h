#ifndef _RING_BUFFER_H__
#define _RING_BUFFER_H__
#include "lib_engine_manager.h"
#include "log.h"


// Maximum of ring buffer
#define MAX_SIZE 1024

// Define structure of ring
struct ty_ringBufS{
    uint16_t head;
    uint16_t tail;
    char buffer[MAX_SIZE];
};

//Struct of ringBuffer
typedef struct ty_ringBufS ringBufS;

// State of ring buffer
typedef enum {
    EMPTY,
    AVALABLE,
    FULL
} State_Ring;

typedef enum {
    ERROR_R = -1,
    OK_R
} STATUS_RING;

void ringBufS_Init(ringBufS *ring);

State_Ring ringBufS_State(ringBufS *ring);

//Get a byte from the queue
char ringBuf_get(ringBufS *ring);

//Put a byte from the queue
STATUS_RING ringBuf_put(ringBufS *ring, char data);

//Put a mul byte from the queue
STATUS_RING ringBuf_push(ringBufS *ring, char buf[], uint16_t length);

STATUS_RING ringBuf_flush(ringBufS *ring);

#endif