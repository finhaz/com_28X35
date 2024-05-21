/*
 * bsp_epwm.c
 *
 *  Created on: 2021年1月6日
 *      Author: sea
 */

#include "DSP28x_Project.h"     // DSP28x Headerfile

#define Prd 1500

void ConfigureEPwm(void)
{
    //EPWM1-2 6 DRIVE

    //Note that the default/2 divider for ePWMs and EMIFs
    //EPWMCLK=SYSCLKOUT=60MHZ
    //ePWM模块时基设置  // TBCLK = EPWMCLK/(HSPCLKDIV*CLKDIV)
    //PWM周期=10000个TBCLK周期100us

    // Initialize EPwm1/2/3/4/5/6/7
    //自增配置程序------------->>>>>>>>>>>>>
    EALLOW;
    SysCtrlRegs.PCLKCR0.bit.TBCLKSYNC = 0;
    EDIS;
    //EPWM Moudle 1 配置
    //TB
    EPwm1Regs.TBCTL.bit.HSPCLKDIV=0;
    EPwm1Regs.TBCTL.bit.CLKDIV=0;
    EPwm1Regs.TBPRD=Prd;
    EPwm1Regs.TBPHS.half.TBPHS=0;//将相位寄存器值清零
    EPwm1Regs.TBCTR=0x0000;
    EPwm1Regs.TBCTL.bit.CTRMODE=TB_COUNT_UPDOWN;//增减模式
    EPwm1Regs.TBCTL.bit.PHSEN=TB_DISABLE;//禁止相位装载
    EPwm1Regs.TBCTL.bit.PRDLD=TB_SHADOW;//当CTR=0时，将映射寄存器中的数据装载到当前寄存器
    EPwm1Regs.TBCTL.bit.SYNCOSEL=TB_CTR_ZERO;//CTR=0时发出同步信号
    //CC
    EPwm1Regs.CMPCTL.bit.SHDWAMODE=CC_SHADOW;//CMPA寄存器工作在 映射模式
    EPwm1Regs.CMPCTL.bit.SHDWBMODE=CC_SHADOW;//CMPB寄存器工作在 映射模式
    EPwm1Regs.CMPCTL.bit.LOADAMODE=CC_CTR_ZERO;//在CTR=0时装载
    EPwm1Regs.CMPCTL.bit.LOADBMODE=CC_CTR_ZERO;//在CTR=0时装载
    //AQ
    EPwm1Regs.AQCTLA.bit.CAU=AQ_SET;//当时间基准计数器的值等于CMPA的值，且正在递增计数，使EPWM1A为高电平
    EPwm1Regs.AQCTLA.bit.CAD=AQ_CLEAR;//当时间基准计数器的值等于CMPA的值，且正在递减计数，使EPWM1A为低电平

     //DB
    EPwm1Regs.AQCSFRC.all = 0x0005 ;//软件强制PWM输出为低,2017/12/26
    //EPwm1Regs.DBCTL.bit.OUT_MODE=DB_FULL_ENABLE;//使能上升沿及下降沿延时信号
    EPwm1Regs.DBCTL.bit.IN_MODE=DBA_ALL;//默认EPWM1A作为上升沿及下降沿延时的信号源
    EPwm1Regs.DBCTL.bit.POLSEL=DB_ACTV_HIC;//AHC\EPWM1B反转极性
    EPwm1Regs.DBFED=120;//下降沿延时FED=120个TBCLK
    EPwm1Regs.DBRED=120;//上升沿延时RED=120个TBCLK
    EPwm1Regs.CMPA.half.CMPA=0.5*Prd;


    //事件触发设置ADC
    EPwm1Regs.ETSEL.bit.SOCAEN  = 1;         // 使能SOCA转换器工作
    EPwm1Regs.ETSEL.bit.SOCASEL = ET_CTR_ZERO;//计数器为0时触发转换
    EPwm1Regs.ETPS.bit.SOCAPRD  = ET_1ST; // Generate pulse on 1st event

    //EPWM Moudle 2 配置
    //TB
    EPwm2Regs.TBCTL.bit.HSPCLKDIV=0;
    EPwm2Regs.TBCTL.bit.CLKDIV=0;
    EPwm2Regs.TBPRD=Prd;
    EPwm2Regs.TBPHS.half.TBPHS=0;//将相位寄存器值清零
    EPwm2Regs.TBCTR=0x0000;
    EPwm2Regs.TBCTL.bit.CTRMODE=TB_COUNT_UPDOWN;//增减模式
    EPwm2Regs.TBCTL.bit.PHSEN=TB_DISABLE;//禁止相位装载
    EPwm2Regs.TBCTL.bit.PRDLD=TB_SHADOW;//当CTR=0时，将映射寄存器中的数据装载到当前寄存器
    EPwm2Regs.TBCTL.bit.SYNCOSEL=TB_SYNC_IN;//CTR=0时接收同步信号
    //EPwm2Regs.TBCTL.bit.SYNCOSEL=TB_CTR_ZERO;//CTR=0时发出同步信号
    //CC
    EPwm2Regs.CMPCTL.bit.SHDWAMODE=CC_SHADOW;//CMPA寄存器工作在 映射模式
    EPwm2Regs.CMPCTL.bit.SHDWBMODE=CC_SHADOW;//CMPB寄存器工作在 映射模式
    EPwm2Regs.CMPCTL.bit.LOADAMODE=CC_CTR_ZERO;//在CTR=0时装载
    EPwm2Regs.CMPCTL.bit.LOADBMODE=CC_CTR_ZERO;//在CTR=0时装载
    //AQ
    EPwm2Regs.AQCTLA.bit.CAU=AQ_SET;//当时间基准计数器的值等于CMPA的值，且正在递增计数，使EPWM1A为高电平
    EPwm2Regs.AQCTLA.bit.CAD=AQ_CLEAR;//当时间基准计数器的值等于CMPA的值，且正在递减计数，使EPWM1A为低电平

     //DB
    EPwm2Regs.AQCSFRC.all = 0x0005 ;//软件强制PWM输出为低,2017/12/26
    //EPwm1Regs.DBCTL.bit.OUT_MODE=DB_FULL_ENABLE;//使能上升沿及下降沿延时信号
    EPwm2Regs.DBCTL.bit.IN_MODE=DBA_ALL;//默认EPWM1A作为上升沿及下降沿延时的信号源
    EPwm2Regs.DBCTL.bit.POLSEL=DB_ACTV_HIC;//AHC\EPWM1B反转极性
    EPwm2Regs.DBFED=120;//下降沿延时FED=120个TBCLK
    EPwm2Regs.DBRED=120;//上升沿延时RED=120个TBCLK
    EPwm2Regs.CMPA.half.CMPA=0.5*Prd;

    //TBD
    EALLOW;
    SysCtrlRegs.PCLKCR0.bit.TBCLKSYNC = 1;         // Start all the timers synced
    EDIS;
}

//打开PWM
void PWMopen(void)
{
    //开关机判断
    //ePWM1封锁
    EPwm1Regs.AQSFRC.bit.RLDCSF = 0;//
    EPwm1Regs.AQCSFRC.all = 0x0000 ;//无动作
    EPwm1Regs.DBCTL.bit.OUT_MODE=3;

    //ePWM2封锁
    EPwm2Regs.AQSFRC.bit.RLDCSF = 0 ;//
    EPwm2Regs.AQCSFRC.all = 0x0000 ;//无动作
    EPwm2Regs.DBCTL.bit.OUT_MODE=3;


}
