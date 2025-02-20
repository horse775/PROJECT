/**********************************************************************************
 *  ��Ŀ���:    
 *  ��Ŀ����:    
 *  �ͻ�����:                                    
 *  �����д:    
 *  ��������: 
 *  �������:    
 *  �汾����:    V1.0
 *
 *  .c file
 *  ��������: 
**********************************************************************************/
#include "Define.h"//ȫ�ֺ궨��
#include "Extern.h"//ȫ�ֱ���������
//�ڲ��� -------------------------------------------------------------------------------
#define TIME0_INT_NUMBER       3//Time0�жϴ���

#define HALL_SR1               BIT6
#define HALL_SR2               BIT7
#define HALL1_IN()            (P3IN & HALL_SR1)//P3.6
#define HALL2_IN()            (P3IN & HALL_SR2)//P3.7

u8 Time2_Int_Cnt = TIME0_INT_NUMBER - 2;

u8 RF_Signal_Sample1;//���ݵ�ƽ����
u8 RF_Signal_Sample2;//���ݵ�ƽ����

u8 High_Time_Cnt;//����ʱ�仺��
u8 Low_Time_Cnt;//����ʱ�仺��
u8 Time_200uS_Cnt;//LIU

//--------------------------------------------------------------------------------------
//  ������:   DetectHallSignal
//  ��  ��:   �������ź�
//  ��  ��:   ������
//  ��  ��:   ���������ȣ����ر�־�����ʵ�����з����־
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
  
  // ����1Ϊ��׼���Ƿ��б��ش���
  if (Hall_Status_Buffer != hall_value)
  {
    // ����3�η�ֹ����
    if (++hall_change_cnt >= 1)
    {
      hall_change_cnt = 0;
      
      // ȷ�����з���
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
      
      // ��ȡ������
      Hall_Pulse_Width = Hall_Time_Cnt;
      Hall_Time_Cnt = 0;
      MotorFlag.Hall_Pulse_edge = TRUE;  
      
      if (MotorFlag.Motor_Fact_Direction ^ MotorFlag.Journey_Direction)
      {
        // �Ƿ������Χ��
        if (Hall_Pulse_Amount_Cnt < HALL_PULSE_MAX)
        {
          ++Hall_Pulse_Amount_Cnt;
        }
      }
      else
      {
        // �Ƿ������Χ��
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
//  ������:  Timer3_B0
//  ����:    Timer3_B0�ж�
//--------------------------------------------------------------------------------------
#pragma vector=TIMER3_B0_VECTOR
__interrupt void Timer3_B0(void)
{
  TB3CCR0 += RF_DECODE_TIME * SYSTEM_CLOCK;//40 uS
  //---------------------------------------------------------------------------------
  // RF�źŲ���
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
  // ����3��ȷ����ƽ״̬�����ж��Ƿ�Ϊ�����ź�
  if (--Time2_Int_Cnt == 0)
  {
    Time2_Int_Cnt = TIME0_INT_NUMBER;
    
    // ��������Ϊ����Ϊ�ߵ�ƽ������Ϊ�͵�ƽ
    if (RF_Signal_Sample1 >> 1)
    {
      // �ϴ�Ϊ�͵�ƽ�����������ź�
      if (!Flag_RF.Signal_Status)
      {
        if (!Flag_RF.Raise_Edge)
        {
          //������ʱ�����3�������е͵�ƽ���򲹳�͵�ƽʱ��
          //�ߵ�ƽʱ������������
          RF_Low_Cnt_Buffer += 3 - RF_Signal_Sample1;//�͵�ƽ�м�����Ӽ�
          // ȡ�������ڸߵ͵�ƽ��ʱ��
          RF_High_Cnt = RF_High_Cnt_Buffer;          
          RF_Low_Cnt = RF_Low_Cnt_Buffer;
          //�͵�ƽ���¼������ߵ�ƽʱ�������¸�����
          RF_Low_Cnt_Buffer = 0;
          RF_High_Cnt_Buffer = RF_Signal_Sample1;
          // ��RF�����ر�־  
          Flag_RF.Raise_Edge = TRUE;
        }
      }
      else
      {
        // ֻҪ3����2�������Ǹߣ����¼3�θߵ�ƽʱ��
        RF_High_Cnt_Buffer += 3;
      }
      Flag_RF.Signal_Status = TRUE;
    }
    else
    {
      // �ϴ�Ϊ�ߵ�ƽ�����½����ź�
      if (Flag_RF.Signal_Status)
      {
        //�½���ʱ�����3�������иߵ�ƽ���򲹳�ߵ�ƽʱ��
        RF_High_Cnt_Buffer += RF_Signal_Sample1;//�ߵ�ƽ�м�����Ӽ�
        RF_Low_Cnt_Buffer += 3 - RF_Signal_Sample1;//�͵�ƽ�м�����Ӽ�
      }
      else
      {
        //ֻҪ3����2�������ǵͣ����¼3�ε͵�ƽʱ��
        RF_Low_Cnt_Buffer += 3;
      }
      Flag_RF.Signal_Status = 0;
    }
    RF_Signal_Sample1 = 0;
  }   
} //�жϽ���������
