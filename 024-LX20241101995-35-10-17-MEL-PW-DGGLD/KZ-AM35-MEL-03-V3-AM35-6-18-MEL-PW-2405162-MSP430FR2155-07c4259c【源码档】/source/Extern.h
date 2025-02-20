/**********************************************************************************
 *  项目编号:    
 *  项目名称:    
 *  客户名称:                                    
 *  程序编写:    
 *  启动日期:    
 *  完成日期:    
 *  版本号码:    V1.0
 *
 *  Extern.h file
 *  功能描述: 外部变量、函数声明
**********************************************************************************/
#ifndef Extern_h
  #define Extern_h

  //全局位变量声明 ---------------------------------------------------------------------
  extern struct 
  {
    u8 RF_Rx_Data                : 1;  
    u8 RF_Rx_Finish              : 1;//RF接收数据标志
    u8 RF_Data_Disposal          : 1;//RF数据处理标志
    
    u8 Time_200us                : 1;//200us时间标志
    u8 Time_2ms                  : 1;//2ms时间标志
    u8 Time_10ms                 : 1;//10ms时间标志
    u8 Time_50ms                 : 1;//50ms时间标志
    u8 Time_100ms                : 1;//100ms时间标志
    u8 Time_500ms                : 1;//500ms时间标志
    u8 Time_1s                   : 1;//1s时间标志
    //---------------------------------------------------------------------------------
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

  extern struct 
  {
    u8 Signal_Status             : 1;//RF电平状态标志
    u8 Key_Disposal              : 1;//RF处理标志
    u8 Signal_Head               : 1;//RF信号头标志
    u8 Raise_Edge                : 1;//RF上升沿标志
  
  }Flag_RF;

//  extern struct 
//  {
//    u8 UP_Dispose                : 1;
//    u8 DOWN_Dispose              : 1;
//    u8 STOP_Dispose              : 1;
//    u8 SET_Dispose               : 1;
//    u8 Direction_Dispose         : 1;
//    
//    u8 CLR_Disposal              : 1;
//    u8 CLR_Status                : 1;
//    u8 CLR_Net_Disposal          : 1;
//    u8 CLR_Net_Status            : 1;
//    
//  }KeyFlag;  
//  
  extern __no_init struct 
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
  
 extern struct
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

  
  extern __no_init u16 Hall_Pulse_Width;//霍尔输出脉冲宽度
  extern __no_init u32 Hall_Pulse_Amount_Cnt;//霍尔输出脉冲计数
  extern __no_init Limit Limit_Point[MID_LIMIT_POINT_AMOUNT + 2];
  extern __no_init u8 Limit_Point_Amount;
  extern __no_init u8 Hall_Status_Buffer;
  extern __no_init u8 hall_change_cnt;
  extern __no_init u16 Hall_Time_Cnt;
  
  extern __no_init u16 Power_Down_Cnt;
  extern __no_init u8  Joint_Time_Level;
  extern __no_init u32 Initialize_FRAM_FLAG;
  
  extern __no_init u8 RF_ID_Amount;
  extern __no_init u8 RF_ID_Save_Place;
  extern __no_init u8 RF_ID_Buffer[ID_MAX_AMOUNT][ID_LENGTH];
  
  extern __no_init Uart_Baud_Rate_TypeDef Uart_Baud_Rate;
  extern __no_init u8 Uart_ID;
  extern __no_init u8 Uart_CH_L;
  extern __no_init u8 Uart_CH_H;
  extern __no_init u8 Motor_Max_Speed;
  extern __no_init u8 Dry_Contact_Control_Mode;
  extern __no_init Motor_TypeDef Motor;
  extern __no_init u8 Switvh_Direction_Order;
  extern __no_init u8 Meet_Plugb_Level;
  extern __no_init u8 Sweet_Spot;
  //-----------------------------------------------------------------------------------
  extern u8 Time_10ms_Cnt;
  extern u8 Time_50ms_Cnt;
  extern u8 Time_100ms_Cnt;
  extern u8 Time_500ms_Cnt;
  extern u8 Time_1s_Cnt;
  //-----------------------------------------------------------------------------------
  extern u8 RF_High_Cnt;//RF高电平时间
  extern u8 RF_Low_Cnt;//RF底电平时间
  //extern u8 Motor_Status_bak;
  
  extern u32 Hall_Pulse_Amount_PCT; //霍尔输出脉冲计数
  extern u32 OprationCue_Amount_Buf;//霍尔输出脉冲计数
  extern u32 Hall_Pulse_Jiggle_Buff;
  
  extern u8 No_learn_key_cnt;
  
  extern u8 Delete_single_Code_ID[6];
  extern u8 Opretion_Event;
  
  extern u8 Target_Limit_Point;
  extern u8 Limit_Point_Amount;
  extern u8 RF_Dblclick_Time;
  extern u8 Run_To_Mid_Limit_Point;
  extern u8 motor_run_time;
  extern u8 motor_off_time;
  
  extern u8 Motor_Speed;//电机转速
  extern u8 Target_Speed;
  extern u16 Motor_Duty;
  
  extern u8 Set_Limit_Point_Cnt;
   
  extern u16 Time_1s_Hint_Cnt;
  extern u8 Delete_single_Code_Cnt;
  extern u8 PowerDown_Delay_Time;
  extern Work_Status_TypeDef Work_Status;
  
  extern u16 Key_Wakeup_time_cnt;

  extern u8 RF_High_Cnt_Buffer;//数据时间缓存
  extern u8 RF_Low_Cnt_Buffer;//数据时间缓存
  extern Motor_Control_TypeDef Joint_Action_Motor_Buf;
  extern u16 Joint_Action_Time_Cnt;
  extern u16 Joint_Level_ADJ_Time;
  
  extern u8 No_ac_power;
  extern u8 No_ac_power_Cnt;
  
  extern u16 Current_Current_mA;
    extern u8   t50ms_c;                   //liu
  //-----------------------------------------------------------------------------------
  extern u16 Learn_Key_Cnt;
  extern u8 Key_Old_Value;
  extern u8 CLR_Key_Num;
  extern u16 CLR_Key_Time_Out_Cnt;
  
  /***************HUAWEI模块移植**************/
    extern u8  flag_tx,//串口主动发送标志
              flag_rx,//串口接收完成标志
              rx_tc,//接收间隔计时变量（超40ms认为接收结束）
              respons_tc,//串口响应计时（超400ms认为无响应)
              tx_c,//无响应重复发送次数
              red_led_tc,//红灯状态转换
              red_led_tc2,
              test_tx_tc,
              test_fail_tc,
              Motor_Status_bak,//以前的状态
              Motor_Flag_Direction_bak,//上一个默认方向
              tx_ok_tc,//最后一个字节发送完成后延时4ms切换RS485_RX
              JourneyPercentCount_bak;//上一个百分比
  extern u8  tx_data,tx_data_i,tx_data_n;//发送命令字
  extern u8  flag_auto_test,
              flag_power_on2,
              flag_power_on;
  extern u8  tx_BACK_STATUS;
  extern u8  tx_BACK_STATUS_2;
  
  extern u8 Motor_Status_Buffer_1;//huang
  extern u32 Click_the_Angle_of_hall;//转动角度的霍尔值huang 
  extern u8 SendTX_interval_Time;//串口间隔时间
  
    extern u8 Motor_Status_Buffer_1;//huang
      extern u8 Time_Power_On;
  extern u8 learn_key_cnt;
   extern u8 Current_Angle_5;            //   步长5
    
  extern u32 Click_the_Angle_of_hall;//转动角度的霍尔值huang 
  extern __no_init u8 Current_Angle;                                   /*当前角度*/
  //extern __no_init u8  Hall_Pulse_Amount_stop;                          /* 上一次霍尔输出脉冲计数 */
  extern __no_init u32  Hall_Pulse_Amount_up;                            /* 上一次0度或者180度时霍尔输出脉冲计数 */  
  //extern __no_init u32  Hall_Pulse_Amount_down;                          /* 上一次180度时霍尔输出脉冲计数 */  
  extern __no_init u32  Hall_difference;//霍尔差值 huang
  extern __no_init u8 Restrict_Up ;                              /* 上限位先设置标志 */
  extern u8 Hall_Pulse_Amount_up_buf;            //缓存
  
   extern u8 TEXT_1;            //缓存
    extern u8 TEXT_2;            //缓存
  extern u8 TEXT_3;            //缓存
  extern u8 set_key_cnt;
  extern u8 Set_Key_Time_Cnt;
  extern u8 Meet_Plugb_Rebound_Time;//临时改变反弹时间（到下限位反弹1S）
  
  //-----------------------------------------------------------------------------------
  //函数声明
  //-----------------------------------------------------------------------------------
  extern void Initialize           (void);//初始化函数
  extern void GPIOSetting          (void);
  extern void CLKSetting           (void);
  extern void TIMXSetting          (void);
  extern void Delay                (register unsigned int i);
  extern void KeyManage            (void);//按键扫描及处理
  extern void LearnDelCodeTime     (void);//学码删码时间控制
  extern void TimeControl          (void);  
                                  
  //ADC 处理                     
  extern void ADCSetting           (void);
  extern void ADSamping            (void);//AD采样函数
                                  
  //Uart 处理函数                
  extern void LinUartInit          (void);
  extern void LinUart0Init         (void);
                                  
  //extern void DetectPowerIn        (void);
  extern void PowerInScan          (void);
                                  
  extern void Dis_no_power         (void);
  
  extern void ErrorDisposal        (void);
  extern u8   RFSignalDecode       (u16 High_Time,u16 Low_Time);//RF解码函数
  extern u8   RFDataVerify         (void);//RF数据校验
  extern void RFDataDisposal       (void);//RF处理函数
  extern void RFStatusInspect      (void);
                                  
  extern u8   CompID               (u8 array[]);//比较是否有相同ID
  extern u8   SaveID               (u8 array[]);//保存新的ID
  extern void DeleteAllID          (u8 check);//删除所有的ID
  extern u8   DeleteChannel        (u8 array[],u8 ch_addr);//删除通道
  extern u8   SaveChannel          (u8 array[],u8 ch_addr);//保存通道
                                  
  extern void MotorControl         (void);//电机控制
  extern void MotorStatusSwitch    (void);//电机状态切换
  extern void MotorStop            (void);
  extern void MotorDown            (u16 duty);
  extern void MotorUp              (u16 duty);
  extern void CountMotorSpeed      (void);
  extern void PowerControl         (void);
                                  
  extern u8   LoopCompare          (u32 data,u8 type);
  extern void JourneyComper        (void);
  extern void DblclickTimeLimit    (void);
  extern u8   MeetPlugbDetect      (void);
  extern void SetLimitPoint        (void);
                                  
                                  
  extern void FlashReadData        (void);
                                  
  extern void PowerDownSaveData    (void);
  extern void PowerDownSamping     (void);
                                  
  extern u16  SampleAverage        (u16 array[],u8 num);
  extern void ADSamping            (void);
                                  
  extern void RFWakeup             (void);
  extern u8   LimitCollator        (Limit array[]);
  extern void Duty_Sub             (u16 *p,u8 i);
  
  extern void Uart1StartupTX       (void);
  extern void Uart1DataDisposal    (void);
  extern void Uart1_Status_Inspect (void);
  
  /*************HUAWEI模块移植**************/
  extern u8 JourneyPercentCount (void);
  extern u32 CountPercentPlace (u8 percent);
  extern void UartControl_WIFI (u8 data);                   //电机控制  HUAWEI模块
  // Uart 处理函数
  extern void LinUartInit (void);
  extern void UartDataDisposal (void);
  extern void UartStartupTX (void);
  extern void UartControl (u8 data);
  extern void Dispose_All_Date_For_TX(void);                //上传所有状态的数据处理
  extern void Angle_calculation(void);                                   /* 角度计算函数 ----*///huang
  extern void UART_TEXT(u8 DATE_1,u8 DATE_2,u8 DATE_3,u8 DATE_4,u8 DATE_5,u8 DATE_6);               //上传所有状态的数据处理
  extern void JourneyDelete(void);
  extern void Motor_Data_Change_Send(void);                //           主动上报

  extern void DetectPowerIn (void);
  extern void Send_Data(u8 TX_Times,u8 TxKind,u8 Tx_total,u8 Tx_first);
#endif
