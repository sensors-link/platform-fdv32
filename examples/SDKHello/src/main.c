/*
 ============================================================================
 Name        : main.c
 Author      : tbf
 Version     :
 Copyright   : Your copyright notice
 Description : Hello RISC-V World in C
 ============================================================================
 */

#include "lib_include.h"
#include <stdio.h>
#include <stdlib.h>

#define TIME_DIFF(s,e) ((s)-(int)(e))

/**
 * @function: 初始化Timer1, 用于DelayNus和总线高低电平计时
 */
static void _init_timer()
{
    // 初始化Timer1, 用于DelayNus和总线高低电平计时
    SYSC->CLKENCFG |= BIT(5); // Timer cfg pclock使能
    SYSC->CLKENCFG |= BIT(6); // timer1 cnt clock使能
    SYSC->TIMCLKDIV = SystemCoreClock / 1000000 - 1; // 1M
    TIMERS->INTCLR  |= BIT(0); // 清TIMER1中断标记
    // 32位自由运行模式
    TIM1->CTCG1    = 0;
    TIMERS->CON    |= BIT(0); // 使能TIMER1
}

void DelayNus(int delay)
{
    uint32_t start = TIM1->CTVAL;
    while(TIME_DIFF(start, TIM1->CTVAL) < delay) __asm("NOP");
}

int main() {
    _init_timer();
    UART_DeInit(UART1);
    UART_Init(UART1, UART1_PORT_P16_P17, UART_MODE_10B_ASYNC, 31250);
    while(1){
        printf("Hello\n");
        DelayNus(1000000);
    }
}
