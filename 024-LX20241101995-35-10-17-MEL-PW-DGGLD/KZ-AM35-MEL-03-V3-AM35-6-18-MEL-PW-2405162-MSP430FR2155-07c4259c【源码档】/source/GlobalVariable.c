/**********************************************************************************
 *  项目编号:    
 *  项目名称:    
 *  客户名称:                                    
 *  程序编写:    
 *  启动日期:    
 *  完成日期:    
 *  版本号码:    V1.0
 *
 *  GlobalVariable.c file
 *  功能描述: 全局变量定义
**********************************************************************************/
#include "Define.h"
//全局位变量定义 -----------------------------------------------------------------------
struct 
{
  u8 RF_Rx_Data                : 1;  
  u8 RF_Rx_Finish              : 1;//RF接收数据标志
  u8 RF_Data_Disposal          : 1;//RF数据处理标志
  
  u8 Time_200us                : 1;// 200us时间标志
  u8 Time_2ms                  : 1;// 2ms时间标志
  u8 Time_10ms                 : 1;// 10ms时间标志
  u8 Time_50ms                 : 1;// 50ms时间标志
  u8 Time_100ms                : 1;// 100ms时间标志
  u8 Time_500ms                : 1;// 500ms时间标志
  u8 Time_1s                   : 1;// 1s时间标志
  //-----------------------------------------------------------------------------------
  u8 Learn_Code_Statu          : 1;//学码状态标志
  u8 Set_Key_ON                : 1;//设置键按下标志
  u8 EX_Key_ON                 : 1;//设置键按下标志
  u8 Direction_Key_ON          : 1;//设置键按下标志
  
  u8 Opretion_Finish           : 1;//操作成功标志

  u8 Delete_Code               : 1;//删码标志
  u8 Del_Code_Finish           : 1;//删码成功标志
  u8 Electrify_Reset           : 1;//上电复位标志
  u8 StatusSwitch_Delay        : 1;//电机换挡延迟标志
  
  u8 Delete_single_Code        : 1;//删除单个码标志
  u8 Set_Limit_Point           : 1;//设置限位点标志
  u8 Set_Journey_Direction     : 1;//设置行程方向标志
  
  u8 RF_UP_dblclick            : 1;//连续控制标志
  u8 RF_DOWN_dblclick          : 1;//连续控制标志
  u8 Run_To_finishing_point    : 1;//运行到终点标志
  
  u8 Save_Limit_Point          : 1;//保存限位点标志
  u8 Delete_Limit_Point        : 1;//删除限位点标志
  
  u8 OprationCue_Run           : 1;//电机下行标志
  u8 OprationCue_Stop          : 1;//电机下行标志
  u8 Jiggle_Stop               : 1;//点动停止标志
  
  u8 Joint_Action              : 1;//连动标志
  u8 Joint_Level_Adj           : 1;                          
     
  u8 Power_Down                : 1;   
  u8 Key_Wakeup                : 1; 
  
  u8 NO_Comper_Limit_Point     : 1;
  u8 Motor_Status_Switch       : 1;
  u8 slowstop                  : 1;
  
  u8 Journey_Arrive            : 1;
  u8 Motor_Run_Buffer          : 1;
  u8 Power_Up_Learn            : 1;
  
  u8 Uart_Read_OK              : 1;//串口接收OK标志
  u8 Goto_PCT                  : 1;
  u8 Uart_tx                   : 1;//串口正发送标志
  u8 CLR_Net_Oder              : 1;
  u8 net_add                   : 1;
  
  /****************HUAWEI*****************/
  
