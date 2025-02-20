/**********************************************************************************
 *  项目编号:    
 *  项目名称:    
 *  客户名称:                                    
 *  程序编写:    
 *  启动日期: 
 *  完成日期:    
 *  版本号码:    V1.0
 *
 *  .c file
 *  功能描述: 
**********************************************************************************/
#include "Define.h"//全局宏定义
#include "Extern.h"//全局变量及函数
//内部宏 -------------------------------------------------------------------------------
#define TIME0_INT_NUMBER       3//Time0中断次数

#define HALL_SR1               BIT6
#define HALL_SR2               BIT7
#define HALL1_IN()            (P3IN & HALL_SR1)//P3.6
#define HALL2_IN()            (P3IN & HALL_SR2)//P3.7

u8 Time2_Int_Cnt = TIME0_INT_NUMBER - 2;

u8 RF_Signal_Sample1;//数据电平采样
u8 RF_Signal_Sample2;//数据电平采样

u8 High_Time_Cnt;//数据时间缓存
u8 Low_Time_Cnt;//数据时间缓存
u8 Time_200uS_Cnt;//LIU

//--------------------------------------------------------------------------------------
//  函数名:   DetectHallSignal
//  功  能:   检测霍尔信号
//  输  入:   霍尔号
//  输  出:   霍尔脉冲宽度，边沿标志，电机实际运行方向标志
//--------------------------------------------------------------------------------------
void DetectHallSignal (void)
{
  u8 hall_value;
  
  hall_value = HALL1_IN();
  
  if (Hall_Time_Cnt < 15000) 
  {
    Hall_Time_Cnt++;
  }
  else
  {
    Motor_Speed = 0;
  }
  
  // 霍尔1为基准，是否有边沿触发
  if (Hall_Status_Buffer != hall_value)
  {
    // 连续3次防止误动作
    if (++hall_change_cnt >= 1)
    {
      hall_change_cnt = 0;
      
      // 确定运行方向
      MotorFlag.Motor_Fact_Direction = 0;
      if (hall_value != 0)// 0 -> 1
      {
        if (!HALL2_IN())
        {
          MotorFlag.Motor_Fact_Direction = TRUE;
        }
      }
      else// 1 -> 0
      {
        if (HALL2_IN())
        {
          MotorFlag.Motor_Fact_Direction = TRUE;
        }
      }
      
      // 读取脉冲宽度
      Hall_Pulse_Width = Hall_Time_Cnt;
      Hall_Time_Cnt = 0;
      MotorFlag.Hall_Pulse_edge = TRUE;  
      
      if (MotorFlag.Motor_Fact_Direction ^ MotorFlag.Journey_Direction)
      {
        // 是否在最大范围内
        if (Hall_Pulse_Amount_Cnt < HALL_PULSE_MAX)
        {
          ++Hall_Pulse_Amount_Cnt;
        }
      }
      else
      {
        // 是否在最大范围内
        if (Hall_Pulse_Amount_Cnt > HALL_PULSE_MIN)
        {
          Hall_Pulse_Amount_Cnt--;
        }
      }
      Hall_Status_Buffer = hall_value;
    }
  }
}
//--------------------------------------------------------------------------------------
//  函数名:  Timer3_B0
//  功能:    Timer3_B0中断
//--------------------------------------------------------------------------------------
#pragma vector=TIMER3_B0_VECTOR
__interrupt void Timer3_B0(void)
{
  TB3CCR0 += RF_DECODE_TIME * SYSTEM_CLOCK;//40 uS
  //---------------------------------------------------------------------------------
  // RF信号采样
  if (RF_SIGNAL_SAMPLE())
  {
    RF_Signal_Sample1++;
  }
  
    //---------------------------------------------------------------------------------
  
  if (--Time_200uS_Cnt == 0)
  {
    Flag.Time_200us = TRUE;
    Time_200uS_Cnt = TIME_200US_VALUE;
  }
  
  //---------------------------------------------------------------------------------
  DetectHallSignal();
  //---------------------------------------------------------------------------------
  // 连续3次确定电平状态，并判断是否为边沿信号
  if (--Time2_Int_Cnt == 0)
  {
    Time2_Int_Cnt = TIME0_INT_NUMBER;
    
    // 两次以上为高则为高电平，否则为低电平
    if (RF_Signal_Sample1 >> 1)
    {
      // 上次为低电平则有上升沿信号
      if (!Flag_RF.Signal_Status)
      {
        if (!Flag_RF.Raise_Edge)
        {
          //上升沿时，如果3次里面有低电平，则补充低电平时间
          //高电平时间留给个周期
          RF_Low_Cnt_Buffer += 3 - RF_Signal_Sample1;//低电平有几次则加几
          // 取本周期内高低电平的时间
          RF_High_Cnt = RF_High_Cnt_Buffer;          
          RF_Low_Cnt = RF_Low_Cnt_Buffer;
          //低电平重新计数，高电平时间留给下个周期
          RF_Low_Cnt_Buffer = 0;
          RF_High_Cnt_Buffer = RF_Signal_Sample1;
          // 置RF上升沿标志  
          Flag_RF.Raise_Edge = TRUE;
        }
      }
      else
      {
        // 只要3次有2次以上是高，则记录3次高电平时间
        RF_High_Cnt_Buffer += 3;
      }
      Flag_RF.Signal_Status = TRUE;
    }
    else
    {
      // 上次为高电平则有下降沿信号
      if (Flag_RF.Signal_Status)
      {
        //下降沿时，如果3次里面有高电平，则补充高电平时间
        RF_High_Cnt_Buffer += RF_Signal_Sample1;//高电平有几次则加几
        RF_Low_Cnt_Buffer += 3 - RF_Signal_Sample1;//低电平有几次则加几
      }
      else
      {
        //只要3次有2次以上是低，则记录3次低电平时间
        RF_Low_Cnt_Buffer += 3;
      }
      Flag_RF.Signal_Status = 0;
    }
    RF_Signal_Sample1 = 0;
  }   
} //中断结束并返回
