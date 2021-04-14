/**
 * @file init.c
 * @author bifei.tang
 * @brief 初始化外设
 * @version 0.1
 * @date 2020-07-06
 *
 * @copyright Fanhai Data Tech. (c) 2020
 *
 */
#include "lib_include.h"

/**
 * @function: 初始化时钟
 */
static void _init_clock()
{
    SYSC->CLKENCFG |= BIT(12); // ANAC_PCKEN = 1
    ANAC->WPROT = 0x5A5A;
    ANAC->WPROT = 0xA5A5;
    ANAC->CLK_CFG |= BIT(1); // HRC_EN = 1
    // 设置系统时钟
    ANAC->WPROT = 0x5A5A;
    ANAC->WPROT = 0xA5A5;
    ANAC->CLK_CFG &= ~BITS(2,4);
    ANAC->WPROT = 0x5A5A;
    ANAC->WPROT = 0xA5A5;
    ANAC->CLK_CFG |= HRC_FSEL << 2;
    SystemCoreClock = 1000000;
    for (int i=1; i<HRC_FSEL; i++)
    SystemCoreClock = SystemCoreClock * 2;
}

/**
 * @function: 初始化调试串口
 */
static void _init_uart()
{
#ifdef _DEBUG
    // 使能UART1
    SYSC->CLKENCFG |= BIT(10); // IOM_PCKEN = 1
#ifndef _UART2
    IOM->AF1 &= ~BITS(0,3);
    IOM->AF1 |= (2 << 2) | 2; // P16_SEL=>UART1_RX, P17_SEL=>UART1_TX
    SYSC->CLKENCFG |= BIT(3); // UART1_PCKEN = 1
    UART1->SCON = 0;
    // 31250是各个HRC主频下都准确的波特率
    // UART1->BDIV = SystemCoreClock / 4000000 * 2 - 1;
    UART1->BDIV = SystemCoreClock / 1000000 * 2 - 1;

    UART1->SCON &= ~BITS(8,9);
    UART1->SCON |= 1 << 8; // 工作模式 = 模式1, 缺省值
    UART1->SCON |= BIT(6); // 接收使能
#else
    IOM->AF0 &= ~BITS(30,31);
    IOM->AF0 |= (0b1111 << 30); // P14_SEL=>UART2_RX, P15_SEL=>UART2_TX
    SYSC->CLKENCFG |= BIT(4); // UART2_PCKEN = 1
    UART2->SCON = 0;
    // 31250是各个HRC主频下都准确的波特率
    UART1->BDIV = SystemCoreClock / 4000000 * 2 - 1;
    // UART2->BDIV = SystemCoreClock / 1000000 * 2 - 1;

    UART2->SCON &= ~BITS(8,9);
    UART2->SCON |= 1 << 8; // 工作模式 = 模式1, 缺省值
    UART2->SCON |= BIT(6); // 接收使能
#endif
#endif
}

/**
 * @function: 初始化两总线控制器TWC
 */
static void _init_twc()
{
    SYSC->CLKENCFG |= BIT(10); // SYSC_CLKENCFG_IOM;

    // PIN10/11配置为TWC
    IOM->AF0 &= ~BITS(20,23); //
    IOM->AF0 |= (3 << 22) | (3 << 20);

    ANAC->WPROT = 0x5A5A;
    ANAC->WPROT = 0xA5A5;
    ANAC->CLK_CFG |= BIT(15); // MRC_EN = 1
    for (int i = 1000; i > 0; --i) __asm("NOP");

    SYSC->CLKENCFG |= (BIT(19) | BIT(21)); // TWC_PCKEN = 1, TWC_MRCKEN = 1

     // 关闭 PIN10 PIN11上拉
    IOM->PU &= ~BITS(10,11);

    // 中断使能
    PLIC_SetPriority(TWC_IRQn, 7);
    PLIC_EnableIRQ(TWC_IRQn);
    TWC->INTEN = BIT(1); // 正常接收数据完成中断使能

    // 命令字段 0x40 - 0x7F 为广播命令, 总是唤醒
    // 包含下面命令: CMD_WriteAddress CMD_WriteWorkModeX CMD_WriteBackground CMD_WriteAlarmX CMD_Broadcast
    TWC->CMD1 = ((0x40 << 9) << 16 ) | 0b0111111111111111;
    // 命令字段 0x20 - 0x3F 为广播命令, 总是唤醒
    // 包含下面命令: CMD_ReadAddress CMD_ReadWorkMode CMD_ReadBackgroundAlarm CMD_ReadCompensation CMD_ReadADvalue
    TWC->CMD2 = ((0x20 << 9) << 16 ) | 0b0011111111111111;

     // 设置TWC逻辑1拉码
    TWC->CR |= BIT(5);

}

