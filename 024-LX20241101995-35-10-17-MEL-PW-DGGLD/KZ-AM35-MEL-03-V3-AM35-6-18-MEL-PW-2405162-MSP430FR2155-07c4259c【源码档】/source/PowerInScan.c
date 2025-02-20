///**********************************************************************************
// *  ��Ŀ���: 
// *  ��Ŀ����: 
// *  �ͻ�����:                                    
// *  �����д: 
// *  ��������:    
// *  �������:    
// *  �汾����:    V1.0
// *
// *  PowerInScan.c file
// *  ��������: 
//**********************************************************************************/
//#include "Define.h"                                          /* ȫ�ֺ궨�� ------*/
//#include "Extern.h"                                          /* ȫ�ֱ��������� --*/
////-------------------------------------------------------------------------------------
//u16 Power_In_Cnt;
//u16 No_Power_In_Cnt;
////-------------------------------------------------------------------------------------
////-------------------------------------------------------------------------------------
//void DetectPowerIn (void)
//{
//  if(!SYSTEM_POWER_IN())
//  {
//    if(++Power_In_Cnt>=100)
//    {
//      No_ac_power=0;
//      No_Power_In_Cnt=0;
//      //-------------------------------------------------------------------------------
//      if(Power_In_Cnt>=65530)
//      {
//        Power_In_Cnt=0;
//      }
//    }
//  }
//  else
//  {
//    if(++No_Power_In_Cnt>=800)
//    {
//      No_ac_power=1;
//      Power_In_Cnt=0;
//      No_Power_In_Cnt=0;
//    }
//  }
//}
////-------------------------------------------------------------------------------------
////-------------------------------------------------------------------------------------
//void PowerInScan (void)
//{
//  if(No_ac_power==1)
//  {
//      
//  }
//}







/**********************************************************************************
 *  ��Ŀ���:    AC235-01
 *  ��Ŀ����:    ���������������
 *  �ͻ�����:                                    
 *  �����д:    ��С��
 *  ��������:    2013-01-31
 *  �������:    
 *  �汾����:    V1.1
 *
 *  PowerInScan.c file
 *  ��������: 
**********************************************************************************/
#include "Define.h"                                          /* ȫ�ֺ궨�� ------*/
#include "Extern.h"                                          /* ȫ�ֱ��������� --*/
/* �ڲ��� -----------------------------------------------------------------------*/
#define POWER_IN_TIME           (40   / 20)  // 100mS
#define PROGRAM_TIME            (800  / 20)  // 800mS
#define DELETE_TIME             (7000 / 20)  // 7S

/* �ڲ����� ---------------------------------------------------------------------*/
void UpKeyIn       (void);
void DownKeyIn     (void);
void ProgramKeyIn  (void);
void EmptyFunction (void);
void ProgramKey10s (void);


Hall Power_Array[POWER_IN_NUM];// = 
//{
//  {0,0,0,POWER_UP_IN,    0,0,0},
//  {0,0,0,POWER_DOWN_IN,  0,0,0},
//  {0,0,0,SYSTEM_POWER_IO,0,0,0}
//};

u16 Power_In_Cnt;
u16 Power_Up_In_Cnt;
u16 Power_Down_In_Cnt;
u16 Power_UpDown_In_Cnt;
u16 No_Power_In_Cnt;
u16 Power_In_Syc_Cnt[POWER_IN_NUM];

/**********************************************************************************
  ������:   EmptyFunction
  ��  ��:   
 *********************************************************************************/
void EmptyFunction (void)
{
  return;
}
/**********************************************************************************
  ������:   UpKeyIn
  ��  ��:   
 *********************************************************************************/
void UpKeyIn (void)
{
    Motor.Motor_Status = MOTOR_UP;
    Flag.Time_50ms = TRUE;
}
/**********************************************************************************
  ������:   DownKeyIn
  ��  ��:   
 *********************************************************************************/
