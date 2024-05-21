/*
 *     main.c
 *
 *     Created on: 2018-6-5
 *     Author: naiyangui,fin
 *     28335CAN
 */

#include "DSP28x_Project.h"     // DSP28x Headerfile
#include "math.h"
#include "bsp_ecan.h"
#include "Can_message.h"

__interrupt void Ecan0ISR(void);//R  接收后进入的中断
//_iq a,b,c;

/**
 * main.c
 */
void main(void)
{
    InitSysCtrl();

    DINT;
    InitPieCtrl();
    IER = 0x0000;
    IFR = 0x0000;
    InitPieVectTable();

    InitECanGpio();
    ConfigureEcan();

    EALLOW;
    PieVectTable.ECAN0INTA = &Ecan0ISR;//R-CAN1  接收后中断函数
    EDIS;

    //使能CPU中断
    IER |=M_INT9;//SCI中断     开CPU中断1~9(必须开放对应的CPU级中断口)
    IER |=M_INT1;//cputime and adc and外部中断
    //使能PIE中断
    PieCtrlRegs.PIECTRL.bit.ENPIE = 1;   // Enable the PIE block
    PieCtrlRegs.PIEIER9.bit.INTx5=1;     //R-CAN0  接收邮箱

    //开全局中断
    EINT;//使能全局中断（开中断）（CPU级的）
    ERTM;//使能实时中断（CPU级的）

    ///////////////////////////////////////初始化结束
    while(1)
    {
        Checkdata();
        CanSend();
    }

}


//

__interrupt void Ecan0ISR(void)//R  接收后进入的中断
{
     //DINT;
    if(ECanaRegs.CANRMP.all==0x00010000)//RX get after flag and int   BOX16
    {
        ECanaRegs.CANRMP.all = 0x00010000;//clear GMIF16
        CanRecieve();
    }
    PieCtrlRegs.PIEACK.all = PIEACK_GROUP9;

    //EINT;
}
