/**********************************************************************************
 *  ��Ŀ���:
 *  ��Ŀ����:
 *  �ͻ�����:                                    
 *  �����д:
 *  ��������:
 *  �������:    
 *  �汾����:    V1.0
 
 *  Initialize.C file
 *  ��������: ϵͳ��ʼ��
 *********************************************************************************/
#include "Define.h"//ȫ�ֺ궨��
#include "Extern.h"//ȫ�ֱ���������

//---------------------------------------------------------------------------------
//  ������:  CompID
//  ��  ��:   
//---------------------------------------------------------------------------------
u8 CompID (u8 data[])
{
  u8 i;
  u8 flag = 0;
  
  for (i = 0; i < RF_ID_Amount; i++)//ɨ��ID����
  {
    //�Ƿ�����ͬ��ID
    if (RF_ID_Buffer[i] [0] == data[1])                               
    {
      if (RF_ID_Buffer[i] [1] == data[2])
      {
        if (RF_ID_Buffer[i] [2] == data[3])
        {
          //���浱ǰID���ڵ����
          flag |= i + 1;
          // ����ͨ������1���ֽ��е�1λͬʱΪ1ʱ˵���Ѿ��Ժ���
          if ((RF_ID_Buffer[i] [3] & data[4])|| 
              (RF_ID_Buffer[i] [4] & data[5]))//�Ƿ�����ͬ��ͨ��
          {
            flag |= 0x80;//����ͬͨ����־
            break;
          }
        }
      }
    }
  }
  return (flag);
}
//---------------------------------------------------------------------------------
//  ������:   SEAVE_ID
//  ��  ��:   
//---------------------------------------------------------------------------------
u8 SaveID (u8 data[])
{
  u8 i;
  u8 flag = 0;
  //�Ƿ�û�д���
  if (RF_ID_Amount <= ID_MAX_AMOUNT)
  {
    RF_ID_Amount++;
  }
  //����
  for(i = 0; i < ID_LENGTH; i++)
  {
    RF_ID_Buffer[RF_ID_Save_Place] [i] = data[i+1];
  }
  flag = TRUE;
  //�Ƿ����
  if (++RF_ID_Save_Place >= ID_MAX_AMOUNT)
  {
    RF_ID_Save_Place = 9;
  }
  return (flag);
}
//---------------------------------------------------------------------------------
//  ������:  	SaveChannel
//  ��  ��:   
//---------------------------------------------------------------------------------
u8 SaveChannel (u8 array[],u8 ch_addr)
{
  u8 flag = 0;
  
  ch_addr &= 0x1f;
  ch_addr--;
  
  if (ch_addr < RF_ID_Amount)
  {
    //���ͨ��
    RF_ID_Buffer[ch_addr] [3] |= array[4];
    RF_ID_Buffer[ch_addr] [4] |= array[5];
    flag = TRUE;
  }
  return (flag);
}
//---------------------------------------------------------------------------------
//  ������:  	DeleteSingleChannel
//  ��  ��:   
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
    // ɾ��ͨ��
    RF_ID_Buffer[ch_addr] [3] &= ~array[4];
    RF_ID_Buffer[ch_addr] [4] &= ~array[5];
    // ͨ���Ƿ����0
    if ((RF_ID_Buffer[ch_addr] [3] == 0) &&  (RF_ID_Buffer[ch_addr] [4] == 0))
    {
      // �ؿ��Ź�
      WDTCTL = WDTPW + WDTHOLD;
    
      // ɾ��ID,�������ǰŲ
      for(i = ch_addr; i < RF_ID_Amount; i++)
      {
        for(j = 0; j < ID_LENGTH; j++)
        {
          RF_ID_Buffer[i] [j] = RF_ID_Buffer[i+1] [j];
        }
      }
      //ID������һ
      if (RF_ID_Save_Place > 0)
      {
        RF_ID_Save_Place--;
      }
      //ID������һ
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
    Restrict_Up = 3;                                              //ɾ����λ
}


//---------------------------------------------------------------------------------
//  ������:  	DeleteAllID
//  ��  ��:   
//---------------------------------------------------------------------------------
void DeleteAllID (u8 check)
{
  static u8 i;
  static u8 j;
  
  if (check == 0xee)
  {
    _DINT();//�����ж� 
    WDTCTL = WDTPW + WDTHOLD;//�ؿ��Ź�
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
//    Restrict_Up = 3;                                              //ɾ����λ
//
//    //---------------------------------------------------------------------------------
    

    
    JourneyDelete();
    //ɾ������ID
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
    Flag.Delete_Code=0;//ɾ��ȫ����ɣ�����ɾ����־λ
    MotorFlag.Motor_Jiggle_Status=0;//����㶯���б�־
    MotorFlag.Direction=0;//�����־
    Joint_Time_Level=4;//�㶯�Ƕ�
    Sweet_Spot = 0;
    /*******��Ҷ����ʼ��******/   
    if(Flag.Angle_mark == 1)
    {
      Hall_Pulse_Amount_up = (Hall_difference + Hall_Pulse_Amount_Cnt);//
    }     
    else //��180��Ϊ���
    {
      Hall_Pulse_Amount_up = (Hall_Pulse_Amount_Cnt - Hall_difference );//
      
    }
    Send_Data(1,0x06,10,2);
    //Hall_Pulse_Amount_up = Hall_Pulse_Amount_Cnt ;//�洢�µ�һ��0��ʱ��λ��
    Flag.Control_Tighten_Up = NULL ;//Ĭ�ϴ��ս�����
    Flag.Up_Meet_Plugb = NULL;//���������־�������20250217ry
    _EINT();//�����ж� 
  }
}