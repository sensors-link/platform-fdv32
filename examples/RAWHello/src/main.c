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

void DelayNus(int delay)
{
    static uint8_t DelayAdjust[] = {0,17,8,4,1,0};
    delay -= DelayAdjust[HRC_FSEL];
    if (delay <= 0 ) return;
    uint32_t start = TIM1->CTVAL;
    while(TIME_DIFF(start, TIM1->CTVAL) < delay) __asm("NOP");
}

int main() {
    while(1){
        printf("Hello\n");
        DelayNus(1000000);
    }
}

void LPT_IrqHandler(void)
{
    LPTIM->INTSTS |= BIT(0); // 清中断标记
}
