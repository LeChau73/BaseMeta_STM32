#ifndef _RING_BUFFER_H__
#define _RING_BUFFER_H__
#include "log.h"


// Maximum of ring buffer
#define MAX_SIZE 1024

#define ATOMIC_INCREASE(DATA)                                 \
  do {                                                        \
    uint32_t val;                                             \
    do {                                                      \
      val = __LDREXW((__IO uint32_t *)&(DATA));               \
      val++;                                                  \
    } while ((__STREXW(val,(__IO uint32_t *)&(DATA))) != 0U); \
  } while(0)


#define ATOMIC_DECREASE(DATA)                                 \
  do {                                                        \
    uint32_t val;                                             \
    do {                                                      \
      val = __LDREXW((__IO uint32_t *)&(DATA));               \
      val--;                                                  \
    } while ((__STREXW(val,(__IO uint32_t *)&(DATA))) != 0U); \
  } while(0)


#define ATOMIC_ADD(DATA, COUNT_NUMBER)                                 \
  do {                                                        \
    uint32_t val;                                             \
    do {                                                      \
      val = __LDREXW((__IO uint32_t *)&(DATA));               \
      val += COUNT_NUMBER;                                                  \
    } while ((__STREXW(val,(__IO uint32_t *)&(DATA))) != 0U); \
  } while(0)
    

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

uint16_t caculate_number_data_avalable(const ringBufS *ring);

//Put a mul byte from the queue
STATUS_RING ringBuf_push(ringBufS *ring, char buf[], uint16_t length);

STATUS_RING ringBuf_flush(ringBufS *ring);



#endif