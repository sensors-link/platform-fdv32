/**
 * @file phnx05.h
 * @author David Lin
 * @brief
 * @version 0.1
 * @date 2021-05-17
 *
 * @copyright Fanhai Data Tech. (c) 2021
 *
 */
#ifndef __PHNX05_H__
#define __PHNX05_H__

#define PLIC_BASE_ADDR 0x00001000
#define CLINT_BASE_ADDR 0x00001800
#include "risc_v_csr.h"

#ifdef cplusplus
extern "C" {
#endif
/* IO definitions (access restrictions to peripheral registers) */
/**

  <strong>IO Type Qualifiers</strong> are used
  \li to specify the access to peripheral variables.
  \li for automatic generation of peripheral register debug information.
  */
#ifdef __cplusplus
#define __I volatile /*!< Defines 'read only' permissions       */
#else
#define __I                                                                    \
    volatile const /*!< Defines 'read only' permissions                 */
#endif
#define __O volatile  /*!< Defines 'write only' permissions     */
#define __IO volatile /*!< Defines 'read / write' permissions   */

/* define interrupt number */
typedef enum IRQn {
    /* ----------     Phonix05 Specific Interrupt Numbers  ---------*/
    RSV_IRQn = 0,
    ADC_IRQn = 1,
    LPTIMER_IRQn = 2,
    TIMER0_IRQn = 3,
    TIMER1_IRQn = 4,
    TIMER2_IRQn = 5,
    TIMER3_IRQn = 6,
    TIMER4_IRQn = 7,
    UART1_IRQn = 8,
    ANAC_IRQn = 9,
    EFC_IRQn = 10,
    IOM_IRQn = 11,
} IRQn_Type;

/* ================================================================================*/
/* ================                       EFC ================ */
/* ================================================================================*/

/**
 * @brief Embeded Flash Control (EFC)
 */

typedef struct /*!< EFC Structure          */
{
    __IO unsigned int CR;   /*!< Control Register */
    __IO unsigned int TNVS; /*!< ROG/ERASE/CEb/NVR/ADDRESS to Web Setup time */
    __IO unsigned int
        TPROG; /*!< PROG/ERASE/CEb/NVR/ADDRESS to Web Setup time          */
    __IO unsigned int TPGS; /*!< WEb low to PROG2 high Setup time */
    __IO unsigned int TRCV; /*!< Program Recovery Time */
    __IO unsigned int TERS; /*!< Erase time */
    __O unsigned int WPT;   /*!< Write Protect Register     */
    __IO unsigned int OPR;  /*!< OPR Flow Register    */
    __IO unsigned int STS;  /*!< Verify Register    */
} EFC_Type;


/* ================================================================================
 */
/* ================                      SYSC ================ */
/* ================================================================================
 */

/**
 * @brief System Control (SYSC)
 */

typedef struct /*!< SYSC Structure             */
{
    __IO unsigned int CLKCTRCFG; /*!< CLKCTRCFG */
    __O unsigned int WRPROCFG;   /*!< WRPROCFG   */
    __IO unsigned int CLKENCFG;
    __O unsigned int MSFTRSTCFG;
    __IO unsigned int TESTCKSEL;
    __IO unsigned int ADCCLKDIV;
    __IO unsigned int RSTSTS;
} SYSC_Type;

/**
 * @brief TIMERS
 */

typedef struct {
    __IO unsigned int T0CR;
    __IO unsigned int T0;
    __IO unsigned int TIE;
    __IO unsigned int TIF;
    __IO unsigned int TCR1;
    __IO unsigned int TN1;
    __IO unsigned int PWMPD1;
    __IO unsigned int PWMDC1;
    __IO unsigned int TCR2;
    __IO unsigned int TN2;
    __IO unsigned int PWMPD2;
    __IO unsigned int PWMDC2;
    __IO unsigned int TCR3;
    __IO unsigned int TN3;
    __IO unsigned int PWMPD3;
    __IO unsigned int PWMDC3;
    __IO unsigned int TCR4;
    __IO unsigned int TN4;
    __IO unsigned int PWMPD4;
    __IO unsigned int PWMDC4;
    __IO unsigned int T0RLD;
} TIMERS_Type;

// define for timer1-timer4,no include timer0
typedef struct {
    __IO unsigned int TCR;
    __IO unsigned int TN;
    __IO unsigned int PWMPD;
    __IO unsigned int PWMDC;
} TIM_Type;

typedef struct {
    __IO unsigned int SCON;
    __IO unsigned int SBUF;
    __IO unsigned int SADDR;
    __IO unsigned int SADEN;
    __IO unsigned int ISR;
    __IO unsigned int BDIV;
} UART_Type;

typedef struct {
    __IO unsigned int DATA;
    __IO unsigned int OE;
    __IO unsigned int PU;
    __IO unsigned int PD;
    __IO unsigned int OTYPE;
    __IO unsigned int AINCTRL;
    __IO unsigned int DRS;
    __IO unsigned int AF0;
    __IO unsigned int AF1;
    __IO unsigned int INT_TYPE;
    __IO unsigned int INT_POLARITY;
    __IO unsigned int EXT_INTE;
    __IO unsigned int INTF;
    __IO unsigned int CTL;

} IOM_Type;

typedef struct {
    __IO unsigned int CR;
    __IO unsigned int WKCFG;
    __O unsigned int SOFTRST;
    __IO unsigned int WKSTS;
    __IO unsigned int RSTSTS;
    __IO unsigned int INTEN;
    __O unsigned int WPT;
    __I unsigned int RSV[1];
    __IO unsigned int BAKR;

} PMU_Type;

typedef struct {
    __IO unsigned int CR;
    __IO unsigned int CFG;
    __IO unsigned int CNT;
    __IO unsigned int INTSTS;

} LPTIM_Type;

typedef struct {
    __IO unsigned int RST;
    __IO unsigned int CR;

} WDT_Type;

/* enum definitions */
typedef enum { DISABLE = 0, ENABLE = !DISABLE } EventStatus, ControlStatus;
typedef enum { FALSE = 0, TRUE = !FALSE } BOOL;
typedef enum { RESET = 0, SET = 1, MAX = 0X7FFFFFFF } FlagStatus;
typedef enum { ERROR = 0, SUCCESS = !ERROR } ErrStatus;

/* bit operations */
#define REG32(addr) (*((volatile unsigned int *)(addr)))
#define REG16(addr) (*((volatile unsigned short *)(addr)))
#define REG8(addr) (*((volatile unsigned char *)(addr)))
#define BIT(x) ((unsigned int)0x01U << (x))
#define BITS(start, end)                                                       \
    ((0xFFFFFFFFUL << (start)) & (0xFFFFFFFFUL >> (31U - (end))))
#define GET_BITS(regval, start, end)                                           \
    (((regval)&BITS((start), (end))) >> (start))

/* main flash and SRAM memory map */

#define FLASH_BASE (0x00002000UL) /*!< main FLASH base address */
#define SRAM_BASE (0x0000B000UL)  /*!< SRAM0 base address      */

/* ================================================================================
 */
/* ================              Peripheral memory map ================ */
/* ================================================================================
 */
#define EFC_BASE 0x0000C000UL
#define SYSC_BASE 0x0000C400UL
#define TIMERS_BASE 0x0000D000UL
#define TIM1_BASE 0x0000D010UL
#define TIM2_BASE 0x0000D020UL
#define TIM3_BASE 0x0000D030UL
#define TIM4_BASE 0x0000D040UL
#define UART1_BASE 0x0000D400UL
#define ANAC_BASE 0x0000CC00UL
#define IOM_BASE 0x0000C800UL
#define LPTIM_BASE 0x0000F800UL
#define WDT_BASE 0x0000FC00UL

/* ================================================================================
 */
/* ================             Peripheral declaration ================ */
/* ================================================================================
 */
#define EFC ((EFC_Type *)EFC_BASE)
#define SYSC ((SYSC_Type *)SYSC_BASE)
#define TIMERS ((TIMERS_Type *)TIMERS_BASE)
#define TIM1 ((TIM_Type *)TIM1_BASE)
#define TIM2 ((TIM_Type *)TIM2_BASE)
#define TIM3 ((TIM_Type *)TIM3_BASE)
#define TIM4 ((TIM_Type *)TIM4_BASE)
#define UART1 ((UART_Type *)UART1_BASE)
#define ANAC ((ANAC_Type *)ANAC_BASE)
#define IOM ((IOM_Type *)IOM_BASE)
#define LPTIM ((LPTIM_Type *)LPTIM_BASE)
#define WDT ((WDT_Type *)WDT_BASE)

/** @} */ /* End of group Device_Peripheral_Registers */
/** @} */ /* End of group Phonix05 */
/** @} */ /* End of group FHSJDZ */

// data type define
typedef unsigned int u32;
typedef unsigned short u16;
typedef unsigned char u8;

typedef signed int s32;
typedef signed short s16;
typedef signed char s8;

#ifdef _FW_DEBUG
extern int __wrap_printf(const char *fmt, ...);
#define PARAM_CHECK(x)                                                         \
    {                                                                          \
        if (x) {                                                               \
            __wrap_printf("Wrong parameters value: file:%s,on line:%d\r\n",    \
                          __FILE__, __LINE__);                                 \
            while (1)                                                          \
                ;                                                              \
        }                                                                      \
    }
#else
#define PARAM_CHECK(x)
#endif

#ifdef cplusplus
}
#endif

#endif /* __PHNX05_H__ */
