/**********************************************************************************
 *  ��Ŀ���:    
 *  ��Ŀ����:    
 *  �ͻ�����:                                    
 *  �����д:    
 *  ��������:    
 *  �������:    
 *  �汾����:    V1.0
 *
 *  Extern.h file
 *  ��������: �ⲿ��������������
**********************************************************************************/
#ifndef Extern_h
  #define Extern_h

  //ȫ��λ�������� ---------------------------------------------------------------------
  extern struct 
  {
    u8 RF_Rx_Data                : 1;  
    u8 RF_Rx_Finish              : 1;//RF�������ݱ�־
    u8 RF_Data_Disposal          : 1;//RF���ݴ����־
    
    u8 Time_200us                : 1;//200usʱ���־
    u8 Time_2ms                  : 1;//2msʱ���־
    u8 Time_10ms                 : 1;//10msʱ���־
    u8 Time_50ms                 : 1;//50msʱ���־
    u8 Time_100ms                : 1;//100msʱ���־
    u8 Time_500ms                : 1;//500msʱ���־
    u8 Time_1s                   : 1;//1sʱ���־
    //---------------------------------------------------------------------------------
    u8 Learn_Code_Statu          : 1;//ѧ��״̬��־
    u8 Set_Key_ON                : 1;//���ü����±�־
    u8 EX_Key_ON                 : 1;//���ü����±�־
    u8 Direction_Key_ON          : 1;//���ü����±�־
    
    u8 Opretion_Finish           : 1;//�����ɹ���־
    
    u8 Delete_Code               : 1;//ɾ���־
    u8 Del_Code_Finish           : 1;//ɾ��ɹ���־
    u8 Electrify_Reset           : 1;//�ϵ縴λ��־
    u8 StatusSwitch_Delay        : 1;//��������ӳٱ�־
    
    u8 Delete_single_Code        : 1;//ɾ���������־
    u8 Set_Limit_Point           : 1;//������λ���־
    u8 Set_Journey_Direction     : 1;//�����г̷����־
    
    u8 RF_UP_dblclick            : 1;//�������Ʊ�־
    u8 RF_DOWN_dblclick          : 1;//�������Ʊ�־
    u8 Run_To_finishing_point    : 1;//���е��յ��־
    
    u8 Save_Limit_Point          : 1;//������λ���־
    u8 Delete_Limit_Point        : 1;//ɾ����λ���־
                                     
    u8 OprationCue_Run           : 1;//������б�־
    u8 OprationCue_Stop          : 1;//������б�־
    u8 Jiggle_Stop               : 1;//�㶯ֹͣ��־
                                      
    u8 Joint_Action              : 1;//������־
    u8 Joint_Level_Adj           : 1;                        
        
    u8 Power_Down                : 1;   
    u8 Key_Wakeup                : 1; 
    
    u8 NO_Comper_Limit_Point     : 1;
    u8 Motor_Status_Switch       : 1;
    u8 slowstop                  : 1;
    
    u8 Journey_Arrive            : 1;
    u8 Motor_Run_Buffer          : 1;
    u8 Power_Up_Learn            : 1;
    
    u8 Uart_Read_OK              : 1;//���ڽ���OK��־
    u8 Goto_PCT                  : 1;
    u8 Uart_tx                   : 1;//���������ͱ�־
    u8 CLR_Net_Oder              : 1;
    u8 net_add                   : 1;
    /****************HUAWEI*****************/
  
    u8 Angle_mark                : 1; //huang
  u8 Angle_mark_1              : 1; //huang
  u8 Angle_correction          : 1; //huang
  u8 Angle_Stop                : 1;                          /* ���нǶ�ֹͣ��־ ----*/ 
  u8 Angle_to_upload           : 1;//huang
  u8 The_network_jitter_once   : 1;//huang 
  u8 Power_on                  : 1;                      //�ϵ��־
  u8 Send_JourneyPercentCount  : 1;//���Ͱٷֱ�
  u8 Send_Current_Angle        : 1;//���ͽǶ�
  u8 Hall_Pulse_Amount_up_buf  : 1;            //�����С
  u8 Send_JourneyPercentCount_1     : 1;       //Ŀ��ֵ�ϱ�
  
  u8 rest_text_receive         : 1;                        //�ϵ����һ�β���
  u8 WifiTest                  : 1;
  u8 statTest                  : 1; 
  u8 Meet_Plugb_Rebound        : 1;                       //���跴����־
  u8 Up_Meet_Plugb             : 1;                       //����λ����1S������־20250217
  u8 Control_Tighten_Up        : 1;                       //�ս��򿪱�־λ20250217
  }Flag;

  extern struct 
  {
    u8 Signal_Status             : 1;//RF��ƽ״̬��־
    u8 Key_Disposal              : 1;//RF�����־
    u8 Signal_Head               : 1;//RF�ź�ͷ��־
    u8 Raise_Edge                : 1;//RF�����ر�־
  
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
    u8 Motor_Direction           : 1;//��������־
    u8 Motor_Fact_Direction      : 1;//���ʵ�ʷ����־
    u8 Motor_Run                 : 1;//������б�־
    u8 Hall_Pulse_edge           : 1;//����������ر�־
    u8 Direction                 : 1;//�����־
    u8 Single_Journey            : 1;//�����г̱�־
    u8 Journey_Set_OK            : 1;//�г��趨OK��־
    u8 Journey_Direction         : 1;//�г̷����־
    u8 EE_Save                   : 1;//�ϵ��־
    u8 Motor_Jiggle_Status       : 1;//����㶯���б�־
  
  }MotorFlag;
  
 extern struct
{
  u8 Motor_Status             :8;//�ϸ����״̬
  u8 Motor_Status_1             :8;//�ϸ����״̬
  u8 Motor_Direction          :1;//�ϸ�����
  u8 Journey_Percent_Count     :8;//�ϸ��ٷֱ�
  u8 Flip_Angle                :8;//�ϸ��Ƕ�
  u8 Joint_Level               :5;//    �ϸ��㶯�ȼ�
  u8 Joint_Status               :1;//    �ϸ�����״̬���㶯������
  u8 Up_Limit_Status           :1;//    �ϸ�����λ״̬
  u8 Down_Limit_Status           :1;//    �ϸ�����λ״̬
  u8 Motor_Mode_Status           :1;//    �ϱ����ͷ���

}Motor_Data_Bak;

  
  extern __no_init u16 Hall_Pulse_Width;//�������������
  extern __no_init u32 Hall_Pulse_Amount_Cnt;//��������������
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
  extern u8 RF_High_Cnt;//RF�ߵ�ƽʱ��
  extern u8 RF_Low_Cnt;//RF�׵�ƽʱ��
  //extern u8 Motor_Status_bak;
  
  extern u32 Hall_Pulse_Amount_PCT; //��������������
  extern u32 OprationCue_Amount_Buf;//��������������
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
  
  extern u8 Motor_Speed;//���ת��
  extern u8 Target_Speed;
  extern u16 Motor_Duty;
  
  extern u8 Set_Limit_Point_Cnt;
   
  extern u16 Time_1s_Hint_Cnt;
  extern u8 Delete_single_Code_Cnt;
  extern u8 PowerDown_Delay_Time;
  extern Work_Status_TypeDef Work_Status;
  
  extern u16 Key_Wakeup_time_cnt;

  extern u8 RF_High_Cnt_Buffer;//����ʱ�仺��
  extern u8 RF_Low_Cnt_Buffer;//����ʱ�仺��
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
  
  /***************HUAWEIģ����ֲ**************/
    extern u8  flag_tx,//�����������ͱ�־
              flag_rx,//���ڽ�����ɱ�־
              rx_tc,//���ռ����ʱ��������40ms��Ϊ���ս�����
              respons_tc,//������Ӧ��ʱ����400ms��Ϊ����Ӧ)
              tx_c,//����Ӧ�ظ����ʹ���
              red_led_tc,//���״̬ת��
              red_led_tc2,
              test_tx_tc,
              test_fail_tc,
              Motor_Status_bak,//��ǰ��״̬
              Motor_Flag_Direction_bak,//��һ��Ĭ�Ϸ���
              tx_ok_tc,//���һ���ֽڷ�����ɺ���ʱ4ms�л�RS485_RX
              JourneyPercentCount_bak;//��һ���ٷֱ�
  extern u8  tx_data,tx_data_i,tx_data_n;//����������
  extern u8  flag_auto_test,
              flag_power_on2,
              flag_power_on;
  extern u8  tx_BACK_STATUS;
  extern u8  tx_BACK_STATUS_2;
  
  extern u8 Motor_Status_Buffer_1;//huang
  extern u32 Click_the_Angle_of_hall;//ת���ǶȵĻ���ֵhuang 
  extern u8 SendTX_interval_Time;//���ڼ��ʱ��
  
    extern u8 Motor_Status_Buffer_1;//huang
      extern u8 Time_Power_On;
  extern u8 learn_key_cnt;
   extern u8 Current_Angle_5;            //   ����5
    
  extern u32 Click_the_Angle_of_hall;//ת���ǶȵĻ���ֵhuang 
  extern __no_init u8 Current_Angle;                                   /*��ǰ�Ƕ�*/
  //extern __no_init u8  Hall_Pulse_Amount_stop;                          /* ��һ�λ������������� */
  extern __no_init u32  Hall_Pulse_Amount_up;                            /* ��һ��0�Ȼ���180��ʱ�������������� */  
  //extern __no_init u32  Hall_Pulse_Amount_down;                          /* ��һ��180��ʱ�������������� */  
  extern __no_init u32  Hall_difference;//������ֵ huang
  extern __no_init u8 Restrict_Up ;                              /* ����λ�����ñ�־ */
  extern u8 Hall_Pulse_Amount_up_buf;            //����
  
   extern u8 TEXT_1;            //����
    extern u8 TEXT_2;            //����
  extern u8 TEXT_3;            //����
  extern u8 set_key_cnt;
  extern u8 Set_Key_Time_Cnt;
  extern u8 Meet_Plugb_Rebound_Time;//��ʱ�ı䷴��ʱ�䣨������λ����1S��
  
  //-----------------------------------------------------------------------------------
  //��������
  //-----------------------------------------------------------------------------------
  extern void Initialize           (void);//��ʼ������
  extern void GPIOSetting          (void);
  extern void CLKSetting           (void);
  extern void TIMXSetting          (void);
  extern void Delay                (register unsigned int i);
  extern void KeyManage            (void);//����ɨ�輰����
  extern void LearnDelCodeTime     (void);//ѧ��ɾ��ʱ�����
  extern void TimeControl          (void);  
                                  
  //ADC ����                     
  extern void ADCSetting           (void);
  extern void ADSamping            (void);//AD��������
                                  
  //Uart ������                
  extern void LinUartInit          (void);
  extern void LinUart0Init         (void);
                                  
  //extern void DetectPowerIn        (void);
  extern void PowerInScan          (void);
                                  
  extern void Dis_no_power         (void);
  
  extern void ErrorDisposal        (void);
  extern u8   RFSignalDecode       (u16 High_Time,u16 Low_Time);//RF���뺯��
  extern u8   RFDataVerify         (void);//RF����У��
  extern void RFDataDisposal       (void);//RF������
  extern void RFStatusInspect      (void);
                                  
  extern u8   CompID               (u8 array[]);//�Ƚ��Ƿ�����ͬID
  extern u8   SaveID               (u8 array[]);//�����µ�ID
  extern void DeleteAllID          (u8 check);//ɾ�����е�ID
  extern u8   DeleteChannel        (u8 array[],u8 ch_addr);//ɾ��ͨ��
  extern u8   SaveChannel          (u8 array[],u8 ch_addr);//����ͨ��
                                  
  extern void MotorControl         (void);//�������
  extern void MotorStatusSwitch    (void);//���״̬�л�
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
  
  /*************HUAWEIģ����ֲ**************/
  extern u8 JourneyPercentCount (void);
  extern u32 CountPercentPlace (u8 percent);
  extern void UartControl_WIFI (u8 data);                   //�������  HUAWEIģ��
  // Uart ������
  extern void LinUartInit (void);
  extern void UartDataDisposal (void);
  extern void UartStartupTX (void);
  extern void UartControl (u8 data);
  extern void Dispose_All_Date_For_TX(void);                //�ϴ�����״̬�����ݴ���
  extern void Angle_calculation(void);                                   /* �Ƕȼ��㺯�� ----*///huang
  extern void UART_TEXT(u8 DATE_1,u8 DATE_2,u8 DATE_3,u8 DATE_4,u8 DATE_5,u8 DATE_6);               //�ϴ�����״̬�����ݴ���
  extern void JourneyDelete(void);
  extern void Motor_Data_Change_Send(void);                //           �����ϱ�

  extern void DetectPowerIn (void);
  extern void Send_Data(u8 TX_Times,u8 TxKind,u8 Tx_total,u8 Tx_first);
#endif
