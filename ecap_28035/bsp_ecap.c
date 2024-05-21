/*
 * bap_ecap.c
 *
 *  Created on: 2021年1月6日
 *      Author: sea
 */
#include "DSP28x_Project.h"     // DSP28x Headerfile



Uint32 fpwm=0;
#define SYSCLKOUT_MHZ 60000000

void ConfigureEcap(void)
{
    EALLOW;

    //enable clock
    SysCtrlRegs.PCLKCR1.bit.ECAP1ENCLK = 1;// enable clk

    // configure CAP1
    ECap1Regs.ECEINT.all = 0x0000;               // Disable all capture interrupts
    ECap1Regs.ECCLR.all = 0xFFFF;              // Clear all CAP interrupt flags
    ECap1Regs.ECCTL2.bit.TSCTRSTOP = 0;          // Make sure the counter is stopped
    ECap1Regs.TSCTR = 0;

    // Configure peripheral registers
    ECap1Regs.ECCTL2.bit.CAP_APWM = 0;    // ECAP mode
    ECap1Regs.ECCTL1.bit.CAP1POL = 1;        // Falling edge
    ECap1Regs.ECCTL1.bit.CTRRST1 = 1;        // Reset counter after CaptureEvent1 time-stamp has been captured
    ECap1Regs.ECCTL1.bit.CAPLDEN = 1;        // Enable CAP1-CAP4 register loads

    ECap1Regs.ECCTL2.bit.STOP_WRAP=0;      // only use CAP1
    ECap1Regs.ECCTL2.bit.CONT_ONESHT=0;    // continue mode
    ECap1Regs.ECEINT.bit.CEVT1 = 1;        // 1 events = interrupt
    ECap1Regs.ECEINT.bit.CTROVF = 1;         // overflow interrupt

    ECap1Regs.ECCTL2.bit.TSCTRSTOP = 1;     // Start Counter

    EDIS;
}


__interrupt void ecap1_isr(void)
{
    if(ECap1Regs.ECFLG.bit.CEVT1)
    {
        ECap1Regs.ECCLR.bit.CEVT1 = 1;

        ECap1Regs.TSCTR = 0;

        fpwm = SYSCLKOUT_MHZ/ECap1Regs.CAP1;

    }

    if(ECap1Regs.ECFLG.bit.CTROVF)
    {
        ECap1Regs.ECCLR.bit.CTROVF=1;
    }

    ECap1Regs.ECCLR.bit.INT=1;//不能省略，否则无法再次进入中断

    PieCtrlRegs.PIEACK.all = PIEACK_GROUP4;
}
