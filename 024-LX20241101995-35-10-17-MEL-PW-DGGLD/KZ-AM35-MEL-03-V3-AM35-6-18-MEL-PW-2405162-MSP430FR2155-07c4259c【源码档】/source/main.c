/**********************************************************************************
 *  项目编号:
 *  项目名称:
 *  客户名称:
 *  程序编写:
 *  启动日期:
 *  完成日期:
 *  版本号码:    V1.0
 *  功能描述:
 *  芯片选型:
 *  振荡频率:
 *  Watchdog:
 *  MAIN.C file
**********************************************************************************/
#include "Define.h"//全局宏定义
#include "Extern.h"//全局变量及函数
//--------------------------------------------------------------------------------------
//  函数名:  main
//  功  能:  
//  输  入:  空
//  输  出:  空
//  返  回:  空
//  描  述： 
//--------------------------------------------------------------------------------------
int main (void)
{
  Initialize();//AM35-开关电源-WIFI
  
  while(1)
  {
    VCC_ON();/*打开电源*/
    
    if (((!MotorFlag.Motor_Run)   && (!Flag.Del_Code_Finish) &&
         (!Flag.Opretion_Finish)) || (motor_run_time >= NO_VOL_SAMPLING_TIME))//避开电机启动瞬间AD采样
    {
      ADSamping();//ad采样
    }
    //RF解码---------------------------------------------------------------------------
    if (Flag_RF.Raise_Edge)//是否有上升沿
    {
      Flag_RF.Raise_Edge = NULL;
      if (RFSignalDecode(RF_High_Cnt,RF_Low_Cnt))//RF解码
      {
        if (RFDataVerify())//数据是否正确
        {
          RFDataDisposal();//RF数据处理
        }
      }
    } 
    //200us -----------------------------------------------------------------------------    
     if (Flag.Time_200us)
     {
       Flag.Time_200us = FALSE;
       //DetectPowerIn(POWER_IN);
       DetectPowerIn();
     }
    //2ms -----------------------------------------------------------------------------
    if (Flag.Time_2ms)
    {
      Flag.Time_2ms = NULL; 
      PowerInScan();             //LIU     
      TimeControl();
      
      JourneyComper();//比较行程
      
      KeyManage();//按键扫描及处理
      
      if(Flag.Uart_tx)
      {
        Flag.Uart_tx = FALSE;
        Uart1StartupTX();//485
      }
      
      if (Flag.Uart_Read_OK)
      {
        Flag.Uart_Read_OK = FALSE;
        Uart1DataDisposal();//485
      }
      Uart1_Status_Inspect();
              
      UartStartupTX();//涂鸦WIFI
      UartDataDisposal();//涂鸦WIFI
    }
    //10ms ----------------------------------------------------------------------------
    if (Flag.Time_10ms)
    {
      Flag.Time_10ms = NULL;
      //计算转速 ----------------------------------------------------------------------
      if (MotorFlag.Hall_Pulse_edge)
      {
        MotorFlag.Hall_Pulse_edge = NULL;
        CountMotorSpeed();//计算出电机的转速
      }
      
      MotorControl();//电机控制
      MotorStatusSwitch();//电机状态切换
      

    }
    //50ms ----------------------------------------------------------------------------
    if (Flag.Time_50ms)
    {
      Flag.Time_50ms = NULL;
      RFStatusInspect();
      SetLimitPoint();

      if (Flag.Delete_Code)//是否删全码
      {
        DeleteAllID(0xee);
      }
      
    }
    //100ms ---------------------------------------------------------------------------
    if (Flag.Time_100ms)
    {
      Flag.Time_100ms = NULL;
      
      DblclickTimeLimit();//多限位点双击
      if(respons_tc) respons_tc--;
  
      if(tx_ok_tc)
      {
        if(--tx_ok_tc==0)
        {
          if(flag_tx)
          {
            if((tx_data != 0x06)||(++tx_data_i > tx_data_n))
            {
              if(--tx_c==0)
              {
                flag_tx=0; 
              }
              else 
              {
                respons_tc=40;
              }
            }
          }
        }
      }
      t50ms_c ++ ;
      if ((!Flag.Del_Code_Finish) && (!Flag.Opretion_Finish))
      {
      if((Motor_Status_bak != Motor.Motor_Status)&&(t50ms_c>=5))//状态变化主动上报
      {
        Motor_Status_bak=Motor.Motor_Status;
        if(Motor.Motor_Status == MOTOR_STOP)
        {
          Send_Data(1,0x06,4,2);
          t50ms_c=0;
        }
        else
        {
          Send_Data(1,0x06,2,1);
          t50ms_c=0;
        }
      }
      if((Motor_Flag_Direction_bak!=MotorFlag.Direction)&&(t50ms_c>=5))          //方向主动上报
      {
        Send_Data(1,0x06, 6,4);
        t50ms_c=0;
        Motor_Flag_Direction_bak=MotorFlag.Direction;
      }
      }
    }
    //1s-------------------------------------------------------------------------------
    if (Flag.Time_1s)
    {
      Flag.Time_1s = NULL;
      Flag.Electrify_Reset = TRUE;              //禁止上电对码  

      LearnDelCodeTime();
    }
  }
}