/**********************************************************************************
*  项目编号:    
*  项目名称:    
*  客户名称:                                    
*  程序编写:    
*  启动日期: 
*  完成日期:    
*  版本号码:    V1.0
*
*  Signal_Sampling.c file
*  功能描述: 信号采样
**********************************************************************************/
#include "Define.h"//全局宏定义
#include "Extern.h"//全局变量及函数
//内部宏--------------------------------------------------------------------------------
#define RF_MAX_BYTE_VALUE      8//接收事件最大值
#define DATA_BIT_AMOUNT        8//数据BIT数量
#define RF_NO_RX_TIME          200/50

//信号头最大值50us*3*106=16MS
#define HEAD_LONG_MAX          ((6500 + ((RF_DECODE_TIME) / 2)) / (RF_DECODE_TIME))
#define HEAD_LONG_MIN          ((3500 + ((RF_DECODE_TIME) / 2)) / (RF_DECODE_TIME))
#define HEAD_SHORT_MAX         ((1000  + ((RF_DECODE_TIME) / 2)) / (RF_DECODE_TIME))
#define HEAD_SHORT_MIN         ((300  + ((RF_DECODE_TIME) / 2)) / (RF_DECODE_TIME))

#define SIGNAL_BIT_MAX         ((1300 + ((RF_DECODE_TIME) / 2)) / (RF_DECODE_TIME))
#define SIGNAL_BIT_MIN         ((600  + ((RF_DECODE_TIME) / 2)) / (RF_DECODE_TIME))

//内部变量------------------------------------------------------------------------------
u8 RF_Data_OverTime_Cnt;//时间溢出计数器
u8 RF_Bit_cnt;//Bit计数器
u8 RF_Byte_Cnt;//Byte计数器
u8 RF_KeyUnloosen_Cnt;//松键计数器
u8 RF_RxData[RF_MAX_BYTE_VALUE];//RF接收数据缓存
u8 RF_RxData_Buf[RF_MAX_BYTE_VALUE];//RF接收数据缓存

u8 * Rx_Data_P;//RF数据接收指针
u8 RF_NO_Rx_Cnt;

u8 Access_Learn_Cnt;           //上电对码

//内部函数------------------------------------------------------------------------------
u8 RFDataVerify (void);
void ErrorDisposal (void);
void RFDataDisposal (void);



