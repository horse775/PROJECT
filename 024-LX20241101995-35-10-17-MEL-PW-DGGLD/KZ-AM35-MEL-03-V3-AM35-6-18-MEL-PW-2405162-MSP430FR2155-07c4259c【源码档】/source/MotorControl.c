/**********************************************************************************
 *  项目编号:    
 *  项目名称:    
 *  客户名称:                                    
 *  程序编写:    
 *  启动日期:    
 *  完成日期:    
 *  版本号码:    V1.0
 *  
 *  MotorControl.c file
 *  功能描述: 
**********************************************************************************/
#include "Define.h"                                          /* 全局宏定义 ------*/
#include "Extern.h"                                          /* 全局变量及函数 --*/
/* 内部宏 -----------------------------------------------------------------------*/
#define NO_SPEED_TIME          (1000 / (TIME_10MS_VALUE * SYSTEM_TIME))//无转速保护时间
#define OVERLOAD_TIME          (500  / (TIME_10MS_VALUE * SYSTEM_TIME))//过载持续的时间
#define SLOW_SPEED_TIME        (300  / (TIME_10MS_VALUE * SYSTEM_TIME))

#define INCHING_ANGLE             15*3// 微动的角度
#define INCHING_ANGLE_TURNS       (REDUCTION_RATIO/28*INCHING_ANGLE/180+1)//马达转减速比的霍尔计数等于输出轴转半圈180度
#define JIGGLE_ANGLE_TURNS(var)   (REDUCTION_RATIO/28*(var*3)/180)//点动角度20级//(((((var) * 3) * 10) + 7) / 14)


static volatile u16 Meet_Plugb_Cnt;
static volatile u16 Meet_Plugb_Cnt1;
static volatile u16 Meet_Plugb_Cnt2;
static volatile u16 Meet_Plugb_Cnt3;

static volatile u32 Hall_Pulse_Amount_Buff;

static volatile u16 Meet_Plugb_Cnt;
static volatile u16 Motor_RunTime_Cnt = MOTOR_RUN_TIME;
static volatile u16 Current_Current_mA_Buff;
static u16 Meet_Plugb_Rebound_Time_Cnt;

u8 Motor_Speed_Buff;
u8 speed_control_cnt;//调整速度的时间
u8 curr_over_on_cnt;//遇阻时间计数器
u8 curr_over_off_cnt;//遇阻时间计数器

static u16 ADC_CUR_Array[4];
static u8 ADC_CUR_Array_Index;

//内部函数------------------------------------------------------------------------------
void MotorStatusSwitch (void);
u8 MeetPlugbDetect (void);
void SlowStop (u32 target);

//-------------------------------------------------------------------------------------
//  函数名:   CountMotorSpeed
//  功  能:   
//-------------------------------------------------------------------------------------
void CountMotorSpeed (void)
{
  if (Hall_Pulse_Width < 35)
  {
    Motor_Speed = 200;
  }
  else if (Hall_Pulse_Width > 15000)
  {
    Motor_Speed = 0;
  }
  else if (Hall_Pulse_Width > 337)
  {
    Motor_Speed = ((u8) SPEED_COUNT(Hall_Pulse_Width));//返回每分钟的转速
  }
  else
  {
    ADC_CUR_Array[ADC_CUR_Array_Index] = Hall_Pulse_Width;
    ADC_CUR_Array_Index++;
    if (ADC_CUR_Array_Index >> 2)//是否存过4次
    {
      u16 temp;
      
      temp = SampleAverage(ADC_CUR_Array,ADC_CUR_Array_Index);
      ADC_CUR_Array_Index = NULL;
    
      Motor_Speed = ((u8) SPEED_COUNT(temp));//返回每分钟的转速
    }
  }
}
//-------------------------------------------------------------------------------------
//  函数名:   MeetPlugbDetect
//  功  能:   
//-------------------------------------------------------------------------------------
u8 MeetPlugbDetect (void)
{
  u8 flag = NULL;
  
  //连续一秒无转速
  if (Motor_Speed == 0)
  {
    if (++Meet_Plugb_Cnt >= NO_SPEED_TIME)
    {
      Meet_Plugb_Cnt = 0;
      flag = TRUE;
    }
  }
  else
  {
    Meet_Plugb_Cnt = 0;
  }
  //-----------------------------------------------------------------------------------
  //0.5秒速度很低并且电流大
  if (Motor_Speed <= MIN_SPEED)
  {
    if (Current_Current_mA >= (STARTUP_CURRENT+(Meet_Plugb_Level*50)))
    {
      if (++Meet_Plugb_Cnt1 >= OVERLOAD_TIME)
      {
        Meet_Plugb_Cnt1 = 0;
        flag = TRUE;
      }
    }
  }
  else
  {
    Meet_Plugb_Cnt1 = 0;
  }
  //-----------------------------------------------------------------------------------
  //0.3秒电流大且转速慢
  if (++Meet_Plugb_Cnt2 >= SLOW_SPEED_TIME)
  {
    Meet_Plugb_Cnt2 = NULL;
    
    if (Current_Current_mA >= (STARTUP_CURRENT+(Meet_Plugb_Level*50)))
    {
      if ((Hall_Pulse_Amount_Cnt < (Hall_Pulse_Amount_Buff + 4))
          && (Hall_Pulse_Amount_Cnt > (Hall_Pulse_Amount_Buff - 4)))
      {
        flag = TRUE;
      }
    }
    Hall_Pulse_Amount_Buff = Hall_Pulse_Amount_Cnt;
  }
  //-----------------------------------------------------------------------------------
  if (!MOTOR_NFAULT())
  {
    if (++Meet_Plugb_Cnt3 >= SLOW_SPEED_TIME)
    {
      Meet_Plugb_Cnt3 = NULL;
      flag = TRUE;
    }
  }
  else
  {
    Meet_Plugb_Cnt3 = NULL;
  }
  //-----------------------------------------------------------------------------------
  return (flag);
}
//-------------------------------------------------------------------------------------
//  函数名:   SlowStop
//  功  能:   缓停
//-------------------------------------------------------------------------------------
void SlowStop (u32 target)
{
#define NUM     ((REDUCTION_RATIO/10)<320?(REDUCTION_RATIO/10):320)
  u16 temp;

  if ((Hall_Pulse_Amount_Cnt < (target + NUM)) && (Hall_Pulse_Amount_Cnt > (target - NUM)))
  {
    //Flag.slowstop = TRUE;   
    //(Motor_Array[0].Motor_Flag.Run_Direction ^ Motor_Array[0].Motor_Flag.Journey_Direction)
    if(target > Hall_Pulse_Amount_Cnt)
    {
      temp = target - Hall_Pulse_Amount_Cnt;
    }
    else
    {
      temp = Hall_Pulse_Amount_Cnt - target;
    }
    
    temp = (MAX_SPEED-MIN_SPEED) * temp / NUM + MIN_SPEED+2;
    if(Target_Speed > temp)
      Target_Speed = temp;
  }
  else
  {
    Flag.slowstop = NULL;
  }
}

