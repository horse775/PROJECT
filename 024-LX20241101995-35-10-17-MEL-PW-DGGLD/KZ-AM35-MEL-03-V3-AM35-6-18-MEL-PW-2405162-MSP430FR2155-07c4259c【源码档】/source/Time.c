/**********************************************************************************
 *  项目编号:
 *  项目名称:  
 *  客户名称:                                    
 *  程序编写:   
 *  启动日期:    
 *  完成日期:    
 *  版本号码:    V1.0
 
 *  Time.C file
 *  功能描述: 
**********************************************************************************/
#include "Define.h"                                          /* 全局宏定义 ------*/
#include "Extern.h"                                          /* 全局变量及函数 --*/

/**********************************************************************************
  函数名:  Timer3_B1
  功能:    Timer3_B1中断
**********************************************************************************/
#pragma vector=TIMER3_B1_VECTOR
__interrupt void Timer3_B1 (void)
{
  if (TB3IV == 2)
  {
    TB3CCR1 += SYSTEM_TIME_VALUE * SYSTEM_CLOCK;     // 2mS
    Flag.Time_2ms = TRUE;
  }
}
/**********************************************************************************
  函数名:   TimeControl
  功  能:   
 *********************************************************************************/
void TimeControl (void)
{
    if (--Time_10ms_Cnt == NULL)//是否到10MS 
    {
      Time_10ms_Cnt = TIME_10MS_VALUE;
      Flag.Time_10ms = TRUE;//置10MS标志 
      if(rx_tc)
      { rx_tc--;
        if(rx_tc==0);
      }
      
      
    }
    if (--Time_50ms_Cnt == NULL)//是否到50MS 
    {
      Time_50ms_Cnt = TIME_50MS_VALUE;
      Flag.Time_50ms = TRUE;//置50MS标志 
    }
    if (--Time_100ms_Cnt == NULL)//是否到100MS
    {
      Time_100ms_Cnt = TIME_100MS_VALUE;
      Flag.Time_100ms = TRUE;//置100MS标志
    }
    if (--Time_500ms_Cnt == NULL)//是否到500MS
    {
      Time_500ms_Cnt = TIME_500MS_VALUE;
      Flag.Time_500ms = TRUE;//置500MS标志
      if (--Time_1s_Cnt == NULL)//是否到1S   
      {
        Time_1s_Cnt = TIME_1S_VALUE;
        Flag.Time_1s = TRUE;//置1S标志   
      }
    }
}
//-------------------------------------------------------------------------------------
/**********************************************************************************
  函数名:   LearnDelCodeTime
  功  能:   
  输  入:   空
  输  出:   空
  返  回:   空
  描  述：  
 *********************************************************************************/
void LearnDelCodeTime (void)
{
  #define LEARN_CODE_TIME             10
  #define DELETE_CODE_TIME            10
  #define SET_LIMIT_POINT_TIME        30
  #define DELETE_SINGLE_CODE_TIME     10
  #define JOINT_ADJ_TIME              40
  
  static u8 Learn_Time_Cnt;
  
  if (Flag.Learn_Code_Statu)
  {
    if (++Learn_Time_Cnt >= LEARN_CODE_TIME)
    {
      Flag.Learn_Code_Statu = NULL;
      Learn_Time_Cnt = NULL;
    }
  }
  else
  {
    Learn_Time_Cnt = NULL;
  }
  
  if (Flag.Delete_single_Code)
  {
    if (++Delete_single_Code_Cnt >= DELETE_SINGLE_CODE_TIME)
    {
      Flag.Delete_single_Code = NULL;
      Delete_single_Code_Cnt = NULL;
    }
  }
  else
  {
    Delete_single_Code_Cnt = NULL;
  }
  
  if ((Flag.Set_Limit_Point) && (Motor.Motor_Status == MOTOR_STOP))
  {
    if (++Set_Limit_Point_Cnt >= SET_LIMIT_POINT_TIME)
    {
      u8 i;
      
      Set_Limit_Point_Cnt = NULL;
      Flag.Set_Limit_Point = NULL;
      Flag.Opretion_Finish = TRUE;
      Opretion_Event = NULL;
      
      for (i = 0; i < (MID_LIMIT_POINT_AMOUNT + 2); i++)
      {
        Limit_Point[i].Flag.F1.Fine_Adjust = NULL;
      }
    }
  }
  else
  {
    Set_Limit_Point_Cnt = NULL;
  }
  
  
  if ((Flag.Joint_Level_Adj)  && (Motor.Motor_Status == MOTOR_STOP)&&(!Flag.Del_Code_Finish)&&(!Flag.Opretion_Finish))
  {
    if (++Joint_Level_ADJ_Time >= JOINT_ADJ_TIME)
    {
      Joint_Level_ADJ_Time = 0;
      Flag.Joint_Level_Adj = NULL;    
      Flag.Opretion_Finish = TRUE;

    }
  }
  else
  {
    Joint_Level_ADJ_Time = 0;
  }
  
  if(learn_key_cnt != 0)
  {
    if(++No_learn_key_cnt>=2)
    {
      learn_key_cnt = 0;
    }
  }
  
    if(Flag.Power_on)
  {
    if(++Time_Power_On >=15)//上电10s内
    {
        Time_Power_On = 0;
        learn_key_cnt = 0;
        Flag.Power_on = 0;
    }
  }
}
//--------------------------------------------------------------------------------------
//  函数名:   DblclickTimeLimit
//  功  能:   
//  输  入:   空
//  输  出:   空
//  返  回:   空
//  描  述：  
//--------------------------------------------------------------------------------------
void DblclickTimeLimit (void)
{
  #define RF_DBLCLICK_TIME      1500 / 100
  
  if ((Flag.RF_UP_dblclick) || (Flag.RF_DOWN_dblclick))
  {
    if (++RF_Dblclick_Time >= RF_DBLCLICK_TIME)
    {
      RF_Dblclick_Time = NULL;
      Flag.RF_UP_dblclick = NULL;
      Flag.RF_DOWN_dblclick = NULL;
    }
  }
  else
  {
    RF_Dblclick_Time = NULL;
  }
}