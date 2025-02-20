/**********************************************************************************
 *  ��Ŀ���:    
 *  ��Ŀ����:    
 *  �ͻ�����:                                    
 *  �����д:    
 *  ��������:    
 *  �������:    
 *  �汾����:    V1.0
 *
 *  GlobalVariable.c file
 *  ��������: ȫ�ֱ�������
**********************************************************************************/
#include "Define.h"
//ȫ��λ�������� -----------------------------------------------------------------------
struct 
{
  u8 RF_Rx_Data                : 1;  
  u8 RF_Rx_Finish              : 1;//RF�������ݱ�־
  u8 RF_Data_Disposal          : 1;//RF���ݴ����־
  
  u8 Time_200us                : 1;// 200usʱ���־
  u8 Time_2ms                  : 1;// 2msʱ���־
  u8 Time_10ms                 : 1;// 10msʱ���־
  u8 Time_50ms                 : 1;// 50msʱ���־
  u8 Time_100ms                : 1;// 100msʱ���־
  u8 Time_500ms                : 1;// 500msʱ���־
  u8 Time_1s                   : 1;// 1sʱ���־
  //-----------------------------------------------------------------------------------
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
//-------------------------------------------------------------------------------------
struct 
{
  u8 Signal_Status             : 1;//RF��ƽ״̬��־
  u8 Key_Disposal              : 1;//RF�����־
  u8 Signal_Head               : 1;//RF�ź�ͷ��־
  u8 Raise_Edge                : 1;//RF�����ر�־
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


 struct
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

__no_init u16 Hall_Pulse_Width;//�������������
__no_init u32 Hall_Pulse_Amount_Cnt;//��������������

u8 Motor_Status_Buffer_1;//huang
u32 Click_the_Angle_of_hall;//ת���ǶȵĻ���ֵhuang
__no_init u8 Current_Angle;                                   /*��ǰ�Ƕ�*/
 //__no_init u8  Hall_Pulse_Amount_stop;                          /* �������������� */
 __no_init u32  Hall_Pulse_Amount_up;                          /* �������������� */  
 //__no_init u32  Hall_Pulse_Amount_down;                          /* �������������� */ 
 __no_init u32  Hall_difference;//������ֵ huang 
 __no_init u8 Restrict_Up ;                              /* ����λ�����ñ�־ */

 


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
u8 RF_High_Cnt;//RF�ߵ�ƽʱ��
u8 RF_Low_Cnt;//RF�׵�ƽʱ��
//u8 Motor_Status_bak;

u32 Hall_Pulse_Amount_PCT;//��������������
u32 OprationCue_Amount_Buf;//��������������
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

u8  Motor_Speed;//���ת��
u8  Target_Speed;
u16 Motor_Duty;

u8 Set_Limit_Point_Cnt;

u16 Time_1s_Hint_Cnt;
u8 Delete_single_Code_Cnt;
u8 PowerDown_Delay_Time;

u16 Key_Wakeup_time_cnt;

u8 RF_High_Cnt_Buffer;//����ʱ�仺��
u8 RF_Low_Cnt_Buffer;//����ʱ�仺��
Motor_Control_TypeDef Joint_Action_Motor_Buf;
u16 Joint_Action_Time_Cnt;
u16 Joint_Level_ADJ_Time;

u8 No_ac_power;
u8 No_ac_power_Cnt;

u16 Current_Current_mA;
u8   t50ms_c;                   //liu
u8 Meet_Plugb_Rebound_Time;//����ʱ�䣨������λ����1S��2025-2-17

/**************HUAWEIģ����ֲ***************/
u8  flag_tx,//�����������ͱ�־
    flag_rx,//���ڽ�����ɱ�־
    rx_tc,//���ռ����ʱ��������40ms��Ϊ���ս�����
    respons_tc,//������Ӧ��ʱ����400ms��Ϊ����Ӧ)
    tx_c,//����Ӧ�ظ����ʹ���
    red_led_tc,//���״̬ת��
    red_led_tc2,
    test_tx_tc,
    test_fail_tc,
    Motor_Status_bak,//
    Motor_Flag_Direction_bak,//��һ��Ĭ�Ϸ���
    tx_ok_tc,//���һ���ֽڷ�����ɺ���ʱ4ms�л�RS485_RX
    JourneyPercentCount_bak;//��һ���ٷֱ�
u8  tx_data,tx_data_i,tx_data_n;//����������
u8  flag_auto_test,
    flag_power_on2,
    flag_power_on;

 u8 Motor_Status_Buffer_1;//huang
 u32 Click_the_Angle_of_hall;//ת���ǶȵĻ���ֵhuang 
 u8 SendTX_interval_Time;//���ڼ��ʱ��
 u8 Time_Power_On;//�ϵ��־
 u8 learn_key_cnt;            //   �����������
 u8 Current_Angle_5;            //   ����5
 u8 Hall_Pulse_Amount_up_buf;            //����
 
 u8 TEXT_1;            //����
  u8 TEXT_2;            //����
   u8 TEXT_3;            //����
   
 u8 set_key_cnt;
 u8 Set_Key_Time_Cnt;