/***************************************************************************
*封装霍尔脉冲计数范围判断函数--20250218ry
*  输  入:   空
//  输  出:   空
//  返  回:   空
//  描  述：
***************************************************************************/
int IsHallPulseOutOfRange(void)
{
    if (!(MotorFlag.Motor_Direction ^ MotorFlag.Journey_Direction))
    {
        if (Hall_Pulse_Amount_Cnt < (HALL_PULSE_MIN + 100))//霍尔脉冲计数是否超出了设定的最小范围
        {
            return 1;
        }
    }
    else
    {
        if (Hall_Pulse_Amount_Cnt > (HALL_PULSE_MAX - 100))//霍尔脉冲计数是否超出了设定的最大范围
        {
            return 1;
        }
    }
    return 0;
}
/***************************************************************************
//  函数名:   JourneyComper
//  功  能:   
//  输  入:   空
//  输  出:   空
//  返  回:   空
//  描  述：  
***************************************************************************/
void JourneyComper (void)
{
  u8 flag_stop = NULL;
  u8 flag_up_limit=NULL;
  
  //是否设定了限位点
  if ((MotorFlag.Motor_Run) && (Limit_Point[0].Flag.F1.Limit_Activate))
  {
    //上限位反弹定时2025-2-17
     if(Meet_Plugb_Rebound_Time != 0)
    {
      // 遇阻反弹标志置1
      if(Flag.Meet_Plugb_Rebound)
      {
        if(Meet_Plugb_Rebound_Time != 0xff)                   // 位0xff不限制时间
        {
          if(++Meet_Plugb_Rebound_Time_Cnt  >= ((100 / SYSTEM_TIME) * Meet_Plugb_Rebound_Time))   //遇阻反弹单位0.1秒
          {
            flag_stop = TRUE;
            Flag.Meet_Plugb_Rebound = NULL;//clear flag
            Meet_Plugb_Rebound_Time_Cnt = 0;//重置计数器
          }
        }
      }
    }
    else
    {
      Flag.Meet_Plugb_Rebound = 0;//如果 Meet_Plugb_Rebound_Time 为 0，则清除 Flag.Meet_Plugb_Rebound 标志
    }
    //霍尔脉冲计数范围判断
    if(IsHallPulseOutOfRange())
    {
      flag_stop = TRUE;//停止运行
    }
    
    if ((!Flag.NO_Comper_Limit_Point) && (!Limit_Point[0].Flag.F1.Fine_Adjust))//不检查限位&&没有微调限位点
    {
      //是否在第一个限位点以外
      if (Hall_Pulse_Amount_Cnt < Limit_Point[0].Limit_Place+1)
      {
        //是否还要往外面走
        if (!(MotorFlag.Motor_Direction ^ MotorFlag.Journey_Direction))
        {
          flag_stop = TRUE;//停止运行
        }
      }
      else if (Run_To_Mid_Limit_Point != NULL)               // 是否要运行到中间限位点
      {
        if ((Hall_Pulse_Amount_Cnt > (Limit_Point[Run_To_Mid_Limit_Point].Limit_Place -2))
        && (Hall_Pulse_Amount_Cnt < (Limit_Point[Run_To_Mid_Limit_Point].Limit_Place + 2)))  
        {
          flag_stop = TRUE;
          Run_To_Mid_Limit_Point = NULL;
        }
      }
      else if (Flag.Goto_PCT)//目标百分比位置
      {
        if ((Hall_Pulse_Amount_Cnt > (Hall_Pulse_Amount_PCT - 1))
        && (Hall_Pulse_Amount_Cnt < (Hall_Pulse_Amount_PCT + 1)))
        {
          flag_stop = TRUE;
          Flag.Goto_PCT = FALSE;
          Flag.Angle_Stop = NULL ;//目标位置关调光LIU 
        }
      }

      else 
      {
         //正常限位
        if (Motor_Speed !=0 )   //LHD
          SlowStop (Limit_Point[Target_Limit_Point].Limit_Place);
        TEXT_1 = Target_Limit_Point;

        if (Flag.Journey_Arrive)//
        {
          if ((Hall_Pulse_Amount_Cnt > (Limit_Point[Target_Limit_Point].Limit_Place - 2))
          && (Hall_Pulse_Amount_Cnt < (Limit_Point[Target_Limit_Point].Limit_Place + 2)))
          {
            if ((!Flag.Del_Code_Finish) && (!Flag.Opretion_Finish))
            {
              flag_stop = TRUE;
            }
          }
        }
        
        if ((Hall_Pulse_Amount_Cnt > (Limit_Point[Target_Limit_Point].Limit_Place - 2))
        && (Hall_Pulse_Amount_Cnt < (Limit_Point[Target_Limit_Point].Limit_Place + 2)))  
        {
          if ((!Flag.Del_Code_Finish) && (!Flag.Opretion_Finish))
          {
            flag_stop = TRUE;
            Flag.Journey_Arrive = TRUE;
          }
        }
      } 
      //是否超出了最大的限位点
      if (Hall_Pulse_Amount_Cnt > Limit_Point[Limit_Point_Amount-1].Limit_Place-1)
      {
        //是否还要往限位点外走
        if (MotorFlag.Motor_Direction ^ MotorFlag.Journey_Direction)
        {
          flag_stop = TRUE;
        }
        else
        {
          //单个限位点 正常限位
          if (Motor_Speed !=0 )   //LHD
          SlowStop (Limit_Point[Target_Limit_Point].Limit_Place);
          TEXT_2 = 1;//缓存
        }
      }
      else //20250217ry
      {
        SlowStop (Limit_Point[Target_Limit_Point].Limit_Place);
        if(Flag.Control_Tighten_Up)               //无收紧功能
        {
          if ((Hall_Pulse_Amount_Cnt > (Limit_Point[Target_Limit_Point].Limit_Place - 3))
          && (Hall_Pulse_Amount_Cnt < (Limit_Point[Target_Limit_Point].Limit_Place + 3)))  
          {
            if ((!Flag.Del_Code_Finish) && (!Flag.Opretion_Finish))
            {
              
              flag_stop = TRUE; 
              
            }
          }  
        }
        else//有收紧功能Flag.Control_Tighten_Up==0
        {
          if ((Hall_Pulse_Amount_Cnt > (Limit_Point[Target_Limit_Point].Limit_Place - 2))
              && (Hall_Pulse_Amount_Cnt < (Limit_Point[Target_Limit_Point].Limit_Place + 2)))  
          {
            if ((!Flag.Del_Code_Finish) && (!Flag.Opretion_Finish))
            {//到上限位向下反弹2024-10-29
              flag_stop = TRUE;
              if(MotorFlag.Journey_Direction != MotorFlag.Direction)//行程方向标志 !=方向标志
              {
                if(Target_Limit_Point == 0)
                  flag_up_limit=1;
              }
              else if(Limit_Point_Amount > 1)
              {
                if(Target_Limit_Point == (Limit_Point_Amount - 1))
                  flag_up_limit=1;
              }
            }
          }
        } 
      }
      
    }
    
    //霍尔脉冲计数范围判断
    if(IsHallPulseOutOfRange())
    {
      flag_stop = TRUE;//停止flag
    }
  }
  
  if ((Flag.Del_Code_Finish) || (Flag.Opretion_Finish))//删码成功标志||操作成功标志
  {
    if (Flag.OprationCue_Run)//操作提示运行
    {
      if ((Hall_Pulse_Amount_Cnt > (OprationCue_Amount_Buf + INCHING_ANGLE_TURNS))
      || (Hall_Pulse_Amount_Cnt < (OprationCue_Amount_Buf - INCHING_ANGLE_TURNS)))
      {
        MotorStop();
        flag_stop = TRUE;
        Flag.OprationCue_Run = NULL;
      }
    }
    //---------------------------------------------------------------------------------
    if (Flag.OprationCue_Stop)//操作提示停止
    {
      if ((Hall_Pulse_Amount_Cnt > (OprationCue_Amount_Buf - 2))
      && (Hall_Pulse_Amount_Cnt < (OprationCue_Amount_Buf + 2)))
      {
        MotorStop();
        flag_stop = TRUE;
        Flag.OprationCue_Stop = NULL;
      }
    }
  }
  else
  {
    if (Flag.Jiggle_Stop)//点动停止标志
    {
      //Target_Speed = MAX_SPEED;
      if (Motor_Duty < MAX_DUTY)
      {
        Motor_Duty++;
      }
      
      if ((Hall_Pulse_Amount_Cnt > (Hall_Pulse_Jiggle_Buff + JIGGLE_ANGLE_TURNS(Joint_Time_Level)))// 5  9度
      || (Hall_Pulse_Amount_Cnt < (Hall_Pulse_Jiggle_Buff - JIGGLE_ANGLE_TURNS(Joint_Time_Level))))
      {
       flag_stop = TRUE;
        Flag.Jiggle_Stop = NULL;
      }
    }
    
       else if (Flag.Angle_Stop)//运行角度停止标志
    {
      //Target_Speed = MAX_SPEED;
      if (Motor_Duty < MAX_DUTY/2)
      {
        Motor_Duty++;
      }
      
      if ((Hall_Pulse_Amount_Cnt > (Hall_Pulse_Jiggle_Buff + Click_the_Angle_of_hall))// 5  9度
      || (Hall_Pulse_Amount_Cnt < (Hall_Pulse_Jiggle_Buff - Click_the_Angle_of_hall)))
      {
       flag_stop = TRUE;
        Flag.Angle_Stop = NULL;
      }
    }
  }
  
  if (flag_stop)
  {
    Flag.Goto_PCT = FALSE;
    Flag.Angle_Stop = NULL ;//关调光LIU 
    MotorFlag.Motor_Run = NULL;
    Motor.Motor_Status = MOTOR_STOP;
    MotorStop();
    BRAKE_ON();//立即刹车，断开继电器    
  }
  
  //上限位反弹1S2025-2-17
  if(flag_up_limit)
  {
    if(!Flag.Meet_Plugb_Rebound)
    {
      Flag.Meet_Plugb_Rebound = TRUE;                // 反弹
      Meet_Plugb_Rebound_Time = 10;//到限位反弹1S
      Meet_Plugb_Rebound_Time_Cnt = 0;
      Motor.Motor_Status = MOTOR_DOWN;//向下反弹
      Flag.Up_Meet_Plugb = TRUE;//反弹后上行遇阻标志置1，电机需反向解锁，才能上行
    }
  }
}

