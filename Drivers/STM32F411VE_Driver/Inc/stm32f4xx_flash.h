#ifndef __STM32F4xx_ITM_h
#define __STM32F4xx_ITM_h

#include "core_m4.h"

#ifdef __cplusplus
extern "C"
{
#endif


typedef enum
{
    FLASH_ERASE_ERROR = -1,
    FLASH_WRITE_ERROR,
    FLASH_BUSY,
    FLASH_OK
} Flash_Status;


typedef enum
{
    MODE_X8 = 0,
    MODE_X16 ,
    MODE_X32 ,
    MODE_X64
} Flash_ModeWrite;


typedef struct
{
    Flash_Status flashStatus;
    void (*FunctionHandeler)( void );
} IRQHander;

static void Flash_Unlock();
static void Flash_Lock();
static Flash_Status Flash_SectorErase( const uint8_t n_sector , uint8_t VoltageRange);
static Flash_Status Flash_MassErase( const uint8_t n_sector );
static Flash_Status Flash_Programming(const uint64_t* data, const Flash_ModeWrite mode_write);
static Flash_Status Flash_ProgrammingIT(const uint64_t* data, const Flash_ModeWrite mode_write);
static Flash_Status Flash_SectorEraseIT( const uint8_t n_sector );
static Flash_Status Flash_MassEraseIT( const uint8_t n_sector );
static void Flash_IRQHandler(Flash_Status status);


#ifdef __cplusplus
}
#endif

#endif