    u8 Angle_mark                : 1; //huang
  u8 Angle_mark_1              : 1; //huang
  u8 Angle_correction          : 1; //huang
  u8 Angle_Stop                : 1;                          /* 运行角度停止标志 ----*/ 
  u8 Angle_to_upload           : 1;//huang
  u8 The_network_jitter_once   : 1;//huang
  u8 Power_on                  : 1;                      //上电标志
  u8 Send_JourneyPercentCount  : 1;//发送百分比
  u8 Send_Current_Angle        : 1;//发送角度
  u8 Hall_Pulse_Amount_up_buf  : 1;            //缓存大小
  u8 Send_JourneyPercentCount_1     : 1;       //目标值上报
  u8 rest_text_receive         : 1;                        //上电接收一次产测
  u8 WifiTest                  : 1;
  u8 statTest                  : 1; 
  u8 Meet_Plugb_Rebound        : 1;                       //遇阻反弹标志
  u8 Up_Meet_Plugb             : 1;                       //上限位反弹1S解锁标志20250217
  u8 Control_Tighten_Up        : 1;                       //收紧打开标志位20250217
}Flag;
//-------------------------------------------------------------------------------------
struct 
{
  u8 Signal_Status             : 1;//RF电平状态标志
  u8 Key_Disposal              : 1;//RF处理标志
  u8 Signal_Head               : 1;//RF信号头标志
  u8 Raise_Edge                : 1;//RF上升沿标志
}Flag_RF;

//struct 
//{
//  u8 UP_Dispose                : 1;
//  u8 DOWN_Dispose              : 1;
//  u8 STOP_Dispose              : 1;
//  u8 SET_Dispose               : 1;
//  u8 Direction_Dispose         : 1;
//  
//  u8 CLR_Disposal              : 1;
//  u8 CLR_Status                : 1;
//  u8 CLR_Net_Disposal          : 1;
//  u8 CLR_Net_Status            : 1;
//  
//}KeyFlag;
 
__no_init struct
{
  u8 Motor_Direction           : 1;//电机方向标志
  u8 Motor_Fact_Direction      : 1;//电机实际方向标志
  u8 Motor_Run                 : 1;//电机运行标志
  u8 Hall_Pulse_edge           : 1;//霍尔脉冲边沿标志
  u8 Direction                 : 1;//方向标志
  u8 Single_Journey            : 1;//单个行程标志
  u8 Journey_Set_OK            : 1;//行程设定OK标志
  u8 Journey_Direction         : 1;//行程方向标志
  u8 EE_Save                   : 1;//上电标志
  u8 Motor_Jiggle_Status       : 1;//电机点动运行标志
    
}MotorFlag;


 struct
{
  u8 Motor_Status             :8;//上个电机状态
  u8 Motor_Status_1             :8;//上个电机状态
  u8 Motor_Direction          :1;//上个方向
  u8 Journey_Percent_Count     :8;//上个百分比
  u8 Flip_Angle                :8;//上个角度
  u8 Joint_Level               :5;//    上个点动等级
  u8 Joint_Status               :1;//    上个运行状态（点动连动）
  u8 Up_Limit_Status           :1;//    上个上限位状态
  u8 Down_Limit_Status           :1;//    上个下限位状态
  u8 Motor_Mode_Status           :1;//    上报开和方向
  
}Motor_Data_Bak;

__no_init u16 Hall_Pulse_Width;//霍尔输出脉冲宽度
__no_init u32 Hall_Pulse_Amount_Cnt;//霍尔输出脉冲计数

u8 Motor_Status_Buffer_1;//huang
u32 Click_the_Angle_of_hall;//转动角度的霍尔值huang
__no_init u8 Current_Angle;                                   /*当前角度*/
 //__no_init u8  Hall_Pulse_Amount_stop;                          /* 霍尔输出脉冲计数 */
 __no_init u32  Hall_Pulse_Amount_up;                          /* 霍尔输出脉冲计数 */  
 //__no_init u32  Hall_Pulse_Amount_down;                          /* 霍尔输出脉冲计数 */ 
 __no_init u32  Hall_difference;//霍尔差值 huang 
 __no_init u8 Restrict_Up ;                              /* 上限位先设置标志 */

 


__no_init Limit Limit_Point[MID_LIMIT_POINT_AMOUNT + 2];
__no_init u8 Limit_Point_Amount;
__no_init u8 Hall_Status_Buffer;
__no_init u8 hall_change_cnt;
__no_init u16 Hall_Time_Cnt;

__no_init u16 Power_Down_Cnt;
__no_init u8 Joint_Time_Level;
__no_init u32 Initialize_FRAM_FLAG;