/**********************************************************************************
  函数名:   Angle_calculation
  功  能:   当上行或者下行后，步进的霍尔脉冲数>=帘子转动180度需要的霍尔值，百叶帘的读数恢复180°或者0°
            运动状态改变且当前状态为停止时上传数据
            当前角度计算
*********************************************************************************/
void Angle_calculation   (void)
{
  if((Motor_Status_Buffer_1 != Motor.Motor_Run_Status))//当上行或者下行后，步进的脉冲数大于125*1.5，百叶帘的读数恢复180°或者0°
  {      

/*-----------------------------huang----赋予初始角度--------------------------------------------*/
    if(!MotorFlag.Journey_Direction)//默认行程方向
    {
      if((Motor_Status_Buffer_1 == MOTOR_UP) && (Motor.Motor_Run_Status == MOTOR_STOP))//上一状态为上行且这一状态为停止
      {/*电机停止之后才进来*/            
        if(MotorFlag.Direction == 0)//默认方向OK
        {
          if(Flag.Angle_mark == 0)//以180度为起点且有上行操作
          {
            Hall_difference = (Hall_Pulse_Amount_up - Hall_Pulse_Amount_Cnt);//现在的霍尔值-上次赋予的0度或者180度时的霍尔值
            
           
          }          
          else//以0度为起点的上行操作
          {
            Hall_difference = (Hall_Pulse_Amount_Cnt -  Hall_Pulse_Amount_up);
          }            
          if(Hall_difference > CURRENT_ANGLE_180_HALL || ((Hall_Pulse_Amount_Cnt > Hall_Pulse_Amount_up) && Flag.Angle_mark == 0))
          {//上行到0度保存值之外

            Flag.Angle_mark = 0;//置0度为起点标志位
            Current_Angle  = 180;//角度置180
            Hall_difference = 0;//霍尔差值清0，重新计数
            Hall_Pulse_Amount_up =  Hall_Pulse_Amount_Cnt ;//存储新的一次180度时的位置
          }
        } 
        else//反向OK
        {            
          if(Flag.Angle_mark == 1)//以0度为起点且有上行操作
          {
            Hall_difference = (Hall_Pulse_Amount_Cnt - Hall_Pulse_Amount_up);//现在的霍尔值-上次赋予的0度或者180度时的霍尔值
          }          
          else//以180度为起点的上行操作
          {  
            Hall_difference = (Hall_Pulse_Amount_up -  Hall_Pulse_Amount_Cnt);
          }             
          if(Hall_difference > CURRENT_ANGLE_180_HALL || ((Hall_Pulse_Amount_Cnt < Hall_Pulse_Amount_up) && Flag.Angle_mark == 1))
          {

            Flag.Angle_mark = 1;//置0度为起点标志位
            Current_Angle  = 0; //角度清0  
            Hall_difference = 0;//霍尔差值清0，重新计数
            Hall_Pulse_Amount_up = Hall_Pulse_Amount_Cnt;//存储新的一次0度时的位置
          } 
        }
      }        
      if((Motor_Status_Buffer_1 == MOTOR_DOWN) && (Motor.Motor_Run_Status == MOTOR_STOP))//上一状态为下行且这一状态为停止
      {/*电机停止之后才进来*/                     
        if(MotorFlag.Direction == 0)//默认方向OK
        {
          if(Flag.Angle_mark == 1)//以0度为起点且有下行操作
          {
            Hall_difference = (Hall_Pulse_Amount_Cnt - Hall_Pulse_Amount_up);
          }     
          else//以180度为起点的下行操作
          {
            Hall_difference = (Hall_Pulse_Amount_up -  Hall_Pulse_Amount_Cnt);
          }               
          if(Hall_difference > CURRENT_ANGLE_180_HALL || ((Hall_Pulse_Amount_Cnt < Hall_Pulse_Amount_up) && Flag.Angle_mark == 1))
          {

            Flag.Angle_mark = 1;//置0度为起点标志位
            Current_Angle  = 0; //角度清0  
            Hall_difference = 0;//霍尔差值清0，重新计数
            Hall_Pulse_Amount_up = Hall_Pulse_Amount_Cnt;//存储新的一次0度时的位置
          } 
        } 
        else//反向OK
        {
          if(Flag.Angle_mark == 0)//以180度为起点且有下行操作
          {
            Hall_difference = (Hall_Pulse_Amount_up - Hall_Pulse_Amount_Cnt);
          }     
          else //以0度为起点
          {
            Hall_difference = (Hall_Pulse_Amount_Cnt -  Hall_Pulse_Amount_up);
          }               
          if(Hall_difference > CURRENT_ANGLE_180_HALL || ((Hall_Pulse_Amount_Cnt > Hall_Pulse_Amount_up) && Flag.Angle_mark == 0))//
          {

            Flag.Angle_mark = 0;//置0度为起点标志位
            Current_Angle  = 180;//角度置180
            Hall_difference = 0;//霍尔差值清0，重新计数
            Hall_Pulse_Amount_up =  Hall_Pulse_Amount_Cnt ;//存储新的一次180度时的位置
          } 
        }
      }
    }
    else //反行程方向
    {
      if((Motor_Status_Buffer_1 == MOTOR_UP) && (Motor.Motor_Run_Status == MOTOR_STOP))//上一状态为上行
      {            
        if(MotorFlag.Direction == 0)//默认方向OK
        {
          if(Flag.Angle_mark == 1)//以0度为起点且有上行操作
          {
            Hall_difference = (Hall_Pulse_Amount_up - Hall_Pulse_Amount_Cnt);//现在的霍尔值-上次赋予的0度或者180度时的霍尔值
          }          
          else//以180度为起点的上行操作
          {
            Hall_difference = (Hall_Pulse_Amount_Cnt - Hall_Pulse_Amount_up);
          }            
          if(Hall_difference >= CURRENT_ANGLE_180_HALL/*以0度为起点*/ 
             || ((Hall_Pulse_Amount_up > Hall_Pulse_Amount_Cnt) && Flag.Angle_mark == 0/*以180度为起点*/))
          {//上行到0度保存值之外
            
     
            Flag.Angle_mark = 0;//置0度为起点标志位
            Current_Angle  = 180; //角度清0  
            Hall_difference = 0;//霍尔差值清0，重新计数
            Hall_Pulse_Amount_up = Hall_Pulse_Amount_Cnt;//存储新的一次0度时的位置
          }
        } 
        else//反向
        {            
          if(Flag.Angle_mark == 0)//以180度为起点且有上行操作
          {
            Hall_difference = (Hall_Pulse_Amount_Cnt - Hall_Pulse_Amount_up);//现在的霍尔值-上次赋予的0度或者180度时的霍尔值
          }          
          else//以0度为起点的上行操作
          {  
            Hall_difference = (Hall_Pulse_Amount_up - Hall_Pulse_Amount_Cnt);      
          }             
          if(Hall_difference > CURRENT_ANGLE_180_HALL/*以0度为起点*/ 
             || ((Hall_Pulse_Amount_Cnt > Hall_Pulse_Amount_up) && Flag.Angle_mark == 1/*以180度为起点*/))
          {
            Flag.Angle_mark = 1;//置180度为起点标志位
            Current_Angle  = 0;//角度置180
            Hall_difference = 0;//霍尔差值清0，重新计数
            Hall_Pulse_Amount_up =  Hall_Pulse_Amount_Cnt ;//存储新的一次180度时的位置
          } 
        }
      }        
      else if((Motor_Status_Buffer_1 == MOTOR_DOWN) && (Motor.Motor_Run_Status == MOTOR_STOP))
      {                     
        if(MotorFlag.Direction == 0)//默认方向OK
        {
          if(Flag.Angle_mark == 0)//以180度为起点且有下行操作
          {
            Hall_difference = (Hall_Pulse_Amount_Cnt - Hall_Pulse_Amount_up);
          }     
          else//以0度为起点的下行操作
          {
            Hall_difference = (Hall_Pulse_Amount_up - Hall_Pulse_Amount_Cnt);
          }               
          if(Hall_difference > CURRENT_ANGLE_180_HALL/*以0度为起点*/ 
             || ((Hall_Pulse_Amount_up < Hall_Pulse_Amount_Cnt) && Flag.Angle_mark == 1/*以180度为起点*/))
          { 

            Flag.Angle_mark = 1;//置180度为起点标志位
            Current_Angle  = 0;//角度置180
            Hall_difference = 0;//霍尔差值清0，重新计数
            Hall_Pulse_Amount_up =  Hall_Pulse_Amount_Cnt ;//存储新的一次180度时的位置
          } 
        } 
        else//反向
        {

          
          if(Flag.Angle_mark == 1)//以0度为起点且有下行操作
          {
            Hall_difference = (Hall_Pulse_Amount_up - Hall_Pulse_Amount_Cnt);//
          }     
          else //以180度为起点
          {
            Hall_difference = (Hall_Pulse_Amount_Cnt - Hall_Pulse_Amount_up);//
            

            
          }               
          if(Hall_difference > CURRENT_ANGLE_180_HALL/*以180度为起点*/ 
             || ((Hall_Pulse_Amount_up > Hall_Pulse_Amount_Cnt) && Flag.Angle_mark == 0/*以0度为起点*/))//1
          {

            Flag.Angle_mark = 0;//置0度为起点标志位
            Current_Angle  = 180;//角度清0
            Hall_difference = 0;//霍尔差值清0，重新计数
            Hall_Pulse_Amount_up = Hall_Pulse_Amount_Cnt ;//存储新的一次0度时的位置
          } 
        }
      }
    }
    
/*---------------------------------huang----角度计算--------------------------------------------*/
    if(Flag.Angle_mark)                              //以0度为起点的百叶帘角度计算
    {
      Current_Angle=Hall_difference*180/CURRENT_ANGLE_180_HALL;
      
      //UART_TEXT(Current_Angle,Hall_difference,Flag.Angle_mark,0,0,0);
      
    }
    else                                            //以180度为起点的百叶帘角度计算
    {
      Current_Angle=180-Hall_difference*180/CURRENT_ANGLE_180_HALL;       //百叶帘角度计算
  
      //UART_TEXT(Current_Angle,Hall_difference,Flag.Angle_mark,0,0,0);
    }
    
/*-----------------------------huang-------上传角度数据------------------------------------------*/   
    if(Motor.Motor_Run_Status == MOTOR_STOP)//这一状态为停止上传数据
    {  
      //Send_Data(1,0x07,8,5);The_network_jitter_once
    }   
    
/*-------------------------huang--------保存为上一状态------------------------------------------*/
    Motor_Status_Buffer_1 = Motor.Motor_Run_Status ;           //存储上一次电机运行时的状态
  }
}




