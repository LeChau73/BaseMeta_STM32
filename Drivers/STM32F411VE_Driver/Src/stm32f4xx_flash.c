#include "stm32f4xx_flash.h"

#define KEY1 0x45670123UL
#define KEY2 0xCDEF89ABUL

static void Flash_Unlock()
{
    FLASH->KEYR = KEY1;
    FLASH->KEYR = KEY2;
}



static void Flash_Lock()
{
    SET_BIT(FLASH->CR, FLASH_CR_LOCK);
}

static Flash_Status Flash_SectorErase( const uint8_t n_sector , uint8_t VoltageRange)
{
    uint32_t tmp_psize = 0U;
    Flash_Status flashStatus = FLASH_OK;

    /* Check the parameters */
    assert_param(IS_FLASH_SECTOR(n_sector));
    assert_param(IS_VOLTAGERANGE(VoltageRange));

    if (VoltageRange == FLASH_VOLTAGE_RANGE_1)
    {
      tmp_psize = FLASH_PSIZE_BYTE;
    }
    else if (VoltageRange == FLASH_VOLTAGE_RANGE_2)
    {
      tmp_psize = FLASH_PSIZE_HALF_WORD;
    }
    else if (VoltageRange == FLASH_VOLTAGE_RANGE_3)
    {
      tmp_psize = FLASH_PSIZE_WORD;
    }
    else
    {
      tmp_psize = FLASH_PSIZE_DOUBLE_WORD;
    }

    /* Check that no Flash memory operation is ongoing by checking the BSY bit in the 
FLASH_SR register */
    while(READ_BIT(FLASH->SR, FLASH_SR_BSY_Msk))
    {
        // Flash memory operation ongoing
    }
    /* Set the SER bit and select the sector out of the 7 sectors (STM32F411xC/E) in the 
main memory block */
    CLEAR_BIT(FLASH->CR, FLASH_CR_PSIZE);
    FLASH->CR |= tmp_psize;
    SET_BIT(FLASH->SR, FLASH_CR_SER_Msk );

    /* Selection sector number which you wish to erase */
    uint32_t temp = 0x00000078;
    CLEAR_BIT(FLASH->CR, temp);
    temp = 0;
    temp = ( n_sector << 3 );
    SET_BIT(FLASH->CR, temp);

    /* Set the STRT bit in the FLASH_CR register */
    SET_BIT(FLASH->CR, FLASH_CR_STRT_Msk);

    /* Wait for the BSY bit to be cleared */
    while(READ_BIT(FLASH->SR, FLASH_SR_BSY_Msk));

    return  flashStatus;
}


static void FLASH_MassErase(uint8_t VoltageRange)
{
  /* Check the parameters */
  assert_param(IS_VOLTAGERANGE(VoltageRange));

  /* If the previous operation is completed, proceed to erase all sectors */
  CLEAR_BIT(FLASH->CR, FLASH_CR_PSIZE);
  FLASH->CR |= FLASH_CR_MER;
  FLASH->CR |= FLASH_CR_STRT | ((uint32_t)VoltageRange << 8U);
}