void DownKeyIn (void)
{
    Motor.Motor_Status = MOTOR_DOWN;
    Flag.Time_50ms = TRUE;
}
/**********************************************************************************
  ������:   ProgramKeyIn
  ��  ��:   
 *********************************************************************************/
void ProgramKeyIn (void)
{
  Motor.Motor_Status = MOTOR_STOP;
  MotorFlag.Motor_Run= NULL;
  
  Power_UpDown_In_Cnt = 0;
  Power_Up_In_Cnt = 0;
  Power_Down_In_Cnt = 0;
}
///*********************************************************************************
//  ������:   
//  ��  ��:   
//  ��  ��:   
//  ��  ��:   
//  ��  ��:	  
//  ��  ����  
//*********************************************************************************/
//void DetectPowerIn (u8 hall_num)
//{
//  u8 temp;
//  
//  //temp = CONTROL_UP_IN();
//  //temp |= CONTROL_DOWN_IN();
//  temp = SYSTEM_POWER_IN();
//  
//  if (Power_Array[hall_num].time_cnt < 65000)          // ����ʱ���ۼ�
//  {
//    ++Power_Array[hall_num].time_cnt;
//  }
//  
//  if (Power_In_Syc_Cnt[hall_num] < 65000)          // ����ʱ���ۼ�
//  {
//    ++Power_In_Syc_Cnt[hall_num];
//  }
//  
//  
//  if (temp & Power_Array[hall_num].HALL)
//  {
//    if (Power_Array[hall_num].vol_status)  // ��ƽ�仯
//    {
//      if (Power_Array[hall_num].time_cnt > (5000 / (TIME_200US_VALUE * RF_DECODE_TIME))) //�����Ƿ���� 6MS
//      {
//        if (Power_Array[hall_num].time_cnt < (15000 / (TIME_200US_VALUE * RF_DECODE_TIME))) //�����Ƿ�С�� 13MS
//        {
//          Power_Array[hall_num].time_cnt = 0;                // ���¼�������ʱ��
//        }
//        else
//        {
//          Power_In_Syc_Cnt[hall_num] = 0;
//        }
//      }
//      else
//      {
//        Power_In_Syc_Cnt[hall_num] = 0;
//      }
//    }
//      
//    Power_Array[hall_num].vol_status = FALSE;            // ��ǰ��ƽΪ0
//  }
//  else
//  {
//    if (!Power_Array[hall_num].vol_status)
//    {
//      if (Power_Array[hall_num].time_cnt > (5000 / (TIME_200US_VALUE * RF_DECODE_TIME)))
//      {
//        if (Power_Array[hall_num].time_cnt < (15000 / (TIME_200US_VALUE * RF_DECODE_TIME)))
//        {
//          if (Power_In_Syc_Cnt[hall_num] > (13000 / (TIME_200US_VALUE * RF_DECODE_TIME)))
//          {
//            if (Power_In_Syc_Cnt[hall_num] < (24000 / (TIME_200US_VALUE * RF_DECODE_TIME)))
//            {
//              Power_Array[hall_num].edge = TRUE;               // �������ر�־
//              Power_Array[hall_num].edge_change = TRUE;
//              
//              if ((Power_In_Syc_Cnt[POWER_UP] > (Power_In_Syc_Cnt[POWER_DOWN] >> 2)) 
//              && (Power_In_Syc_Cnt[POWER_UP] < ((Power_In_Syc_Cnt[POWER_DOWN] >> 1) + (2000 / (TIME_200US_VALUE * RF_DECODE_TIME)))))
//              {
//                Power_Array[POWER_UP].edge = FALSE;
//                Power_Array[POWER_UP].edge_change = FALSE;
//              }
//              
//              if ((Power_In_Syc_Cnt[POWER_DOWN] > (Power_In_Syc_Cnt[POWER_UP] >> 2))
//              && (Power_In_Syc_Cnt[POWER_DOWN] < ((Power_In_Syc_Cnt[POWER_UP] >> 1) + (2000 / (TIME_200US_VALUE * RF_DECODE_TIME)))))
//              {
//                Power_Array[POWER_DOWN].edge = FALSE;
//                Power_Array[POWER_DOWN].edge_change = FALSE;
//              }
//              
//            }
//          }
//        }
//      }
//      Power_In_Syc_Cnt[hall_num] = 0;
//      Power_Array[hall_num].time_cnt = 0;                // ���¼�������ʱ��
//    }
//    Power_Array[hall_num].vol_status = TRUE;
//  }
//}

