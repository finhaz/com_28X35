#include "DSP28x_Project.h"     // DSP28x Headerfile
#include "bsp_ecan.h"
#include "bsp_spi.h"
#include "Can_message.h"

__interrupt void Ecan0ISR(void);
__interrupt void SPIISR(void);
//手动控制标志
int test_b=0;
int test_c=0;
//发送与结束的变量
float Ret_var1=3.2;
Uint16 numv=46;
float Ret_var3=0;

Uint16 Ret_var2=0x4100;
Uint16 Sta_var2=0;
Uint16 Ret_var4=0;
Uint16 Sta_var4=0;
Uint16 ParametI[256];
Uint16 spi_cnt=0;

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


    InitECanGpio();
    Initspigpio();

    ConfigureEcan();

#ifdef SPI_rec
    ConfigureSpi2();
#else
    ConfigureSpi();
#endif

    EALLOW;  // This is needed to write to EALLOW protected registers
    PieVectTable.ECAN0INTA = &Ecan0ISR;//R-CAN1  接收后中断函数

#ifdef SPI_rec
    PieVectTable.SPIRXINTA =&SPIISR;//R-SPI  接收后中断函数
#endif

    EDIS;   // This is needed to disable write to EALLOW protected registers

    IER |=M_INT9;// 开CPU中断1~9(必须开放对应的CPU级中断口)

    PieCtrlRegs.PIECTRL.bit.ENPIE = 1;   // Enable the PIE block
    PieCtrlRegs.PIEIER9.bit.INTx5=1;     //R-CAN0  接收邮箱

#ifdef SPI_rec
    PieCtrlRegs.PIEIER6.bit.INTx1=1;     // Enable PIE Group 6, INT 1
#endif

    EINT;//开总中断
    ERTM;//使能实时中断（CPU级的）

#ifdef SPI_rec
    for(i=0;i<255;i++)
        ParametI[i]=0;
#endif

    while(1)
    {
        Checkdata();
        CanSend();

#ifndef SPI_rec
        if(test_b)
        {
            //Write_to_25LC640F(numv,Ret_var1);
            //Write_to_25LC640(numv,Ret_var2);

//            int i=0;
//            for(i=0;i<256;i++)
//            {
//                Ret_var2=i<<8;
//                Write_to_25LC640_OneByte(numv+i,Ret_var2);
//                DELAY_US(10);
//            }

//            int i=0;
//            for(i=0;i<256;i++)
//            {
//                Ret_var2=i<<8;
//                Write_to_25LC640(numv+i,Ret_var2);
//                DELAY_US(10);
//            }
            numv++;
            Write_to_25LC640_OneByte(numv,Ret_var2);

//            SpiWrite_Data(numv,Ret_var2);

            Sta_var2=Read_State();
            test_b=0;
        }

        if(test_c)
        {
            //Ret_var3=Read_From_25LC640F(numv);
            //Ret_var4=Read_From_25LC640(numv);

//            int i=0;
//            for(i=0;i<256;i++)
//            {
//                Ret_var4=Read_From_25LC640_OneByte(numv+i);
//                ParametI[i]=Ret_var4;
//                DELAY_US(10);
//            }

//            int i=0;
//            for(i=0;i<256;i++)
//            {
//                Ret_var4=Read_From_25LC640(numv+i);
//                ParametI[i]=Ret_var4;
//                DELAY_US(10);
//            }


            Ret_var4=Read_From_25LC640_OneByte(numv);

//            Ret_var4=SpiRead_Data(numv);
            Sta_var4=Read_State();

            test_c=0;
        }
#endif

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

#ifdef SPI_rec
__interrupt void SPIISR(void)//R  接收后进入的中断
{
    ParametI[spi_cnt]=SpiaRegs.SPIRXBUF&0x00ff;
    spi_cnt++;
    if(spi_cnt==250)
        spi_cnt=0;
    PieCtrlRegs.PIEACK.all = PIEACK_GROUP6;
}
#endif