__no_init u8 RF_ID_Amount;
__no_init u8 RF_ID_Save_Place;
__no_init u8 RF_ID_Buffer[ID_MAX_AMOUNT][ID_LENGTH];

__no_init Uart_Baud_Rate_TypeDef Uart_Baud_Rate;
__no_init u8 Uart_ID;
__no_init u8 Uart_CH_L;
__no_init u8 Uart_CH_H;
__no_init u8 Motor_Max_Speed;
__no_init u8 Dry_Contact_Control_Mode;
__no_init Motor_TypeDef Motor;
__no_init u8 Switvh_Direction_Order;
__no_init u8 Meet_Plugb_Level;
__no_init u8 Sweet_Spot;
//-------------------------------------------------------------------------------------
u8 Time_10ms_Cnt = TIME_10MS_VALUE;
u8 Time_50ms_Cnt = TIME_50MS_VALUE;
u8 Time_100ms_Cnt = TIME_100MS_VALUE;
u8 Time_500ms_Cnt = TIME_500MS_VALUE;
u8 Time_1s_Cnt = TIME_1S_VALUE;
//-------------------------------------------------------------------------------------
u8 RF_High_Cnt;//RF高电平时间
u8 RF_Low_Cnt;//RF底电平时间
//u8 Motor_Status_bak;

u32 Hall_Pulse_Amount_PCT;//霍尔输出脉冲计数
u32 OprationCue_Amount_Buf;//霍尔输出脉冲计数
u32 Hall_Pulse_Jiggle_Buff;

u8 No_learn_key_cnt;

u8 Delete_single_Code_ID[6];
u8 Opretion_Event;

u8 Target_Limit_Point;
u8 Limit_Point_Amount;
u8 RF_Dblclick_Time;
u8 Run_To_Mid_Limit_Point;
u8 motor_run_time;
u8 motor_off_time;

u8  Motor_Speed;//电机转速
u8  Target_Speed;
u16 Motor_Duty;

u8 Set_Limit_Point_Cnt;

u16 Time_1s_Hint_Cnt;
u8 Delete_single_Code_Cnt;
u8 PowerDown_Delay_Time;

u16 Key_Wakeup_time_cnt;

u8 RF_High_Cnt_Buffer;//数据时间缓存
u8 RF_Low_Cnt_Buffer;//数据时间缓存
Motor_Control_TypeDef Joint_Action_Motor_Buf;
u16 Joint_Action_Time_Cnt;
u16 Joint_Level_ADJ_Time;

u8 No_ac_power;
u8 No_ac_power_Cnt;

u16 Current_Current_mA;
u8   t50ms_c;                   //liu
u8 Meet_Plugb_Rebound_Time;//反弹时间（到下限位反弹1S）2025-2-17

/**************HUAWEI模块移植***************/
u8  flag_tx,//串口主动发送标志
    flag_rx,//串口接收完成标志
    rx_tc,//接收间隔计时变量（超40ms认为接收结束）
    respons_tc,//串口响应计时（超400ms认为无响应)
    tx_c,//无响应重复发送次数
    red_led_tc,//红灯状态转换
    red_led_tc2,
    test_tx_tc,
    test_fail_tc,
    Motor_Status_bak,//
    Motor_Flag_Direction_bak,//上一个默认方向
    tx_ok_tc,//最后一个字节发送完成后延时4ms切换RS485_RX
    JourneyPercentCount_bak;//上一个百分比
u8  tx_data,tx_data_i,tx_data_n;//发送命令字
u8  flag_auto_test,
    flag_power_on2,
    flag_power_on;

 u8 Motor_Status_Buffer_1;//huang
 u32 Click_the_Angle_of_hall;//转动角度的霍尔值huang 
 u8 SendTX_interval_Time;//串口间隔时间
 u8 Time_Power_On;//上电标志
 u8 learn_key_cnt;            //   按背面键次数
 u8 Current_Angle_5;            //   步长5
 u8 Hall_Pulse_Amount_up_buf;            //缓存
 
 u8 TEXT_1;            //缓存
  u8 TEXT_2;            //缓存
   u8 TEXT_3;            //缓存
   
 u8 set_key_cnt;
 u8 Set_Key_Time_Cnt;
