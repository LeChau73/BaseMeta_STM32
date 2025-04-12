#include <stdint.h>


#ifdef __cplusplus
extern "C"
{
#endif

/* following defines should be used for structure members */
#define __IM volatile const /*! Defines 'read only' structure member permissions */
#define __OM volatile       /*! Defines 'write only' structure member permissions */
#define __IOM volatile      /*! Defines 'read / write' structure member permissions */


    typedef struct
    {
        __OM union
        {
            __OM uint8_t u8;   // Dùng cho các thao tác với 8 bit
            __OM uint16_t u16; // Dùng cho các thao tác với 16 bit
            __OM uint32_t u32; // Dùng cho các thao tác với 32 bit
        } PORT[32U]; // Cung cấp khả năng truy cập linh hoạt cho các kích thước dữ liệu khác nhau
        __IOM uint32_t RESERVED0[864U];
        __IOM uint32_t TER; /*!< Offset: 0xE00 (R/W)  ITM Trace Enable Register */
        uint32_t RESERVED1[15U];
        __IOM uint32_t TPR; /*!< Offset: 0xE40 (R/W)  ITM Trace Privilege Register */
        uint32_t RESERVED2[15U];
        __IOM uint32_t TCR; /*!< Offset: 0xE80 (R/W)  ITM Trace Control Register */
        uint32_t RESERVED3[29U];
        __OM uint32_t IWR;   /*!< Offset: 0xEF8 ( /W)  ITM Integration Write Register */
        __IM uint32_t IRR;   /*!< Offset: 0xEFC (R/ )  ITM Integration Read Register */
        __IOM uint32_t IMCR; /*!< Offset: 0xF00 (R/W)  ITM Integration Mode Control Register */
        uint32_t RESERVED4[43U];
        __OM uint32_t LAR; /*!< Offset: 0xFB0 ( /W)  ITM Lock Access Register */
        __IM uint32_t LSR; /*!< Offset: 0xFB4 (R/ )  ITM Lock Status Register */
        uint32_t RESERVED5[6U];
        __IM uint32_t PID4; /*!< Offset: 0xFD0 (R/ )  ITM Peripheral Identification
                               Register #4 */
        __IM uint32_t PID5; /*!< Offset: 0xFD4 (R/ )  ITM Peripheral Identification
                               Register #5 */
        __IM uint32_t PID6; /*!< Offset: 0xFD8 (R/ )  ITM Peripheral Identification
                               Register #6 */
        __IM uint32_t PID7; /*!< Offset: 0xFDC (R/ )  ITM Peripheral Identification
                               Register #7 */
        __IM uint32_t PID0; /*!< Offset: 0xFE0 (R/ )  ITM Peripheral Identification
                               Register #0 */
        __IM uint32_t PID1; /*!< Offset: 0xFE4 (R/ )  ITM Peripheral Identification
                               Register #1 */
        __IM uint32_t PID2; /*!< Offset: 0xFE8 (R/ )  ITM Peripheral Identification
                               Register #2 */
        __IM uint32_t PID3; /*!< Offset: 0xFEC (R/ )  ITM Peripheral Identification
                               Register #3 */
        __IM uint32_t CID0; /*!< Offset: 0xFF0 (R/ )  ITM Component  Identification
                               Register #0 */
        __IM uint32_t CID1; /*!< Offset: 0xFF4 (R/ )  ITM Component  Identification
                               Register #1 */
        __IM uint32_t CID2; /*!< Offset: 0xFF8 (R/ )  ITM Component  Identification
                               Register #2 */
        __IM uint32_t CID3; /*!< Offset: 0xFFC (R/ )  ITM Component  Identification
                               Register #3 */
    } ITM_Type;



// ITM Trace Control Register Definitions
#define ITM_TCR_ITMENA_Pos        1U                                      /* Position 1 */
#define ITM_TCR_ITMENA_Msk        (1U << ITM_TCR_ITMENA_Pos)             /* Mask */

#define ITM_TCR_TSENA_Pos         2U                                      /* Position 2 */
#define ITM_TCR_TSENA_Msk         (1U << ITM_TCR_TSENA_Pos)              /* Mask */

#define ITM_TCR_SYNCENA_Pos       3U                                      /* Position 3 */
#define ITM_TCR_SYNCENA_Msk       (1U << ITM_TCR_SYNCENA_Pos)            /* Mask */

#define ITM_TCR_SWOENA_Pos        5U                                      /* Position 4 */
#define ITM_TCR_SWOENA_Msk        (1U << ITM_TCR_SWOENA_Pos)             /* Mask */

#define ITM_TCR_BUSY_Pos          24U                                     /* Position 4 */
#define ITM_TCR_BUSY_Msk          (1U << ITM_TCR_BUSY_Pos)               /* Mask */






    typedef struct
    {
        __IOM uint32_t CTRL;     /*!< Offset: 0x000 (R/W)  Control Register */
        __IOM uint32_t CYCCNT;   /*!< Offset: 0x004 (R/W)  Cycle Count Register */
        __IOM uint32_t CPICNT;   /*!< Offset: 0x008 (R/W)  CPI Count Register */
        __IOM uint32_t EXCCNT;   /*!< Offset: 0x00C (R/W)  Exception Overhead Count Register */
        __IOM uint32_t SLEEPCNT; /*!< Offset: 0x010 (R/W)  Sleep Count Register */
        __IOM uint32_t LSUCNT;   /*!< Offset: 0x014 (R/W)  LSU Count Register */
        __IOM uint32_t FOLDCNT;  /*!< Offset: 0x018 (R/W)  Folded-instruction Count Register */
        __IM uint32_t PCSR;      /*!< Offset: 0x01C (R/ )  Program Counter Sample Register */
        __IOM uint32_t COMP0;    /*!< Offset: 0x020 (R/W)  Comparator Register 0 */
        __IOM uint32_t MASK0;    /*!< Offset: 0x024 (R/W)  Mask Register 0 */
        __IOM uint32_t FUNCTION0; /*!< Offset: 0x028 (R/W)  Function Register 0 */
        uint32_t RESERVED0[1U];
        __IOM uint32_t COMP1;     /*!< Offset: 0x030 (R/W)  Comparator Register 1 */
        __IOM uint32_t MASK1;     /*!< Offset: 0x034 (R/W)  Mask Register 1 */
        __IOM uint32_t FUNCTION1; /*!< Offset: 0x038 (R/W)  Function Register 1 */
        uint32_t RESERVED1[1U];
        __IOM uint32_t COMP2;     /*!< Offset: 0x040 (R/W)  Comparator Register 2 */
        __IOM uint32_t MASK2;     /*!< Offset: 0x044 (R/W)  Mask Register 2 */
        __IOM uint32_t FUNCTION2; /*!< Offset: 0x048 (R/W)  Function Register 2 */
        uint32_t RESERVED2[1U];
        __IOM uint32_t COMP3;     /*!< Offset: 0x050 (R/W)  Comparator Register 3 */
        __IOM uint32_t MASK3;     /*!< Offset: 0x054 (R/W)  Mask Register 3 */
        __IOM uint32_t FUNCTION3; /*!< Offset: 0x058 (R/W)  Function Register 3 */
    } DWT_Type;

/* Debug Exception and Monitor Control Register Definitions */
#define CoreDebug_DEMCR_TRCENA_Pos         24U                                            /*!< CoreDebug DEMCR: TRCENA Position */
#define CoreDebug_DEMCR_TRCENA_Msk         (1UL << CoreDebug_DEMCR_TRCENA_Pos)            /*!< CoreDebug DEMCR: TRCENA Mask     */





    typedef struct
    {
        __IM uint32_t SSPSR; /*!< Offset: 0x000 (R/ )  Supported Parallel Port Size Register */
        __IOM uint32_t CSPSR; /*!< Offset: 0x004 (R/W)  Current Parallel Port Size Register */
        uint32_t RESERVED0[2U];
        __IOM uint32_t ACPR; /*!< Offset: 0x010 (R/W)  Asynchronous Clock Prescaler Register */
        uint32_t RESERVED1[55U];
        __IOM uint32_t SPPR; /*!< Offset: 0x0F0 (R/W)  Selected Pin Protocol Register */
        uint32_t RESERVED2[131U];
        __IM uint32_t FFSR;  /*!< Offset: 0x300 (R/ )  Formatter and Flush Status Register */
        __IOM uint32_t FFCR; /*!< Offset: 0x304 (R/W)  Formatter and Flush Control Register */
        __IM uint32_t FSCR;  /*!< Offset: 0x308 (R/ )  Formatter Synchronization
                                Counter Register */
        uint32_t RESERVED3[759U];
        __IM uint32_t TRIGGER;   /*!< Offset: 0xEE8 (R/ )  TRIGGER Register */
        __IM uint32_t FIFO0;     /*!< Offset: 0xEEC (R/ )  Integration ETM Data */
        __IM uint32_t ITATBCTR2; /*!< Offset: 0xEF0 (R/ )  ITATBCTR2 */
        uint32_t RESERVED4[1U];
        __IM uint32_t ITATBCTR0; /*!< Offset: 0xEF8 (R/ )  ITATBCTR0 */
        __IM uint32_t FIFO1;     /*!< Offset: 0xEFC (R/ )  Integration ITM Data */
        __IOM uint32_t ITCTRL;   /*!< Offset: 0xF00 (R/W)  Integration Mode Control */
        uint32_t RESERVED5[39U];
        __IOM uint32_t CLAIMSET; /*!< Offset: 0xFA0 (R/W)  Claim tag set */
        __IOM uint32_t CLAIMCLR; /*!< Offset: 0xFA4 (R/W)  Claim tag clear */
        uint32_t RESERVED7[8U];
        __IM uint32_t DEVID;   /*!< Offset: 0xFC8 (R/ )  TPIU_DEVID */
        __IM uint32_t DEVTYPE; /*!< Offset: 0xFCC (R/ )  TPIU_DEVTYPE */
    } TPI_Type;


/* TPI Selected Pin Protocol Register Definitions */
#define TPI_SPPR_TXMODE_Pos                 0U                                         /*!< TPI SPPR: TXMODE Position */
#define TPI_SPPR_TXMODE_Msk                (0x3UL /*<< TPI_SPPR_TXMODE_Pos*/)          /*!< TPI SPPR: TXMODE Mask */



/**
  \brief  Structure type to access the Core Debug Register (CoreDebug).
 */
typedef struct
{
  __IOM uint32_t DHCSR;                  /*!< Offset: 0x000 (R/W)  Debug Halting Control and Status Register */
  __OM  uint32_t DCRSR;                  /*!< Offset: 0x004 ( /W)  Debug Core Register Selector Register */
  __IOM uint32_t DCRDR;                  /*!< Offset: 0x008 (R/W)  Debug Core Register Data Register */
  __IOM uint32_t DEMCR;                  /*!< Offset: 0x00C (R/W)  Debug Exception and Monitor Control Register */
} CoreDebug_Type;


/* Memory mapping with hardware */



#define ITM_BASE            (0xE0000000UL)
#define DWT_BASE            (0xE0001000UL)            /*!< DWT Base Address */
#define TPI_BASE            (0xE0040000UL)            /*!< TPI Base Address */
#define CoreDebug_BASE      (0xE000EDF0UL)                            /*!< Core Debug Base Address */

#define ITM                 ((ITM_Type*)ITM_BASE) // Ép kiểu ITM_Type để có thể truy cập vào struct này,và nói rằng là một con trỏ kiểu ITM_Type đang trỏ đến ITM_BASE
#define DWT                 ((DWT_Type*)DWT_BASE)                   /*!< DWT configuration struct */
#define TPI                 ((TPI_Type*)TPI_BASE)                   /*!< TPI configuration struct */

#define CoreDebug           ((CoreDebug_Type *) CoreDebug_BASE)


#ifdef __cplusplus
}
#endif