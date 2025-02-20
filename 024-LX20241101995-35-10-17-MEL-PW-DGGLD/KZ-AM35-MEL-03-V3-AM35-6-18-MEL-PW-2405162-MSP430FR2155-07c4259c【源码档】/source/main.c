/**********************************************************************************
 *  ��Ŀ���:
 *  ��Ŀ����:
 *  �ͻ�����:
 *  �����д:
 *  ��������:
 *  �������:
 *  �汾����:    V1.0
 *  ��������:
 *  оƬѡ��:
 *  ��Ƶ��:
 *  Watchdog:
 *  MAIN.C file
**********************************************************************************/
#include "Define.h"//ȫ�ֺ궨��
#include "Extern.h"//ȫ�ֱ���������
//--------------------------------------------------------------------------------------
//  ������:  main
//  ��  ��:  
//  ��  ��:  ��
//  ��  ��:  ��
//  ��  ��:  ��
//  ��  ���� 
//--------------------------------------------------------------------------------------
int main (void)
{
  Initialize();//AM35-���ص�Դ-WIFI
  
  while(1)
  {
    VCC_ON();/*�򿪵�Դ*/
    
    if (((!MotorFlag.Motor_Run)   && (!Flag.Del_Code_Finish) &&
         (!Flag.Opretion_Finish)) || (motor_run_time >= NO_VOL_SAMPLING_TIME))//�ܿ��������˲��AD����
    {
      ADSamping();//ad����
    }
    //RF����---------------------------------------------------------------------------
    if (Flag_RF.Raise_Edge)//�Ƿ���������
    {
      Flag_RF.Raise_Edge = NULL;
      if (RFSignalDecode(RF_High_Cnt,RF_Low_Cnt))//RF����
      {
        if (RFDataVerify())//�����Ƿ���ȷ
        {
          RFDataDisposal();//RF���ݴ���
        }
      }
    } 
    //200us -----------------------------------------------------------------------------    
     if (Flag.Time_200us)
     {
       Flag.Time_200us = FALSE;
       //DetectPowerIn(POWER_IN);
       DetectPowerIn();
     }
    //2ms -----------------------------------------------------------------------------
    if (Flag.Time_2ms)
    {
      Flag.Time_2ms = NULL; 
      PowerInScan();             //LIU     
      TimeControl();
      
      JourneyComper();//�Ƚ��г�
      
      KeyManage();//����ɨ�輰����
      
      if(Flag.Uart_tx)
      {
        Flag.Uart_tx = FALSE;
        Uart1StartupTX();//485
      }
      
      if (Flag.Uart_Read_OK)
      {
        Flag.Uart_Read_OK = FALSE;
        Uart1DataDisposal();//485
      }
      Uart1_Status_Inspect();
              
      UartStartupTX();//ͿѻWIFI
      UartDataDisposal();//ͿѻWIFI
    }
    //10ms ----------------------------------------------------------------------------
    if (Flag.Time_10ms)
    {
      Flag.Time_10ms = NULL;
      //����ת�� ----------------------------------------------------------------------
      if (MotorFlag.Hall_Pulse_edge)
      {
        MotorFlag.Hall_Pulse_edge = NULL;
        CountMotorSpeed();//����������ת��
      }
      
      MotorControl();//�������
      MotorStatusSwitch();//���״̬�л�
      

    }
    //50ms ----------------------------------------------------------------------------
    if (Flag.Time_50ms)
    {
      Flag.Time_50ms = NULL;
      RFStatusInspect();
      SetLimitPoint();

      if (Flag.Delete_Code)//�Ƿ�ɾȫ��
      {
        DeleteAllID(0xee);
      }
      
    }
    //100ms ---------------------------------------------------------------------------
    if (Flag.Time_100ms)
    {
      Flag.Time_100ms = NULL;
      
      DblclickTimeLimit();//����λ��˫��
      if(respons_tc) respons_tc--;
  
      if(tx_ok_tc)
      {
        if(--tx_ok_tc==0)
        {
          if(flag_tx)
          {
            if((tx_data != 0x06)||(++tx_data_i > tx_data_n))
            {
              if(--tx_c==0)
              {
                flag_tx=0; 
              }
              else 
              {
                respons_tc=40;
              }
            }
          }
        }
      }
      t50ms_c ++ ;
      if ((!Flag.Del_Code_Finish) && (!Flag.Opretion_Finish))
      {
      if((Motor_Status_bak != Motor.Motor_Status)&&(t50ms_c>=5))//״̬�仯�����ϱ�
      {
        Motor_Status_bak=Motor.Motor_Status;
        if(Motor.Motor_Status == MOTOR_STOP)
        {
          Send_Data(1,0x06,4,2);
          t50ms_c=0;
        }
        else
        {
          Send_Data(1,0x06,2,1);
          t50ms_c=0;
        }
      }
      if((Motor_Flag_Direction_bak!=MotorFlag.Direction)&&(t50ms_c>=5))          //���������ϱ�
      {
        Send_Data(1,0x06, 6,4);
        t50ms_c=0;
        Motor_Flag_Direction_bak=MotorFlag.Direction;
      }
      }
    }
    //1s-------------------------------------------------------------------------------
    if (Flag.Time_1s)
    {
      Flag.Time_1s = NULL;
      Flag.Electrify_Reset = TRUE;              //��ֹ�ϵ����  

      LearnDelCodeTime();
    }
  }
}