//-------------------------------------------------------------------------------------
//  函数名:   MotorUp
//  功  能:   
//-------------------------------------------------------------------------------------
void MotorUp (u16 duty)
{
  if(!MotorFlag.Direction)         //LIU调光运动状态判断
  {
    Motor.Motor_Run_Status = MOTOR_UP;       //上
  }
  else
  {
    Motor.Motor_Run_Status = MOTOR_DOWN;       //下 
  }
  MOTOR_WORK();//DRV8256E的nSLEEP引脚置1  
  MOTOR_PH_H();//DRV8256E的PH引脚置1，电机运行方向选择
  
  TB2CCTL1 = OUTMOD_2;//CCR1 toggle/reset
  TB2CCTL2 = OUTMOD_2;//CCR2 toggle/reset
  
  P5SEL0 |= BIT0;//DRV8256E的EN引脚输出PWM
  TB2CCR1 = duty;
   MotorFlag.Motor_Direction = 1;
}
//-------------------------------------------------------------------------------------
//  函数名:   MotorDown
//  功  能:   
//-------------------------------------------------------------------------------------
void MotorDown (u16 duty)
{
    if(MotorFlag.Direction)         //LIU调光运动状态判断
  {
    Motor.Motor_Run_Status = MOTOR_UP;       //上
  }
  else
  {
    Motor.Motor_Run_Status = MOTOR_DOWN;       //下 
  }
  MOTOR_WORK();//DRV8256E的nSLEEP引脚置1
  MOTOR_PH_L();//DRV8256E的PH引脚清0，电机运行方向选择
  
  TB2CCTL1 = OUTMOD_2;//CCR1 toggle/reset
  TB2CCTL2 = OUTMOD_2;//CCR2 toggle/reset
  
  P5SEL0 |= BIT0;//DRV8256E的EN引脚输出PWM
  TB2CCR1 = duty;
   MotorFlag.Motor_Direction = 0;
}
//-------------------------------------------------------------------------------------
//  函数名:   MotorStop
//  功  能:   
//-------------------------------------------------------------------------------------
void MotorStop (void)
{
  Motor.Motor_Run_Status = MOTOR_STOP;       //停止LIU
  
  MOTOR_SLEEP();
  
  Motor_Duty = 0;
  TB2CCR1 = 0; 
}
//-------------------------------------------------------------------------------------
//  函数名:   Duty_Add
//  功  能:   
//-------------------------------------------------------------------------------------
void Duty_Add (u16 *p,u8 i)
{
  *p += i;
  if (*p > MAX_DUTY)
  {
    *p = MAX_DUTY;
  }
}
//-------------------------------------------------------------------------------------
//  函数名:   Duty_Sub
//  功  能:   
//-------------------------------------------------------------------------------------
void Duty_Sub (u16 *p,u8 i)
{
  if (*p > i)
  {
    *p -= i;
  }
}
//-------------------------------------------------------------------------------------
//  函数名:   MotorControl
//  功  能:   
//  输  入:   空
//  输  出:   空
//  返  回:   空
//  描  述：  
//-------------------------------------------------------------------------------------
void MotorControl (void)
{
  #define OPRATION_OVERTIME 3000/16
  static u8 Time_Limit_Cnt;
  
  if ((!Flag.Del_Code_Finish) && (!Flag.Opretion_Finish))
  {//删码完成，抖动提醒完成才会执行这一段
    if (MotorFlag.Motor_Run)
    {
      motor_off_time = NULL;
      
      if  (motor_run_time < 6)
      {
        MOTOR_WORK();
        BRAKE_OFF();//跳通继电器
        MotorStop();
        Motor_Speed_Buff = 0;
      }
      //----------------------------------------------------------------------
      else if  (motor_run_time < 8)
      {
        Target_Speed = MIN_SPEED;
        Motor_Duty = MAX_DUTY >> 3;
      }
      //----------------------------------------------------------------------
      else
      {
        signed int En;//,En1;
        u8 flag;
        
        En = Target_Speed - Motor_Speed;//得到当前速度误差 E(n)
        //En1 = Target_Speed - Motor_Speed_Buff;//得到上次速度误差 E(n-1)
        //En += En1;    
        //将误差转换成正数
        if (En >= 0)//当前增量是否为正数
        {
          flag = 1;                
        }
        else//增量为负数
        {
          En = 0 - En;//转换成正数
          flag = 0;
        }
        
        if (Current_Current_mA < MAX_OUT_CURRENT)
        {
          if (flag == 0)//增量为负数
          {
              if (En >= 12)
            {
              Duty_Sub(&Motor_Duty,16);
            }
            else if (En >= 6)
            {
              Duty_Sub(&Motor_Duty,8);
            }
            else if (En >= 5)
            {
              Duty_Sub(&Motor_Duty,5);
            }
            else if (En >= 4)
            {
              Duty_Sub(&Motor_Duty,3);
            }
            else if (En >= 3)
            {
              Duty_Sub(&Motor_Duty,2);
            }
            else if (En >= 1)
            {
              Duty_Sub(&Motor_Duty,1);
            }
          }
          else//增量为正数
          {
             if (En >= 12)
            {
              Duty_Add(&Motor_Duty,16);
            }
            else if (En >= 6)
            {
              Duty_Add(&Motor_Duty,8);
            }
            else if (En >= 5)
            {
              Duty_Add(&Motor_Duty,5);
            }
            else if (En >= 4)
            {
              Duty_Add(&Motor_Duty,3);
            }
            else if (En >= 3)
            {
              Duty_Add(&Motor_Duty,2);
            }
            else if (En >= 1)
            {
              Duty_Add(&Motor_Duty,1);
            }
          }
        }
        else Duty_Sub(&Motor_Duty,5);        //-----------------------------------------------------------------------------
        //遇阻处理
        if (motor_run_time >= 20)
        {
          //遇阻或驱动芯片保护
          if (MeetPlugbDetect())
          {
            //立即停止
            MotorFlag.Motor_Run = NULL;
            MotorStop();
            BRAKE_ON();//刹车，断开继电器
            Motor.Motor_Status = MOTOR_STOP;
            //----------------------------------------------
            //驱动芯片保护则复位芯片
            if (!MOTOR_NFAULT())
            {
               //进入睡眠，再次操作电机运行时启动工作
               MOTOR_SLEEP();
            }
          }
        }
        else
        {
          Hall_Pulse_Amount_Buff = Hall_Pulse_Amount_Cnt;
          Meet_Plugb_Cnt = NULL;
          Meet_Plugb_Cnt1 = NULL;
          Meet_Plugb_Cnt2 = NULL;
          Meet_Plugb_Cnt3 = NULL;
          Flag.slowstop = NULL;
        }
        //--------------------------------------------------------------
        if (!Flag.slowstop)
        {
          if (Target_Speed < MAX_SPEED)
          {
            if (++speed_control_cnt >= 10)
            {
              Target_Speed ++;
              speed_control_cnt = NULL;
            }
          }
        }
        //--------------------------------------------------------------
        if (Flag.Motor_Run_Buffer ^ MotorFlag.Motor_Direction)
        {
          Flag.Motor_Run_Buffer = MotorFlag.Motor_Direction;
          Flag.Journey_Arrive = NULL;
        }
        //--------------------------------------------------------------
        if (Motor_Duty > MAX_DUTY)
        {
          Motor_Duty = MAX_DUTY;
        }
        //--------------------------------------------------------------
        if (Motor_Duty < MIN_DUTY)
        {
          Motor_Duty = MIN_DUTY;
        }
        //--------------------------------------------------------------
        if (MotorFlag.Motor_Run)
        {
          if (MotorFlag.Motor_Direction)
          {
            MotorUp(Motor_Duty);
          }
          else
          {
            MotorDown(Motor_Duty);
          }
        }
      }
      //----------------------------------------------------------------
      if (motor_run_time < 250)
      {
        motor_run_time++;
      }
      //----------------------------------------------------------------
      Motor_Speed_Buff = Motor_Speed;
    }
    //---------------------------------------------------------------------------------
    else//MotorFlag.Motor_Run等于0执行这一段
    {
      motor_run_time = NULL;
      MotorStop();
      BRAKE_ON();//刹车，断开继电器
      
      if (motor_off_time < 250)
      {
        motor_off_time++;
      }
    }
  }
    else//需要微动提示
  {//正在删码或者需要抖动提醒执行这一段
   //Flag.Del_Code_Finish等于1或者Flag.Opretion_Finish等于1执行这一段
    static u8 Jiggle_Direction;
    
    switch (Opretion_Event)
    {
      case 0:
        MotorFlag.Motor_Run = NULL;
        Motor.Motor_Status = MOTOR_STOP;
        MOTOR_WORK();
        
        Opretion_Event++;
        Flag.StatusSwitch_Delay = TRUE; 
        Motor_RunTime_Cnt = MOTOR_SWITCH_LATIME >> 1;
        
        Target_Speed = MAX_SPEED;
        Motor_Duty = MAX_DUTY - 100;
        Flag.Jiggle_Stop = NULL;
        
        Jiggle_Direction = 0;
        
         if (Limit_Point[0].Flag.F1.Limit_Activate)
          {
            if(Hall_Pulse_Amount_Cnt < (Limit_Point[0].Limit_Place + INCHING_ANGLE_TURNS+5))
            {
              Jiggle_Direction = MotorFlag.Journey_Direction;
            }
            else if ((Limit_Point_Amount > 1)&&(Hall_Pulse_Amount_Cnt > (Limit_Point[Limit_Point_Amount-1].Limit_Place-INCHING_ANGLE_TURNS-5)))
            {
              Jiggle_Direction = !MotorFlag.Journey_Direction;
            }
            else{
              Jiggle_Direction = MotorFlag.Motor_Direction;
            }
          }
          else
          {
            Jiggle_Direction = MotorFlag.Motor_Direction;
          }
      break;
      
      case 1://上
      case 5://上
        if (!Flag.StatusSwitch_Delay)
        {
          OprationCue_Amount_Buf = Hall_Pulse_Amount_Cnt;
          Flag.OprationCue_Run = TRUE;
          Flag.StatusSwitch_Delay = TRUE;//等待500MS
          Opretion_Event++;
          Time_Limit_Cnt = NULL;
          
          MotorStop();
          BRAKE_OFF();//跳通继电器，准备上微动
          Motor_Duty = MAX_DUTY - 100;
        }
      break;
    
      case 2:
      case 6:
        if (Flag.OprationCue_Run)
        {
          if (++Time_Limit_Cnt > OPRATION_OVERTIME)
          {
            Time_Limit_Cnt = NULL;
            Flag.OprationCue_Run = NULL;
            Flag.StatusSwitch_Delay = TRUE;//等待200MS
            Motor_RunTime_Cnt = MOTOR_SWITCH_LATIME >> 1;
          }
          else
          {
            if (Motor_Duty < MAX_DUTY)
            {
              Duty_Add(&Motor_Duty,5);
            }
            
            //上微动
              {
                if (Jiggle_Direction)
                {
                  MotorDown(Motor_Duty); // down
                }
                else
                {
                  MotorUp(Motor_Duty);  // UP
                }
              }
          }
        }
        else if (!Flag.StatusSwitch_Delay)
        {
          Opretion_Event++;
          Flag.StatusSwitch_Delay = TRUE;//上微动结束后等待200MS
          Motor_RunTime_Cnt = MOTOR_SWITCH_LATIME >> 1;
          
          MotorStop();//上微动后超时停止
          BRAKE_ON();//刹车，断开继电器
        }
      break;
    
      case 3://下
      case 7://下
        if (!Flag.StatusSwitch_Delay)
        {
          Opretion_Event++;
          Flag.OprationCue_Stop = TRUE;
          Time_Limit_Cnt = NULL;
          MotorStop();
          BRAKE_OFF();//跳通继电器，准备下微动
          Motor_Duty = MAX_DUTY - 100;
        }
      break;
    
      case 4: 
      case 8://OFF
        Time_Limit_Cnt++;	  
        if ((!Flag.OprationCue_Stop) || (Time_Limit_Cnt > OPRATION_OVERTIME))
        {
          //下微动结束或超时
          Time_Limit_Cnt = NULL;
          Flag.OprationCue_Run = NULL;
          if (Flag.Del_Code_Finish)
          {//抖动两次执行这一段，两个case语句连一起是为了抖动两次
            Opretion_Event++;
            Flag.StatusSwitch_Delay = TRUE;//删全码则等待0.5秒，准备下一次微动
          }
          else
          {
            Opretion_Event = 9;
          }
        }
        else
        {
          if (Motor_Duty < MAX_DUTY)
          {
            Duty_Add(&Motor_Duty,5);
          }
            
          //下微动
            {
              if (Jiggle_Direction)
              {
                MotorUp(Motor_Duty);  // UP
              }
              else
              {
                MotorDown(Motor_Duty); // down
              }
            }
        }
      break;
       
      default:
        Opretion_Event = NULL;
        Flag.Del_Code_Finish = NULL;
        Flag.Opretion_Finish = NULL;
        Motor.Motor_Status = MOTOR_STOP;
        motor_off_time = 0;//微动结束后，延迟2秒
        MotorStop();
        BRAKE_ON();//刹车，断开继电器
      break;
    }
  }
}
//-------------------------------------------------------------------------------------
//  函数名:   MotorStatusSwitch
//  功  能:   工作状态切换
//  输  入:   空
//  输  出:   空
//  返  回:   空
//  描  述：  
//-------------------------------------------------------------------------------------
void MotorStatusSwitch (void)
{
  if (!Flag.StatusSwitch_Delay)//是否在延迟500mS
  {
    if (Motor.Motor_Status == Motor.Motor_Status_Buffer)//状态是否没有改变
    {
      if (++Motor_RunTime_Cnt >= MOTOR_RUN_CNT_VALUE)//最长运行时间是否到
      {
        Motor.Motor_Status = MOTOR_STOP;
        Motor_RunTime_Cnt = NULL;
      }
    }
    else
    {
      Set_Limit_Point_Cnt = NULL;
      Joint_Level_ADJ_Time = 0;
      Flag.Motor_Status_Switch = TRUE;
      MotorStop();
      
      motor_run_time = NULL;
      motor_off_time = NULL;
      Motor_RunTime_Cnt = NULL;
      Motor_Speed = NULL;

      //Hall_Pulse_Jiggle_Buff = Hall_Pulse_Amount_Cnt;//保存当前位置
      
      
      if (3 == (Motor.Motor_Status_Buffer | Motor.Motor_Status))//是否上下切换
      {
        Flag.StatusSwitch_Delay = TRUE;//等待500MS
        Motor.Motor_Status_Buffer = MOTOR_STOP;//电机停止
      }
      else
      {
        Motor_RunTime_Cnt = MOTOR_RUN_TIME;
        Motor.Motor_Status_Buffer = Motor.Motor_Status;//更新档位
      }
    }
    //---------------------------------------------------------------------------------
    switch (Motor.Motor_Status_Buffer)//当前电机状态是什么
    {
      case MOTOR_UP://上
        MotorFlag.Motor_Run = TRUE;
        SET_UP_DIRECTION();
      break;
      case MOTOR_STOP://停
        MotorFlag.Motor_Run = NULL;
      break;
      case MOTOR_DOWN://下
        MotorFlag.Motor_Run = TRUE;
        SET_DOWN_DIRECTION();
        if(!Flag.Meet_Plugb_Rebound){
          Flag.Up_Meet_Plugb = NULL;//反弹遇阻标志清零解锁20250217ry
        }
      break;
    }
    //---------------------------------------------------------------------------------
    if (Flag.Joint_Action)//是否需要点动变连动
    {
      if (++Joint_Action_Time_Cnt > JOINT_ACTION_TIME)
      {
//        Joint_Action_Time_Cnt = 0;
//        Motor.Motor_Status = Joint_Action_Motor_Buf;
//        Flag.Joint_Action = NULL;
      }
    }
    else
    {
      Joint_Action_Time_Cnt = 0;
    }
    //---------------------------------------------------------------------------------
   
        if (Flag.Run_To_finishing_point)  // 是否要运行到终点
    {
      Flag.Run_To_finishing_point = NULL;
      if (!(MotorFlag.Motor_Direction ^ MotorFlag.Journey_Direction))
      {
        Target_Limit_Point = NULL; // 只检查第一个限位点
      }
      else
      {
        Target_Limit_Point = Limit_Point_Amount - 1; // 运行到最后一个限位点
      }
    }
    // 是否有切换状态，并有了限位点
    else if ((Flag.Motor_Status_Switch) && (Limit_Point[0].Flag.F1.Limit_Activate))
    {
      u8 x;
      
      Flag.Motor_Status_Switch = NULL;
      Flag.NO_Comper_Limit_Point = NULL;
      
      // 是否和第一个行程方向一样
      if (!(MotorFlag.Motor_Direction ^ MotorFlag.Journey_Direction))
      {
        Flag.NO_Comper_Limit_Point = NULL;
        if (Flag.Set_Limit_Point)  // 是否在设置限位状态
        {
          Target_Limit_Point = NULL; // 只检查第一个限位点
        }
        else
        {
          x = LoopCompare(Hall_Pulse_Amount_Cnt,LIMIT_PLACE);  /*当前是否在某个限位点*/
          if (x != NULL)                              
          {
            if (x > 1) // 是否当前在第一个限位以外
            {
              Target_Limit_Point = x - 2;  // 运行到当前限位点的下一个
            }
            else
            {
              Target_Limit_Point = NULL; // 当前在第一个限位点，则检查第一个限位
            }
          }
          else   // 当前不在限位点位置
          {
            // 从最后一个限位点开始检查
            for (x = MID_LIMIT_POINT_AMOUNT; x > 0; x--)
            {
              if (Limit_Point[x].Flag.F1.Limit_Activate)
              {
                // 当前位置是否在该限位点后面
                if (Hall_Pulse_Amount_Cnt > Limit_Point[x].Limit_Place)
                {
                  break;  //退出运行到该限位点
                }
              }
            }
            Target_Limit_Point = x;
          }
        }
      }
      else  // 往第一个限位点相反的方向走
      {
        if (Flag.Set_Limit_Point) // 是否在设置限位点
        {
          Flag.NO_Comper_Limit_Point = TRUE;  // 不检查限位
        }
        else
        {
          Flag.NO_Comper_Limit_Point = NULL;
          x = LoopCompare(Hall_Pulse_Amount_Cnt,LIMIT_PLACE);  /*当前是否在某个限位点*/
          if (x != NULL)                              
          {
            if (x >= Limit_Point_Amount) // 当前是否最后一个限位点上
            {
              if (Limit_Point_Amount == 1) // 是否只有一个限位点
              {
                Flag.NO_Comper_Limit_Point = TRUE;  // 不检查限位
              }
              else //不只一个限位点，当前停止最后的限位点上
              {
                Target_Limit_Point = x - 1;  // 保持检查最后一个限位点
              }
            }
            else // 不在最后一个限位点上
            {
              Target_Limit_Point = x; // 运行到下一个
            }
          }
          else  // 没有停在限位点上
          {
            // 从第2个限位点开始比较
            for (x = 1; x < (MID_LIMIT_POINT_AMOUNT + 2); x++)
            {
              if (Limit_Point[x].Flag.F1.Limit_Activate)
              {
                // 是否当前位置在该限位点前面
                if (Hall_Pulse_Amount_Cnt < Limit_Point[x].Limit_Place)
                {
                  break; // 退出运行到该限位点
                }
              }
              else // 后面没有限位点了
              {
                if (x > 1) // 不在第一个限位点范围
                {
                  x--;  // 保持检查当前限位点
                }
                else  // 只有一个限位点
                {
                  Flag.NO_Comper_Limit_Point = TRUE;  // 不检查限位
                }
                break;
              }
            }
            Target_Limit_Point = x;
          }
        }
      }
    }
  }
  else if (++Motor_RunTime_Cnt > MOTOR_SWITCH_LATIME)
  {
    Motor_RunTime_Cnt = NULL;
    Flag.StatusSwitch_Delay = NULL;
  }
}