/**********************************************************************************
函数名:   
功  能:   进入产测
输  入:   空
输  出:   空    
返  回:	空
描  述：  
**********************************************************************************/
void Enter_test(u8 *RF_data,u8 exe_conditon)
{
  if(exe_conditon)//确定执行
  {          
    
    {
      if(0x53 == RF_data[6])//背面键
      {
        Time_Power_On = 0;             
        No_learn_key_cnt = 0;
        if(++learn_key_cnt >=10)//按键三次
        {
          learn_key_cnt = 0;
          Flag.statTest = 0;
          Time_Power_On = 0;
          Flag.WifiTest = 1;//开始测试wifi
          tx_c = 1;
          flag_tx = 1;
          tx_data = 0x0e;
          Flag.Uart_tx = 1;
        }
      }
    }
  }
}
//---------------------------------------------------------------------------------
//  函数名:   
//  功  能:   
//  输  入:   空
//  输  出:   空    
//  返  回:   空
//  描  述：
//---------------------------------------------------------------------------------
void RFStatusInspect (void)
{
  if (Flag.RF_Rx_Finish)
  {
    Flag.RF_Rx_Finish = NULL;
    RF_NO_Rx_Cnt = NULL;
  }
  else
  {
    if (++RF_NO_Rx_Cnt > RF_NO_RX_TIME)
    {
      RF_NO_Rx_Cnt = NULL;
      Flag.RF_Data_Disposal = NULL;
    }
  }
}
//---------------------------------------------------------------------------------
//  函数名:  	ErrorDisposal
//  功  能:   
//---------------------------------------------------------------------------------
void ErrorDisposal (void)
{
  RF_Bit_cnt = NULL;//清BIT计数器
  RF_Byte_Cnt = NULL;
  Flag_RF.Signal_Head = NULL;//RF信号头标志
}
//---------------------------------------------------------------------------------
//  函数名:  	LimitLLC
//  功  能:       
//---------------------------------------------------------------------------------
void LimitLLC (Limit array[],u8 place)
{
  array[place].Flag.F1.Fine_Adjust = 0;
  array[place].Flag.F1.Limit_Activate = 0;
  array[place].Limit_Place = 0; 
  
  for (; place < (MID_LIMIT_POINT_AMOUNT + 2); place++)//如果是最后一个则不再前移
  {
    if (place == (MID_LIMIT_POINT_AMOUNT + 1))
    {
      //最后一个已经移到前面了，直接删除退出
      Limit_Point[place].Flag.F1.Fine_Adjust = 0;
      Limit_Point[place].Flag.F1.Limit_Activate = 0;
      Limit_Point[place].Limit_Place = 0; 
      break;
    }
    else if (array[place + 1].Flag.F1.Limit_Activate)//后面限位点是否设置了
    {
      //后面有限位则将后面的往前移
      array[place].Flag.F1.Fine_Adjust = array[place + 1].Flag.F1.Fine_Adjust; 
      array[place].Flag.F1.Limit_Activate = array[place + 1].Flag.F1.Limit_Activate;
      array[place].Limit_Place = Limit_Point[place + 1].Limit_Place;
    }
    else
    {
      //后面没有限位点则把最后的清除
      //因为最后的已经移到前面去了
      Limit_Point[place].Flag.F1.Fine_Adjust = 0;
      Limit_Point[place].Flag.F1.Limit_Activate = 0;
      Limit_Point[place].Limit_Place = 0; 
    }
  }
}
//---------------------------------------------------------------------------------
//  函数名:   LimitCollator
//  功  能:   限位点排序
//  输  入:   空
//  输  出:   空    
//  返  回:   数据是否正确标志
//  描  述：  
//---------------------------------------------------------------------------------
u8 LimitCollator (Limit array[])
{
  u8 i;
  u8 j;
  u8 flag = 1;
  u32 mid;
  
  //挑出错误的限位点
  j = 0;
  for (i = 0; i < (MID_LIMIT_POINT_AMOUNT + 2); i++)
  {
    if (!array[j].Flag.F1.Limit_Activate)//是否没有设置限位点
    {
      LimitLLC (array,j);
    }
    else//设置了限位
    {
      //是否超出位置
      if ((array[j].Limit_Place > HALL_PULSE_MAX) || (array[j].Limit_Place < HALL_PULSE_MIN))
      {
        LimitLLC (array,j);
      }
      else
      {
        j++;
      }
    }
  }
  
  //将限位点排序，小的在前面
  for (i = 1; ((i <= (MID_LIMIT_POINT_AMOUNT + 2)) && (flag)); i++)//注意判定条件 小于限位点的最大数量，并有交换位置
  {
    flag = 0;//每次外圈时初始化
    for (j = 0; (j < (MID_LIMIT_POINT_AMOUNT + 2) - i); j++)//是否小于最大限位点数量 - i
    {
      if ((array[j].Flag.F1.Limit_Activate) && (array[j + 1].Flag.F1.Limit_Activate))
      {
        if (array[j].Limit_Place > array[j+1].Limit_Place)
        {
          mid = array[j].Limit_Place;//将大的移到缓存
          array[j].Limit_Place = array[j+1].Limit_Place;//将小的上移
          array[j+1].Limit_Place = mid;//将大的下移
          flag = 1;//如果有交换，那么可以进入下一圈循环
        }
      }
    }
  }
  
  //数限位点的数量
  flag = 0;
  for (i = 0; i < (MID_LIMIT_POINT_AMOUNT + 2); i++)
  {
    if (Limit_Point[i].Flag.F1.Limit_Activate)//是否设置限位点
    {
      flag++;
    }
  }
  
  return (flag);
}
//---------------------------------------------------------------------------------
//  函数名:  RFDataVerify
//  功  能:  效验收到的数据是否正确
//  输  入:  空
//  输  出:  空
//  返  回:  数据是否正确标志
//  描  述：  
//---------------------------------------------------------------------------------
u8 RFDataVerify (void)
{
  u8 flag = NULL;
  u8 i;
  
  if (RF_RxData_Buf[0] == RF_DATA_HEAD)//头码是否正确
  {
    i = RF_RxData_Buf[1] + RF_RxData_Buf[2] + RF_RxData_Buf[3] 
      + RF_RxData_Buf[4] + RF_RxData_Buf[5] + RF_RxData_Buf[6];
    
    if (RF_RxData_Buf[7] == i)//校验和是否相等
    {
      flag = TRUE;   
      for (i = 0; i < RF_MAX_BYTE_VALUE; i++)
      {
        RF_RxData[i] = RF_RxData_Buf[i];
      }
    }
  }
  
  return (flag);
}
//---------------------------------------------------------------------------------
//  函数名:   RFDataDisposal
//  功  能:   RF数据处理
//  输  入:   空
//  输  出:   空    
//  返  回:   空
//  描  述：  
//---------------------------------------------------------------------------------
void RFDataDisposal (void)
{  
  u8 ch_addr; 
  //----------------------------------上电对码-------------------------------------------------
  if (!Flag.Electrify_Reset)
  {
    if (RF_RxData[6] == RF_ACCESS_LEARN) //  进入学码
    {
      if (++Access_Learn_Cnt >= 5)
      {
        Access_Learn_Cnt = 0;
        Flag.Electrify_Reset = TRUE; 
        Flag.Learn_Code_Statu = TRUE;                          // 进入学习状态
        Flag.Opretion_Finish = TRUE;
        Delete_single_Code_ID[1] = RF_RxData[1];
        Delete_single_Code_ID[2] = RF_RxData[2];
        Delete_single_Code_ID[3] = RF_RxData[3];
        Flag.Power_Up_Learn = TRUE;
      }
    }
  }
  
  //-----------------------------------------------------------------------------------
  if ((!Flag.RF_Data_Disposal) && (Flag.Delete_single_Code))
  {
    if ((Delete_single_Code_ID[1] == RF_RxData[1]) &&
        (Delete_single_Code_ID[2] == RF_RxData[2]) &&
          (Delete_single_Code_ID[3] == RF_RxData[3]))
    {
      if (RF_RxData[6] == RF_DATA_CLEAR)//删全码
      {
        Flag.Delete_Code = TRUE;
        Flag.Delete_single_Code = NULL;
      }
      else if (RF_RxData[6] == RF_DATA_LEARN)
      {
        Delete_single_Code_Cnt = NULL;//再次按学码键的话从新计时，而不会退出。
      }
    }
  }
  //-----------------------------------------------------------------------------------
  if ((!Flag.Set_Key_ON) && (!Flag.Opretion_Finish) && 
      (!Flag.Del_Code_Finish)&& (Flag.Electrify_Reset))
  {
    ch_addr = CompID(RF_RxData);//比较ID
    //是否有相同的ID和通道，并且不在删码状态
    
    Enter_test(RF_RxData,Flag.Power_on);           //产测 
    
    if (ch_addr & 0x80)
    {
      u8 i;
      
      Flag.RF_Rx_Finish = TRUE;
      
      //是在学习状态
      if (Flag.Learn_Code_Statu)
      {
        switch (RF_RxData[6])//是什么命令
        {
        case RF_DATA_UP://上行 学码（已有ID，学通道）
          //SaveChannel(RF_RxData,ch_addr);
          Flag.Learn_Code_Statu = NULL;
          Flag.Opretion_Finish = TRUE;
          Opretion_Event = NULL;
          break;
        case RF_DATA_STOP://停止 点动-连动切换
          MotorFlag.Motor_Jiggle_Status = !MotorFlag.Motor_Jiggle_Status;
          Send_Data(1,0x06,0,7);
          Flag.Opretion_Finish = TRUE;//启动抖动提醒
          Opretion_Event = NULL;
          
          Flag.Learn_Code_Statu = NULL;
          break;
        case RF_DATA_DOWN://下行 换向
          MotorFlag.Direction = !MotorFlag.Direction;
          Send_Data(1,0x06,0,5);
          // JourneyDelete();               //删除限位
          Flag.Opretion_Finish = TRUE;//启动抖动提醒
          Opretion_Event = NULL;
          
          Flag.Learn_Code_Statu = NULL;
          break;
        case RF_DATA_LEARN://学码  删单码
          Flag.Delete_single_Code = TRUE;
          Flag.RF_Data_Disposal = TRUE;
          
          Flag.Opretion_Finish = TRUE;//启动抖动提醒
          Opretion_Event = NULL;
          
          Delete_single_Code_ID[0] = ch_addr;
          Delete_single_Code_ID[1] = RF_RxData[1];
          Delete_single_Code_ID[2] = RF_RxData[2];
          Delete_single_Code_ID[3] = RF_RxData[3];
          Delete_single_Code_ID[4] = RF_RxData[4];
          Delete_single_Code_ID[5] = RF_RxData[5];
          Flag.Learn_Code_Statu = NULL;
          
          DeleteChannel(Delete_single_Code_ID,Delete_single_Code_ID[0]);
          break;
        }
      }
      else//不在学习状态
      {
        switch (RF_RxData[6])//是什么命令
        {
        case RF_DATA_UP://上行
          if (!Flag.RF_Data_Disposal)
          {
            // 进入限位点设置状态后，先按上行则为设定上行程
            if (Flag.Set_Journey_Direction)
            {
              Flag.Set_Journey_Direction = NULL;
              MotorFlag.Journey_Direction = !MotorFlag.Direction;//确定行程方向
              Restrict_Up = 1;                                              //先设置上限位
              
            }
            //----------------------------------------------------------------------
            if ((MotorFlag.Motor_Jiggle_Status) || (Flag.Set_Limit_Point) ||
                Flag.Joint_Level_Adj || (!Limit_Point[1].Flag.F1.Limit_Activate))                  // 默认为点动模式
            { if (Motor.Motor_Status != MOTOR_UP)
            {
              Motor.Motor_Status = MOTOR_UP;
            }
            else{
              Motor.Motor_Status = MOTOR_STOP;
            }
            }
            else{
              Motor.Motor_Status = MOTOR_UP;
            }   
            Flag.Jiggle_Stop = NULL;
            Flag.Goto_PCT = FALSE;//遥控电机运行时，关闭百分比控制
            Flag.Angle_Stop = NULL ;//遥控关调光LIU 
            //----------------------------------------------------------------------
            
            
            {
              if (Limit_Point[2].Flag.F1.Limit_Activate)//是否已有多个限位点
              {
                if (!Flag.RF_Data_Disposal)
                {
                  Flag.RF_DOWN_dblclick = NULL;
                  Flag.RF_Data_Disposal = TRUE;
                  if (Flag.RF_UP_dblclick)
                  {
                    Flag.RF_UP_dblclick = NULL;          
                    if(MotorFlag.Motor_Jiggle_Status)
                    {
                      
                    }
                    else
                    {
                      Flag.Run_To_finishing_point = TRUE;
                    } 
                  }
                  else
                  {
                    Flag.RF_UP_dblclick = TRUE;
                    RF_Dblclick_Time = TRUE;
                  }
                }
              }
              
#ifdef INCHING_FIXED_ANGLE
              if ((MotorFlag.Motor_Jiggle_Status) || (Flag.Set_Limit_Point) ||
                  Flag.Joint_Level_Adj || (!Limit_Point[1].Flag.F1.Limit_Activate))//默认为点动模式
              {
                Flag.Jiggle_Stop = TRUE;
                Flag.RF_Data_Disposal = TRUE;
                Hall_Pulse_Jiggle_Buff = Hall_Pulse_Amount_Cnt;//保存当前位置
                
                Joint_Action_Motor_Buf = Motor.Motor_Status;
                Flag.Joint_Action = TRUE;
                Joint_Action_Time_Cnt = 0;
              }
#endif
            }
            if (SendTX_interval_Time++>=8)
            {
              SendTX_interval_Time =0;                   //延时发送LIU
              
              Send_Data(3,0x6A,0,0);
              Motor_Data_Bak.Motor_Status_1 = Motor.Motor_Status;
            }
          }
          break;
          
        case RF_CONSECUTIVE_UP://上行
          Motor.Motor_Status = MOTOR_UP;
          Flag.Jiggle_Stop = NULL;
          Flag.RF_Data_Disposal = NULL;
          
          if (SendTX_interval_Time++>=8)
          {
            SendTX_interval_Time =0;                   //延时发送LIU
            Send_Data(3,0x6A,0,0);
            Motor_Data_Bak.Motor_Status_1 = Motor.Motor_Status;
          }
          break;
          
        case RF_DATA_STOP://停止
          Motor.Motor_Status = MOTOR_STOP;
          
          Flag.Goto_PCT = FALSE;//遥控电机运行时，关闭百分比控制
          Flag.Angle_Stop = NULL ;//遥控关调光LIU 
          Joint_Action_Time_Cnt = 0;
          Joint_Action_Motor_Buf = MOTOR_STOP;
          Flag.Joint_Action = NULL;
          break;
        case RF_DATA_DOWN://下行
          if (!Flag.RF_Data_Disposal)
          {
            //进入限位点设置状态后，先按下行则为设定下行程
            if (Flag.Set_Journey_Direction)
            {
              Flag.Set_Journey_Direction = NULL;
              MotorFlag.Journey_Direction = MotorFlag.Direction;//确定行程方向
              
              Restrict_Up = 2;                                              //先设置上限位
              
            }
            //----------------------------------------------------------------------
            if ((MotorFlag.Motor_Jiggle_Status) || (Flag.Set_Limit_Point)|| 
                Flag.Joint_Level_Adj || (!Limit_Point[1].Flag.F1.Limit_Activate))                  // 默认为点动模式
            { if (Motor.Motor_Status != MOTOR_DOWN)
            {
              Motor.Motor_Status = MOTOR_DOWN;
            }
            else{
              Motor.Motor_Status = MOTOR_STOP;
            }
            }
            else{
              Motor.Motor_Status = MOTOR_DOWN;
            }             Flag.Jiggle_Stop = NULL;
            Flag.Goto_PCT = FALSE;//遥控电机运行时，关闭百分比控制
            Flag.Angle_Stop = NULL ;//遥控关调光LIU 
            //----------------------------------------------------------------------
            
            {
              if (Limit_Point[2].Flag.F1.Limit_Activate)//是否已有多个限位点
              {
                if (!Flag.RF_Data_Disposal)
                {
                  Flag.RF_UP_dblclick = NULL;
                  Flag.RF_Data_Disposal = TRUE;
                  if (Flag.RF_DOWN_dblclick)
                  {
                    Flag.RF_DOWN_dblclick = NULL;     
                    if(MotorFlag.Motor_Jiggle_Status)
                    {
                      
                    }
                    else{
                      Flag.Run_To_finishing_point = TRUE;
                    }
                  }
                  else
                  {
                    Flag.RF_DOWN_dblclick = TRUE;
                    RF_Dblclick_Time = TRUE;
                  }
                }
              }
              //-----------------------------------------------------------------
#ifdef INCHING_FIXED_ANGLE
              if ((MotorFlag.Motor_Jiggle_Status) || (Flag.Set_Limit_Point) ||
                  Flag.Joint_Level_Adj || (!Limit_Point[1].Flag.F1.Limit_Activate))//默认为点动模式
              {
                Flag.Jiggle_Stop = TRUE;
                Flag.RF_Data_Disposal = TRUE;
                Hall_Pulse_Jiggle_Buff = Hall_Pulse_Amount_Cnt;//保存当前位置
                
                Joint_Action_Motor_Buf = Motor.Motor_Status;
                Flag.Joint_Action = TRUE;
                Joint_Action_Time_Cnt = 0;
              }
#endif
            }
            if (SendTX_interval_Time++>=8)
            {
              SendTX_interval_Time =0;                   //延时发送LIU
              Send_Data(3,0x6A,0,0);
              Motor_Data_Bak.Motor_Status_1 = Motor.Motor_Status;
            }
          }
          break;
        case RF_CONSECUTIVE_DOWN://下行
          Motor.Motor_Status = MOTOR_DOWN;
          Flag.Jiggle_Stop = NULL;
          Flag.RF_Data_Disposal = NULL;
          if (SendTX_interval_Time++>=8)
          {    
            SendTX_interval_Time =0;                   //延时发送LIU
            
            Send_Data(3,0x6A,0,0);
            Motor_Data_Bak.Motor_Status_1 = Motor.Motor_Status;
          }
          break;
        case RF_JIGGLE_STOP://点动停止
#ifndef INCHING_FIXED_ANGLE
          if ((MotorFlag.Motor_Jiggle_Status) || (Flag.Set_Limit_Point) ||
              (!Limit_Point[1].Flag.F1.Limit_Activate))//默认为点动模式
          {
            Flag.Jiggle_Stop = TRUE;
          }
#endif
          Joint_Action_Time_Cnt = 0;
          Joint_Action_Motor_Buf = MOTOR_STOP;
          Flag.Joint_Action = NULL;
          Flag.RF_Data_Disposal = NULL;
          break;
          
        case RF_DATA_AWOKEN://唤醒
          if ((!Flag.Set_Limit_Point) && (!Flag.Learn_Code_Statu))
          {
            Motor.Motor_Status = MOTOR_STOP;
            Motor.Motor_Status_Buffer = MOTOR_STOP;
            MotorFlag.Motor_Run = NULL;
            Flag.Learn_Code_Statu = TRUE;//进入学习状态
            Flag.Opretion_Finish = TRUE;
            Opretion_Event = NULL;
          }
          break;
        case RF_DATA_LEARN://进入限位点设置状态
          if (!Flag.RF_Data_Disposal)
          {
            Flag.RF_Data_Disposal = TRUE;
            Motor.Motor_Status = MOTOR_STOP;
            MotorFlag.Motor_Run = NULL;
            
            if (Flag.Set_Limit_Point)//是在设置限位状态
            {//保存限位点
              Flag.Opretion_Finish = TRUE;//启动抖动提醒
              Opretion_Event = NULL;
              Flag.Save_Limit_Point = TRUE;
              Flag.Time_50ms = NULL;
              Time_50ms_Cnt = TIME_50MS_VALUE;
              Flag.Set_Journey_Direction = NULL;//清设定行程方向标志
            }
            else
            {
              if (!Limit_Point[0].Flag.F1.Limit_Activate)//是否尚未设定限位点
              {
                Flag.Set_Journey_Direction = TRUE;//置设定行程方向标志
                MotorFlag.Journey_Direction = !MotorFlag.Direction;//默认设置上限位
                Flag.Set_Limit_Point = TRUE;//进入设置限位状态
                Flag.Opretion_Finish = TRUE;//启动抖动提醒
                Opretion_Event = NULL;
              }
              else//已经有限位点
              {
                //比较当前位置是否与某个限位点相等
                i = LoopCompare(Hall_Pulse_Amount_Cnt,LIMIT_PLACE);
                if (i != NULL)//是否要微调限位点
                {
                  Limit_Point[--i].Flag.F1.Fine_Adjust = TRUE;//置微调限位点标志
                  Flag.Set_Limit_Point = TRUE;//进入设置限位状态
                  
                  Flag.Opretion_Finish = TRUE;//启动抖动提醒
                  Opretion_Event = NULL;
                }
                else
                {
                  //是否尚未设置完6个限位点
                  if (Limit_Point_Amount < (MID_LIMIT_POINT_AMOUNT + 2))//是否限位点尚未设置完
                  {
                    //是否离某个限位点太近
                    if (!(LoopCompare(Hall_Pulse_Amount_Cnt,LIMIT_SET_RANGE)))//是否在允许设置的范围内
                    {
                      //是否在上限位点以内
                      if ((Hall_Pulse_Amount_Cnt > (Limit_Point[0].Limit_Place - 10)))
                      {
                        //是否已经设置了下限位点
                        if (Limit_Point[1].Flag.F1.Limit_Activate)
                        {
                          //是否在下限位点以内
                          if (Hall_Pulse_Amount_Cnt < (Limit_Point[Limit_Point_Amount-1].Limit_Place + 10))
                          {
                            Flag.Set_Limit_Point = TRUE;//进入设置限位状态
                            
                            Flag.Opretion_Finish = TRUE;//启动抖动提醒
                            Opretion_Event = NULL;
                          }
                        }
                        else
                        {
                          Flag.Set_Limit_Point = TRUE;//进入设置限位状态
                          
                          Flag.Opretion_Finish = TRUE;//启动抖动提醒
                          Opretion_Event = NULL;
                        }
                      }
                    }
                  }
                }
              }
            }
          }
          break;
          
        case RF_DATA_CLEAR://删限位点
          if (Flag.Set_Limit_Point)//是在设置限位状态
          {
            // Flag.Opretion_Finish = TRUE;//启动抖动提醒
            Opretion_Event = NULL;
            
            Flag.Set_Limit_Point = NULL;
            Flag.Delete_Limit_Point = TRUE;
          }
          break;
          
        case RF_UP_DOWN_5S://进入点动角度调整
          if (!Flag.Joint_Level_Adj)
          {
            Flag.Joint_Level_Adj = TRUE;
            
            Flag.Opretion_Finish = TRUE;//启动抖动提醒
            Opretion_Event = NULL;
          }
          break;
        case RF_UP_STOP://上行键+停止，点到角度调大
          if (Flag.Joint_Level_Adj)
          {
            if (Joint_Time_Level < 20)
            {
              Joint_Time_Level++;//按一下加大一级
              Send_Data(1,0x06,0,8);
              Flag.Opretion_Finish = TRUE;//启动抖动提醒
              Opretion_Event = NULL;
              
              Motor.Motor_Status = MOTOR_UP;
              Flag.Jiggle_Stop = TRUE;
            }
          }
          break;
          
        case RF_DOWN_STOP://下行键+停止，点到角度调大
          if (Flag.Joint_Level_Adj)
          {
            if (Joint_Time_Level >=2)
            {
              Joint_Time_Level--;//按一下减小一级
              Send_Data(1,0x06,0,8);
              Flag.Opretion_Finish = TRUE;//启动抖动提醒
              Opretion_Event = NULL;
              
              Motor.Motor_Status = MOTOR_DOWN;
              Flag.Jiggle_Stop = TRUE;
            }
          }
          break;
        case RF_STOP_2S://停止键长按2秒，退出角度调节
          if (Flag.Joint_Level_Adj)
          {
            Flag.Joint_Level_Adj = 0;
            Flag.Opretion_Finish = TRUE;
            Opretion_Event = NULL;
          }
          break;
        case RF_DATA_RESET_NETWORK://清网
          {
            if (!Flag.RF_Data_Disposal)
            {
              Flag.RF_Data_Disposal = TRUE;
              set_key_cnt++;
              Set_Key_Time_Cnt = 0;
              if(set_key_cnt >= 3)//短按set键3~4次，进入清网
              {
                set_key_cnt = 0;
                Send_Data(1,0x04,0,0);
                Flag.Learn_Code_Statu = FALSE;
                Flag.Opretion_Finish = TRUE;
                Flag.CLR_Net_Oder = TRUE;
                Flag.net_add = TRUE;
                Flag.Uart_tx = TRUE;   // 串口发送清网命令提示
              }
            }
          }
          break;
        }
      }
    }
    else//没有相同的ID和通道
    {
      if (Flag.Learn_Code_Statu)//是否在学习状态
      {
        if (Flag.Power_Up_Learn)
        {
          if ((Delete_single_Code_ID[1] == RF_RxData[1])
              &&  (Delete_single_Code_ID[2] == RF_RxData[2])
                &&  (Delete_single_Code_ID[3] == RF_RxData[3]))
          {
            if (RF_RxData[6] == RF_UP)                      /* 是否上行键 ------*/
            {
              if (0 != ch_addr)                               /* 是否有相同的ID --*/
              {
                SaveChannel(RF_RxData,ch_addr);
              }
              else
              {
                SaveID(RF_RxData);                               /* 学习ID ----------*/
              }
              Flag.Learn_Code_Statu = 0;
              Flag.Power_Up_Learn = 0;
              Flag.Opretion_Finish=1;
              
            }
          }
        }
        else 
        {
          if (RF_RxData[6] == RF_DATA_UP)//是否上行键
          {
            if (NULL != ch_addr)//是否有相同的ID
            {
              SaveChannel(RF_RxData,ch_addr);
            }
            else
            {
              SaveID(RF_RxData);//学习ID
            }
            Flag.Opretion_Finish=1;
            Flag.Learn_Code_Statu = NULL;
          }
        }
      }
    }
  }
}
//---------------------------------------------------------------------------------
//  函数名:  SetLimitPoint
//  功  能:   
//  输  入:  空
//  输  出:  空
//  返  回:	  
//  描  述：  
//---------------------------------------------------------------------------------
void SetLimitPoint (void)
{
  u8 i;
  
  if ((Flag.Save_Limit_Point)&&(!Flag.Del_Code_Finish) && (!Flag.Opretion_Finish))
  {
    Flag.Save_Limit_Point = NULL;
    if (!Limit_Point[0].Flag.F1.Limit_Activate)//是否尚未设定限位点
    {
      Limit_Point[0].Flag.F1.Limit_Activate = TRUE;
      Limit_Point[0].Limit_Place = MIN_JOURNEY_VALUE;
      Hall_Pulse_Amount_Cnt = MIN_JOURNEY_VALUE;
      Target_Limit_Point = NULL;
      Limit_Point_Amount = TRUE;
      
      Flag.Set_Limit_Point = NULL;//退出设置状态
    }
    else//已经有限位点
    {
      if (!(LoopCompare(Hall_Pulse_Amount_Cnt,LIMIT_SET_RANGE)))//当前位置没有和某个限位点重合
      {
        i = LoopCompare(1,LIMIT_ADJUST);
        if (i != NULL)//是否要微调限位点
        {
          Limit_Point[--i].Flag.F1.Fine_Adjust = NULL;//清微调限位点标志
          Limit_Point[i].Limit_Place = Hall_Pulse_Amount_Cnt;//保存当前位置
        }
        else // 设置新限位点
        {
          i = LoopCompare(0,LIMIT_ACTIVATE);//比较那组没有限位
          if (i != NULL)//是否有没有设置限位的
          {
            Limit_Point[--i].Flag.F1.Limit_Activate = TRUE;   
            Limit_Point[i].Limit_Place = Hall_Pulse_Amount_Cnt;
          }
        }
        Limit_Point_Amount = LimitCollator(Limit_Point);
        
        Flag.Set_Limit_Point = NULL;//退出设置状态
      }
    }
  }
  else if (Flag.Delete_Limit_Point)
  {
    Flag.Delete_Limit_Point = 0;
    
    i = LoopCompare(0,LIMIT_ADJUST);
    if (i != 0)//是否在微调限位状态
    {
      Limit_Point[i - 1].Flag.F1.Fine_Adjust = 0;//删微调限位点标志
      //第一个限位点不删
      if (i > 1)//是否不是第一个限位
      {
        i--;
        Limit_Point[i].Flag.F1.Limit_Activate = 0;
        Limit_Point[i].Limit_Place = 0;//删掉该限位点
        Limit_Point_Amount = LimitCollator(Limit_Point);//删掉限位点后重新排序               
        
        Flag.Opretion_Finish = TRUE;//启动抖动提醒
        Opretion_Event = NULL;
        
        Flag.Set_Limit_Point = 0;//退出设置状态
      }
    }
  }
}
//---------------------------------------------------------------------------------
//  函数名:  RFSignalDecode
//  功  能:  RF信号解码
//  输  入:  空
//  输  出:  空
//  返  回:  是否收到正确数据标志
//  描  述：  
//---------------------------------------------------------------------------------
u8 RFSignalDecode (u16 High_Time,u16 Low_Time)
{
  u8 flag = NULL;
  u8 temp = High_Time + Low_Time;
  
  if (!Flag_RF.Signal_Head)//是否没有信号头
  {
    if ((RF_High_Cnt <= HEAD_LONG_MAX) && (RF_High_Cnt >= HEAD_LONG_MIN))
    { 
      if ((Low_Time <= HEAD_SHORT_MAX) && (Low_Time >= HEAD_SHORT_MIN))
      {
        Flag_RF.Signal_Head = TRUE;
        Rx_Data_P = RF_RxData_Buf;
        RF_Bit_cnt = NULL;
        RF_Byte_Cnt = NULL;
      }
      else if (Low_Time <= HEAD_SHORT_MIN)
      {
        RF_High_Cnt_Buffer += RF_High_Cnt;
        RF_Low_Cnt_Buffer += RF_Low_Cnt;
      }
    }
    else if (Low_Time <= HEAD_SHORT_MIN)
    {
      RF_High_Cnt_Buffer += RF_High_Cnt;
      RF_Low_Cnt_Buffer += RF_Low_Cnt;
    }
  }
  else//已有信号头
  {
    if (temp < SIGNAL_BIT_MIN)
    {
      RF_High_Cnt_Buffer += RF_High_Cnt;
      RF_Low_Cnt_Buffer += RF_Low_Cnt;
      return (flag);
    }
    
    if ((temp <= SIGNAL_BIT_MAX) && (temp >= SIGNAL_BIT_MIN))
    {       
      *Rx_Data_P <<= 1;
      if (High_Time > Low_Time)
      {
        *Rx_Data_P |= 0x01;//接收1bit
      }
      if (++RF_Bit_cnt >= DATA_BIT_AMOUNT)//一个字节是否收完
      {
        RF_Bit_cnt = NULL;
        ++Rx_Data_P;                             
        if (++RF_Byte_Cnt >= RF_MAX_BYTE_VALUE)//数据是否接收完成
        {
          flag = TRUE;//置处理标志
          RF_KeyUnloosen_Cnt = NULL;//清松键计数器
          RF_Byte_Cnt = NULL;
          ErrorDisposal();
        }
      }
    }
    else if ((RF_High_Cnt <= HEAD_LONG_MAX) && (RF_High_Cnt >= HEAD_LONG_MIN))
    {
      if ((Low_Time <= HEAD_SHORT_MAX) && (Low_Time >= HEAD_SHORT_MIN))
      {
        Flag_RF.Signal_Head = TRUE;
        Rx_Data_P = RF_RxData_Buf;
        RF_Bit_cnt = NULL;
        RF_Byte_Cnt = NULL;
      }
      else
      {
        ErrorDisposal();
      }
    }
    else
    {
      ErrorDisposal();
    }
  }
  
  return (flag);
}
//---------------------------------------------------------------------------------
//  函数名:  LoopCompare
//  功  能:   
//  输  入:  空
//  输  出:  空
//  返  回:	  
//  描  述：  
//---------------------------------------------------------------------------------
u8 LoopCompare (u32 data,u8 type)
{
  u8 i;
  u8 flag = NULL;
  
  for (i = 0; i < (MID_LIMIT_POINT_AMOUNT + 2); i++)
  {
    if (type == LIMIT_ACTIVATE)
    {
      if (data == NULL)
      {
        if (!Limit_Point[i].Flag.F1.Limit_Activate)
        {
          flag = TRUE;
          break;
        }
      }
      else
      {
        if (Limit_Point[i].Flag.F1.Limit_Activate)
        {
          flag = TRUE;
          break;
        }
      } 
    }
    else if (type == LIMIT_ADJUST)
    {
      if (Limit_Point[i].Flag.F1.Fine_Adjust)
      {
        flag = TRUE;
        break;
      }
    }
    else if (type == LIMIT_PLACE)
    {
      if (Limit_Point[i].Flag.F1.Limit_Activate)
      {
        if ((Limit_Point[i].Limit_Place < (data + 4)) 
            && (Limit_Point[i].Limit_Place > (data - 4))) 
        {
          flag = TRUE;
          break;
        }
      }
    }
    else if (type == LIMIT_SET_RANGE)//检查当前是否在允许设置限位的范围内
    {
      if (Limit_Point[i].Flag.F1.Limit_Activate)//是否已经有限位
      {
        if ((Limit_Point[i].Limit_Place < (data + 40))//当前位置是否接近该限位点
            && (Limit_Point[i].Limit_Place > (data - 40))) 
        {
          if (!Limit_Point[i].Flag.F1.Fine_Adjust)//不在微调状态
          {
            flag = TRUE;
          }
        }
      }
    }
  }
  
  return ((flag)? (i +1):NULL);
}



