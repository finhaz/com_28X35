#include "DSP28x_Project.h"     // DSP28x Headerfile
#include "bsp_ecap.h"
#include "bsp_epwm.h"


__interrupt void ecap1_isr(void);


void main(void)
{

    /* Create a shadow register structure for the CAN control registers. This is
    needed, since, only 32-bit access is allowed to these registers. 16-bit access
    to these registers could potentially corrupt the register contents. This is
    especially true while writing to a bit (or group of bits) among bits 16 - 31 */

    // Step 1. Initialize System Control:
    // PLL, WatchDog, enable Peripheral Clocks
    // This example function is found in the DSP2833x_SysCtrl.c file.
    InitSysCtrl();


    // Just initalize eCAN pins for this example
    // This function is in DSP2833x_ECan.c


    // Step 3. Clear all interrupts and initialize PIE vector table:
    // Disable CPU interrupts
    DINT;

    // Initialize the PIE control registers to their default state.
    // The default state is all PIE interrupts disabled and flags
    // are cleared.
    // This function is found in the DSP2833x_PieCtrl.c file.
    InitPieCtrl();

    // Disable CPU interrupts and clear all CPU interrupt flags:
    IER = 0x0000;
    IFR = 0x0000;

    // Initialize the PIE vector table with pointers to the shell Interrupt
    // Service Routines (ISR).
    // This will populate the entire table, even if the interrupt
    // is not used in this example.  This is useful for debug purposes.
    // The shell ISR routines are found in DSP2833x_DefaultIsr.c.
    // This function is found in DSP2833x_PieVect.c.
    InitPieVectTable();

    // Interrupts that are used in this example are re-mapped to
    // ISR functions found within this file.

    InitEPwmGpio();
    InitECapGpio();

    ConfigureEPwm();
    PWMopen();
    ConfigureEcap();



    EALLOW;  // This is needed to write to EALLOW protected registers
    PieVectTable.ECAP1_INT = ecap1_isr;
    EDIS;   // This is needed to disable write to EALLOW protected registers

    IER |=M_INT4;// 开CPU中断1~9(必须开放对应的CPU级中断口)

    PieCtrlRegs.PIECTRL.bit.ENPIE = 1;   // Enable the PIE block
    PieCtrlRegs.PIEIER4.bit.INTx1 = 1; //CAP1 interrupt


    EINT;//开总中断
    ERTM;//使能实时中断（CPU级的）



    while(1)
    {

    }


}


