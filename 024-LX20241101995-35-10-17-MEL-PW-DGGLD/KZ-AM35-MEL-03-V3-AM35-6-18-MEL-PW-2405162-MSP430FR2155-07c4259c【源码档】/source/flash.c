/**********************************************************************************
 *  项目编号:
 *  项目名称:
 *  客户名称:                                    
 *  程序编写:
 *  启动日期:
 *  完成日期:    
 *  版本号码:    V1.0
 
 *  Initialize.C file
 *  功能描述: 系统初始化
 *********************************************************************************/
#include "Define.h"//全局宏定义
#include "Extern.h"//全局变量及函数

//---------------------------------------------------------------------------------
//  函数名:  CompID
//  功  能:   
//---------------------------------------------------------------------------------
u8 CompID (u8 data[])
{
  u8 i;
  u8 flag = 0;
  
  for (i = 0; i < RF_ID_Amount; i++)//扫描ID区域
  {
    //是否有相同的ID
    if (RF_ID_Buffer[i] [0] == data[1])                               
    {
      if (RF_ID_Buffer[i] [1] == data[2])
      {
        if (RF_ID_Buffer[i] [2] == data[3])
        {
          //保存当前ID所在的序号
          flag |= i + 1;
          // 两个通道中有1个字节中的1位同时为1时说明已经对好码
          if ((RF_ID_Buffer[i] [3] & data[4])|| 
              (RF_ID_Buffer[i] [4] & data[5]))//是否有相同的通道
          {
            flag |= 0x80;//有相同通道标志
            break;
          }
        }
      }
    }
  }
  return (flag);
}
//---------------------------------------------------------------------------------
//  函数名:   SEAVE_ID
//  功  能:   
//---------------------------------------------------------------------------------
u8 SaveID (u8 data[])
{
  u8 i;
  u8 flag = 0;
  //是否没有存满
  if (RF_ID_Amount <= ID_MAX_AMOUNT)
  {
    RF_ID_Amount++;
  }
  //保存
  for(i = 0; i < ID_LENGTH; i++)
  {
    RF_ID_Buffer[RF_ID_Save_Place] [i] = data[i+1];
  }
  flag = TRUE;
  //是否存满
  if (++RF_ID_Save_Place >= ID_MAX_AMOUNT)
  {
    RF_ID_Save_Place = 9;
  }
  return (flag);
}
//---------------------------------------------------------------------------------
//  函数名:  	SaveChannel
//  功  能:   
//---------------------------------------------------------------------------------
u8 SaveChannel (u8 array[],u8 ch_addr)
{
  u8 flag = 0;
  
  ch_addr &= 0x1f;
  ch_addr--;
  
  if (ch_addr < RF_ID_Amount)
  {
    //添加通道
    RF_ID_Buffer[ch_addr] [3] |= array[4];
    RF_ID_Buffer[ch_addr] [4] |= array[5];
    flag = TRUE;
  }
  return (flag);
}
//---------------------------------------------------------------------------------
//  函数名:  	DeleteSingleChannel
//  功  能:   
//---------------------------------------------------------------------------------
u8 DeleteChannel (u8 array[],u8 ch_addr)
{
  u8 i;
  u8 j;
  u8 flag = 0;
  
  ch_addr &= 0x1f;
  ch_addr--;
  
  if (ch_addr < RF_ID_Amount)
  {
    // 删除通道
    RF_ID_Buffer[ch_addr] [3] &= ~array[4];
    RF_ID_Buffer[ch_addr] [4] &= ~array[5];
    // 通道是否等于0
    if ((RF_ID_Buffer[ch_addr] [3] == 0) &&  (RF_ID_Buffer[ch_addr] [4] == 0))
    {
      // 关看门狗
      WDTCTL = WDTPW + WDTHOLD;
    
      // 删除ID,后面的往前挪
      for(i = ch_addr; i < RF_ID_Amount; i++)
      {
        for(j = 0; j < ID_LENGTH; j++)
        {
          RF_ID_Buffer[i] [j] = RF_ID_Buffer[i+1] [j];
        }
      }
      //ID数量减一
      if (RF_ID_Save_Place > 0)
      {
        RF_ID_Save_Place--;
      }
      //ID数量减一
      if (RF_ID_Amount > 0)
      {
        RF_ID_Amount--;
      }
    }
    flag = TRUE;
  }
  return (flag);
}


void JourneyDelete(void)
{
   static u8 LIMIT_i;
    for (LIMIT_i = 0; LIMIT_i < (MID_LIMIT_POINT_AMOUNT + 2); LIMIT_i++)
    {
      Limit_Point[LIMIT_i].Limit_Place = 0;
      Limit_Point[LIMIT_i].Flag.F8 = 0;

    }
    Limit_Point_Amount     = 0;
     Flag.Set_Journey_Direction = FALSE; 
    Hall_Pulse_Amount_Cnt  = MIN_JOURNEY_VALUE;
    Restrict_Up = 3;                                              //删除限位
}


//---------------------------------------------------------------------------------
//  函数名:  	DeleteAllID
//  功  能:   
//---------------------------------------------------------------------------------
void DeleteAllID (u8 check)
{
  static u8 i;
  static u8 j;
  
  if (check == 0xee)
  {
    _DINT();//关总中断 
    WDTCTL = WDTPW + WDTHOLD;//关看门狗
    //---------------------------------------------------------------------------------
//    for (i = 0; i < (MID_LIMIT_POINT_AMOUNT + 2); i++)
//    {
//      Limit_Point[i].Limit_Place = 0;
//      Limit_Point[i].Flag.F8 = 0;
//
//    }
//    //---------------------------------------------------------------------------------
    RF_ID_Save_Place = 0;
    RF_ID_Amount=0;
//    Restrict_Up = 3;                                              //删除限位
//
//    //---------------------------------------------------------------------------------
    

    
    JourneyDelete();
    //删除所有ID
    for(i = 0; i < ID_MAX_AMOUNT; i++)
    {
      for(j = 0; j < ID_LENGTH; j++)
      {
        RF_ID_Buffer[i] [j] = 0;
      }
    }
    //---------------------------------------------------------------------------------
    Flag.Learn_Code_Statu = NULL;
    Opretion_Event = NULL;
    Motor.Motor_Status = MOTOR_STOP;
    Flag.Opretion_Finish = NULL;
    Flag.Del_Code_Finish = TRUE;
    Flag.Joint_Level_Adj = 0;    
    Flag.Set_Limit_Point = 0;
    Flag.Delete_Code=0;//删除全码完成，清零删除标志位
    MotorFlag.Motor_Jiggle_Status=0;//电机点动运行标志
    MotorFlag.Direction=0;//方向标志
    Joint_Time_Level=4;//点动角度
    Sweet_Spot = 0;
    /*******百叶帘初始化******/   
    if(Flag.Angle_mark == 1)
    {
      Hall_Pulse_Amount_up = (Hall_difference + Hall_Pulse_Amount_Cnt);//
    }     
    else //以180度为起点
    {
      Hall_Pulse_Amount_up = (Hall_Pulse_Amount_Cnt - Hall_difference );//
      
    }
    Send_Data(1,0x06,10,2);
    //Hall_Pulse_Amount_up = Hall_Pulse_Amount_Cnt ;//存储新的一次0度时的位置
    Flag.Control_Tighten_Up = NULL ;//默认打开收紧功能
    Flag.Up_Meet_Plugb = NULL;//反弹遇阻标志清零解锁20250217ry
    _EINT();//开总中断 
  }
}