/**
 * @function: 初始化低功耗定时器LPTimer
 */
static void _init_lpt()
{
    SYSC->CLKENCFG |= BIT(15); // lp timer apb clock使能
    SYSC->CLKENCFG |= BIT(13); // pmu apb clock使能
    PMU->WPT = 0xC3;
    PMU->WPT = 0x3C;
    PMU->CR &= ~BIT(5); // 常开电源域Low Power时钟选择=LRC时钟

    // LPTimer 设为100ms
    // 其中LPT计时时钟为LRC或者XTL时钟周期的32分频，即当使用LRC 8KHz时，计时周期为1000/250 ms=4ms
    // 计时长度为CFG+1个LPT周期
    LPTIM->CFG = 24;

    LPTIM->CR |= BIT(2); // PIT循环计数模式
    PMU->WPT = 0xC3;
    PMU->WPT = 0x3C;
    PMU->CR |= BIT(6); // Low Power Timer Clock使能
    LPTIM->CR |= BIT(0); // LPT定时器运行控制位, 启动

    PLIC_SetPriority(LPT_IRQn, 1);
    PLIC_EnableIRQ(LPT_IRQn);
    LPTIM->INTSTS |= BIT(0); // 清中断标记
    LPTIM->CR |= BIT(3); // LPT中断使能
}

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


/**
 * @brief 设置AD采样相关参数
 *
 */
void _init_SDC()
{
    // Step1: 配置 SYSC.ANCLKDIV，设置 ADC 采样时钟即转换速度。
    if (SystemCoreClock > 1000000)
        // ADC 采样时钟 = 1M, ANAC_500K_CLK=500K
        SYSC->ANCLKDIV = (SystemCoreClock/2000000 - 1) | 0x10;
    else
        // ADC 采样时钟 = 500K, ANAC_500K_CLK=250K
        // 采样太慢，连续结果只有4-5个有效
        SYSC->ANCLKDIV = 0x10;

    // Step2: 配置ADC
    // ADC 参考电压输入源 = 内部参考源voltRef
    // 使能内部参考电压模块
    // 内部参考电压档位 = 2.5V
    // ADC放大器使能
    // ADC输入通道 = PD SENSOR
    ANAC->ADC_CFG = 0b11001000111;

    // Step3: 配置 ADC.CTL
    // 硬件采样次数 = 8
    // 输入信号采样保持时间 = 4cycle
    // 多次自动转换使能
    // 启动控制 = 0
    // 禁止 ADC 中断。
    // 使能 ADC 模块。
    // ANAC->ADC_CTL = 0b11001001;
    ANAC->ADC_CTL = 0b11111001;

    // Step4: 配置DCDC
    // 7.1 DCDC输出电压档位 5.5V
    ANAC->DC_CFG &= ~BITS(3,4);
    ANAC->DC_CFG |= 0b01 << 3;
    // // 7.2 使能DCDC, 并等待DCDC输出电压稳定
    // ANAC->DC_CFG |= BIT(0);
    // while((ANAC->ANAC_FLAG & BIT(8)) == 0);
    // // 7.3 DCDC输出和ledDrv连接
    ANAC->DC_CFG &= ~BITS(9,10);
    ANAC->DC_CFG |= 0b10 << 9;

    // Step5:  红外红光管LED驱动电流调整 160mA
    ANAC->LED_CFG &= ~BITS(1,5);
    ANAC->LED_CFG |= 16 << 1;

    // Step6: PD Sensor增益配置 50/17dB
    ANAC->PDSENS_CFG &= BITS(2,6);
    ANAC->PDSENS_CFG |= 0b01111 << 2;

    // Step7: 配置ME_CTL
    // 配置 ME_CTL.ME_AUTO 启动自动测量模式。
    // 配置 ME_CTL.MODE_SEL 选择烟雾检测模式。
    // 配置 ME_CTL.ME_MODE 选择软件配置SST位触发转换
    ANAC->ME_CTL  |= BIT(4) | BITS(5,6);
    ANAC->ME_CTL &= ~BIT(3);
}

/**
 * @function: 初始化外设，重载系统weak函数
 */
void _init(void) {

    DisableGlobleIRQ();

    _init_clock();
    _init_uart();
    _init_twc();
    _init_lpt();
    _init_timer();
    // _init_SDC();

    // // 设置异步唤醒方式
    // PMU->WKCFG |= BIT(9);
    // // 加速从DEEPSLEEP唤醒
    // SYSC->WRPROCFG = 0x5A5A;
    // SYSC->WRPROCFG = 0xA5A5;
    // SYSC->CLKCTRCFG &= ~BITS(23,31);

    EnableGlobleIRQ();
}
