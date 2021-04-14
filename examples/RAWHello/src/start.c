/*
 ============================================================================
 Name        : start.c
 Author      : Raymond Wu
 Version     :
 Copyright   : sensors.link
 Description : RISCV entry file in C, for phoenix MCUs
 ============================================================================
 */
#include "phnx02.h"
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

extern uint32_t _data_lma[], _data[], _edata[], __bss_start[], _end[];
extern void __libc_fini_array();
extern void __libc_init_array();
extern int main();

unsigned int SystemCoreClock = 8000000;
void _init(void) __attribute__((weak));
void _fini(void) __attribute__((weak));

void STUB_IrqHandler(void);

/** 异常处理的桩函数 **/
void MSOFT_IntHandler(void) __attribute__((weak, alias("STUB_IrqHandler")));
void MEXP_Handler(void) __attribute__((weak, alias("STUB_IrqHandler")));
void NMI_Handler(void) __attribute__((weak, alias("STUB_IrqHandler")));

/** 外部中断处理的桩函数 **/
void PMU_IrqHandler(void) __attribute__((weak, alias("STUB_IrqHandler")));
void LPT_IrqHandler(void) __attribute__((weak, alias("STUB_IrqHandler")));
void TIMER1_IrqHandler(void) __attribute__((weak, alias("STUB_IrqHandler")));
void TIMER2_IrqHandler(void) __attribute__((weak, alias("STUB_IrqHandler")));
void TIMER3_IrqHandler(void) __attribute__((weak, alias("STUB_IrqHandler")));
void TIMER4_IrqHandler(void) __attribute__((weak, alias("STUB_IrqHandler")));
void UART1_IrqHandler(void) __attribute__((weak, alias("STUB_IrqHandler")));
void UART2_IrqHandler(void) __attribute__((weak, alias("STUB_IrqHandler")));
void SPI_IrqHandler(void) __attribute__((weak, alias("STUB_IrqHandler")));
void ANAC_IrqHandler(void) __attribute__((weak, alias("STUB_IrqHandler")));
void EFC_IrqHandler(void) __attribute__((weak, alias("STUB_IrqHandler")));
void IOM_IrqHandler(void) __attribute__((weak, alias("STUB_IrqHandler")));
void I2C_IrqHandler(void) __attribute__((weak, alias("STUB_IrqHandler")));
void RTC_IrqHandler(void) __attribute__((weak, alias("STUB_IrqHandler")));
void TWC_IrqHandler(void) __attribute__((weak, alias("STUB_IrqHandler")));
void LPU_IrqHandler(void) __attribute__((weak, alias("STUB_IrqHandler")));

/** 中断向量表 **/
typedef void (*IRQ_FUNC)(void);
const IRQ_FUNC _irq_vect[] = {
    STUB_IrqHandler,   //
    PMU_IrqHandler,    // PMU_IRQn = 1
    LPT_IrqHandler,    // LPT_IRQn = 2
    TIMER1_IrqHandler, // TIMER1_IRQn = 3
    TIMER2_IrqHandler, // TIMER2_IRQn = 4
    TIMER3_IrqHandler, // TIMER3_IRQn = 5
    TIMER4_IrqHandler, // TIMER4_IRQn = 6
    UART1_IrqHandler,  // UART1_IRQn = 7
    UART2_IrqHandler,  // UART2_IRQn = 8
    SPI_IrqHandler,    // SPI_IRQn = 9
    ANAC_IrqHandler,   // ANAC_IRQn = 10
    EFC_IrqHandler,    // EFC_IRQn = 11
    IOM_IrqHandler,    // IOM_IRQn = 12
    I2C_IrqHandler,    // I2C_IRQn = 13
    RTC_IrqHandler,    // RTC_IRQn = 14
    TWC_IrqHandler,    // TWC_IRQn = 15
    LPU_IrqHandler,    // LPU_IRQn = 16
};

/**
 * @function: 中断处理函数
 */
void trap_entry(void) __attribute((interrupt));
void trap_entry() {
    /* 保存现场, __attribute((interrupt))函数自动保存 */

    uint32_t mcause = READ_CSR(mcause);
    uint32_t epc = READ_CSR(mepc);

    if (mcause == (MCAUSE_INT | EXP_M_EXT_INT)) {
        // 外部中断处理
        uint32_t src = PLIC_GetCLAIM();
        if (src < sizeof(_irq_vect) / sizeof(IRQ_FUNC)) {
            _irq_vect[src]();
        }
        PLIC_SetCLAIM(src);
    } else if (mcause == (MCAUSE_INT | EXP_M_SOFT_INT)) {
        // 软中断处理
        MSOFT_IntHandler();
    } else if (mcause == 0x1e) {
        // NMI中断处理
        NMI_Handler();
    } else {
        // 其他中断处理
        MEXP_Handler();
    }

    WRITE_CSR(mepc, epc);

    /* 恢复现场, __attribute((interrupt))函数自动恢复现场 */
    /* __attribute((interrupt))函数将调用mret返回 */
}

/**
 * @function: “真正的”入口函数
 */
void _start(void) __attribute((naked, section(".init")));
void _start(void) {
    // 禁止全局中断
    DisableGlobleIRQ();
    // 加载gp和sp
    __asm(".option push");
    __asm(".option norelax");
    __asm("la gp, __global_pointer$");
    __asm(".option pop");
    __asm("la sp, _sp");

    uint32_t *dst, *src;
    // 初始化.data段
    for (src = _data_lma, dst = _data; dst < _edata; *dst++ = *src++)
        ;
    // 初始化.bss段
    // cppcheck-suppress "comparePointers"
    for (dst = __bss_start; dst < _end; *dst++ = 0)
        ;

    atexit(__libc_fini_array);
    // __libc_init_array将调用_init函数
    __libc_init_array();

    // 初始化中断向量
    WRITE_CSR(mtvec, &trap_entry);
    // 开全局中断
    EnableGlobleIRQ();
    // 开外部中断
    EnableExtIRQ();

    // 进入主函数
    main();
    while (1)
        ;
}

/**
 * @function: 初始化硬件，weak函数，可以在应用中重载
 */
void _init(void) {
    // 时钟设置
    SYSC->CLKENCFG |= BIT(12) | BIT(10); // ANAC_PCKEN = 1, IOM_PCKEN = 1
    ANAC->WPROT = 0X5A5A;
    ANAC->WPROT = 0XA5A5;
    ANAC->CLK_CFG |= BIT(1) | BIT(15) | BIT(0); // HRC_EN = 1, MRC_EN = 1, LRC_EN = 1
    // 设置系统时钟
    ANAC->WPROT = 0X5A5A;
    ANAC->WPROT = 0XA5A5;
    ANAC->CLK_CFG |= 4 << 2; // 1=1M, 2=2M, 3=4M, 4=8M, 5=16M
}

void _fini() {}
void STUB_IrqHandler(void) {}