/*********************************************************************************
  ������:   
  ��  ��:   
  ��  ��:   
  ��  ��:   
  ��  ��:	  
  ��  ����  
*********************************************************************************/
void DetectPowerIn (void)   //2024.1.23 �Ż���Դ�źż�ⲿ��
{
  if(Power_Array[POWER_IN].Buf != POWER_DETECT())
  {
    if(++Power_Array[POWER_IN].Cnt>=3)
    {
      Power_Array[POWER_IN].Cnt = 0;
      Power_Array[POWER_IN].Buf = POWER_DETECT();
      if(Power_Array[POWER_IN].Buf)
        Power_Array[POWER_IN].edge = TRUE;
    }
  }
  else
    Power_Array[POWER_IN].Cnt = 0;
  
  if(Power_Array[POWER_UP].Buf != CONTROL_UP_IN())
  {
    if(++Power_Array[POWER_UP].Cnt>=3)
    {
      Power_Array[POWER_UP].Cnt = 0;
      Power_Array[POWER_UP].Buf = CONTROL_UP_IN();
      if(Power_Array[POWER_UP].Buf)
        Power_Array[POWER_UP].edge = TRUE;
    }
  }
  else
    Power_Array[POWER_UP].Cnt = 0;
  
  if(Power_Array[POWER_DOWN].Buf != CONTROL_DOWN_IN())
  {
    if(++Power_Array[POWER_DOWN].Cnt>=3)
    {
      Power_Array[POWER_DOWN].Cnt = 0;
      Power_Array[POWER_DOWN].Buf = CONTROL_DOWN_IN();
      if(Power_Array[POWER_DOWN].Buf)
        Power_Array[POWER_DOWN].edge = TRUE;
    }
  }
  else
    Power_Array[POWER_DOWN].Cnt = 0;
}

///**********************************************************************************
//  ������:   VoltageProtect
//  ��  ��:   
// *********************************************************************************/
//void PowerDownSave (void)
//{
//  if ((Motor_Array[0].Motor_Flag.SaveEE) && ((Motor_Array[0].Motor_Flag.Journey_Set_OK) || (Motor_Array[0].Motor_Flag.Single_Journey)))
//  {
//    if (Motor_Array[0].Motor_Flag.PowerDown_Remember)  // �ϵ��Ƿ���Ҫ����
//    {
//      FlashWritePlace();
//    }
//  }
//  Motor_Array[0].Motor_Flag.SaveEE = NULL;
//}
/**********************************************************************************
  ������:   PowerInScan
  ��  ��:   �����·���룬ĳ·�������źź���Ƶ��״̬��ֻ����һ�Ρ�
            ����·��û���źź����ѿ��Ʊ�־���������¿��Ƶ��״̬��
  ��  ��:   ���״̬�Ĵ���
 *********************************************************************************/
