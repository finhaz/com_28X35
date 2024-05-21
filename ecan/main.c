/*
 *     main.c
 *
 *     Created on: 2018-6-5
 *     Author: naiyangui,fin
 *     启动电流为2A，保护为8A
 *     把弱磁电压外环调节时间改成和转速外环一样为5ms
 *     定位角度11pi/6
 *     电流环：Kp=20;Ki=0.0001;限幅值：155；
 *     电压外环：Kp:0.001;Ki；0.0001；限流：-6;
 *     转速环：Kp=0.0025(2000rad/s以前)；Ki=0.00025  限幅值：=-6
 *          Kp=0.002(3000rad/s以前)；Ki=0.00025   限幅值：=-6
 *          Kp=0.003(4000rad/s以前)；Ki=0.00025   限幅值：=-6
 *          Kp=0.004(5000rad/s以前)；Ki=0.00025   限幅值：=-6
 *          Kp=0.005(7000rad/s以前)；Ki=0.00025   限幅值：=-6
 *     角度补偿：电流补偿一个周期角度，电压补偿2个周期角度
 *     停机制动：封锁pwm,零矢量
 *      把弱磁电压外环改为1个周期调节一次
 *      开关频率改为10K
 *      带载转速：17000rpm
 *      增加了减速斜坡模块
 *      最终停机使用版本(iq<0,封锁PWM。延时通直流量)
 *      中断时间缩短
 *      修改切换方案
 *      主控芯片从28279D改为28335
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