void PowerInScan (void)
{
  static u8 Power_Detect_Cnt;
  
  if ((Power_Array[POWER_UP].edge) || (Power_Array[POWER_DOWN].edge) || (Power_Array[POWER_IN].edge))  // �Ƿ����ź�
  {
    if (++Power_Detect_Cnt >= 2)
    {
      Power_Detect_Cnt = 0;
      
      if ((Power_Array[POWER_UP].edge) && (Power_Array[POWER_DOWN].edge)) // �Ƿ�ͬʱ���ź�
      {
        if (Power_UpDown_In_Cnt < 65530)
        {
          Power_UpDown_In_Cnt++;
        }
        if (Power_UpDown_In_Cnt == PROGRAM_TIME)
        {
          ProgramKeyIn();
          Power_Up_In_Cnt = 0;
          Power_Down_In_Cnt = 0;
          Power_In_Cnt = 0;
        }
      }
      else if (Power_Array[POWER_UP].edge)    // 
      {
        if (Power_Up_In_Cnt < 65530)
        {
          Power_Up_In_Cnt++;
        }
        if (Power_Up_In_Cnt == POWER_IN_TIME)
        {
          UpKeyIn();
          Power_UpDown_In_Cnt = 0;
          Power_Down_In_Cnt = 0;
          Power_In_Cnt = 0;
        }
      }
      else if (Power_Array[POWER_DOWN].edge)  // 
      {
        if (Power_Down_In_Cnt < 65530)
        {
          Power_Down_In_Cnt++;
        }
        if (Power_Down_In_Cnt == POWER_IN_TIME)
        {
          Power_UpDown_In_Cnt = 0;
          Power_Up_In_Cnt = 0;
          Power_In_Cnt = 0;
          DownKeyIn();
        }
      }
      else if (Power_Array[POWER_IN].edge)  // 
      {
        if (Power_In_Cnt < 65530)
        {
          Power_In_Cnt++;
        }
        if ((Power_Up_In_Cnt >= POWER_IN_TIME) || (Power_Down_In_Cnt >= POWER_IN_TIME) || (Power_UpDown_In_Cnt >= PROGRAM_TIME))
        {
          if ((Power_In_Cnt > (80 / 20)) && (Power_In_Cnt < (300 / 20)))    // 80mS - 300mS
          {
            Motor.Motor_Status = MOTOR_STOP;
            MotorFlag.Motor_Run = NULL;
            
          //  Motor_Array[0].RunTime_Cnt = NULL;
            motor_run_time = 0;
            Flag.StatusSwitch_Delay  = TRUE;
          }
//          else if (Power_In_Cnt > (300 / 20))    // 80mS - 300mS
//          {
//            Motor.Motor_Status = MOTOR_STOP;
//             Motor.Motor_Status_Buffer = MOTOR_STOP;
//            MotorFlag.Motor_Run = NULL;
//            
//            Power_UpDown_In_Cnt = 0;
//            Power_Up_In_Cnt = 0;
//            Power_Down_In_Cnt = 0;
//          }
        }
      }
      
      No_Power_In_Cnt = 0;
      Power_Array[POWER_UP].edge = FALSE;
      Power_Array[POWER_DOWN].edge = FALSE;
      Power_Array[POWER_IN].edge = FALSE;
    }
  }
  else   // ���ź�����
  {
    if (No_Power_In_Cnt < 65530)
    {
      No_Power_In_Cnt++;
    }
    
    if (No_Power_In_Cnt >= (50 / SYSTEM_TIME))
    {
      Motor.Motor_Status = MOTOR_STOP;
       Motor.Motor_Status_Buffer = MOTOR_STOP;
      MotorFlag.Motor_Run = NULL;
            Time_10ms_Cnt = TIME_10MS_VALUE;

      Power_UpDown_In_Cnt = 0;
      Power_Up_In_Cnt = 0;
      Power_Down_In_Cnt = 0;
      Power_In_Cnt = 0;
    }
    if (No_Power_In_Cnt == (150 / SYSTEM_TIME))
    {
      Motor.Motor_Status = MOTOR_STOP;
       Motor.Motor_Status_Buffer = MOTOR_STOP;
      MotorFlag.Motor_Run = NULL;
           Time_10ms_Cnt = TIME_10MS_VALUE;
 
      Power_UpDown_In_Cnt = 0;
      Power_Up_In_Cnt = 0;
      Power_Down_In_Cnt = 0;
      Power_In_Cnt = 0;
    }
  }
}