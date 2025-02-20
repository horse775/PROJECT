/**********************************************************************************
 *  ��Ŀ���:    
 *  ��Ŀ����:    
 *  �ͻ�����:                                    
 *  �����д:    
 *  ��������:    
 *  �������:    
 *  �汾����:    V1.0
 *  
 *  MotorControl.c file
 *  ��������: 
**********************************************************************************/
#include "Define.h"                                          /* ȫ�ֺ궨�� ------*/
#include "Extern.h"                                          /* ȫ�ֱ��������� --*/
/* �ڲ��� -----------------------------------------------------------------------*/
#define NO_SPEED_TIME          (1000 / (TIME_10MS_VALUE * SYSTEM_TIME))//��ת�ٱ���ʱ��
#define OVERLOAD_TIME          (500  / (TIME_10MS_VALUE * SYSTEM_TIME))//���س�����ʱ��
#define SLOW_SPEED_TIME        (300  / (TIME_10MS_VALUE * SYSTEM_TIME))

#define INCHING_ANGLE             15*3// ΢���ĽǶ�
#define INCHING_ANGLE_TURNS       (REDUCTION_RATIO/28*INCHING_ANGLE/180+1)//���ת���ٱȵĻ����������������ת��Ȧ180��
#define JIGGLE_ANGLE_TURNS(var)   (REDUCTION_RATIO/28*(var*3)/180)//�㶯�Ƕ�20��//(((((var) * 3) * 10) + 7) / 14)


static volatile u16 Meet_Plugb_Cnt;
static volatile u16 Meet_Plugb_Cnt1;
static volatile u16 Meet_Plugb_Cnt2;
static volatile u16 Meet_Plugb_Cnt3;

static volatile u32 Hall_Pulse_Amount_Buff;

static volatile u16 Meet_Plugb_Cnt;
static volatile u16 Motor_RunTime_Cnt = MOTOR_RUN_TIME;
static volatile u16 Current_Current_mA_Buff;
static u16 Meet_Plugb_Rebound_Time_Cnt;

u8 Motor_Speed_Buff;
u8 speed_control_cnt;//�����ٶȵ�ʱ��
u8 curr_over_on_cnt;//����ʱ�������
u8 curr_over_off_cnt;//����ʱ�������

static u16 ADC_CUR_Array[4];
static u8 ADC_CUR_Array_Index;

//�ڲ�����------------------------------------------------------------------------------
void MotorStatusSwitch (void);
u8 MeetPlugbDetect (void);
void SlowStop (u32 target);

//-------------------------------------------------------------------------------------
//  ������:   CountMotorSpeed
//  ��  ��:   
//-------------------------------------------------------------------------------------
void CountMotorSpeed (void)
{
  if (Hall_Pulse_Width < 35)
  {
    Motor_Speed = 200;
  }
  else if (Hall_Pulse_Width > 15000)
  {
    Motor_Speed = 0;
  }
  else if (Hall_Pulse_Width > 337)
  {
    Motor_Speed = ((u8) SPEED_COUNT(Hall_Pulse_Width));//����ÿ���ӵ�ת��
  }
  else
  {
    ADC_CUR_Array[ADC_CUR_Array_Index] = Hall_Pulse_Width;
    ADC_CUR_Array_Index++;
    if (ADC_CUR_Array_Index >> 2)//�Ƿ���4��
    {
      u16 temp;
      
      temp = SampleAverage(ADC_CUR_Array,ADC_CUR_Array_Index);
      ADC_CUR_Array_Index = NULL;
    
      Motor_Speed = ((u8) SPEED_COUNT(temp));//����ÿ���ӵ�ת��
    }
  }
}
//-------------------------------------------------------------------------------------
//  ������:   MeetPlugbDetect
//  ��  ��:   
//-------------------------------------------------------------------------------------
u8 MeetPlugbDetect (void)
{
  u8 flag = NULL;
  
  //����һ����ת��
  if (Motor_Speed == 0)
  {
    if (++Meet_Plugb_Cnt >= NO_SPEED_TIME)
    {
      Meet_Plugb_Cnt = 0;
      flag = TRUE;
    }
  }
  else
  {
    Meet_Plugb_Cnt = 0;
  }
  //-----------------------------------------------------------------------------------
  //0.5���ٶȺܵͲ��ҵ�����
  if (Motor_Speed <= MIN_SPEED)
  {
    if (Current_Current_mA >= (STARTUP_CURRENT+(Meet_Plugb_Level*50)))
    {
      if (++Meet_Plugb_Cnt1 >= OVERLOAD_TIME)
      {
        Meet_Plugb_Cnt1 = 0;
        flag = TRUE;
      }
    }
  }
  else
  {
    Meet_Plugb_Cnt1 = 0;
  }
  //-----------------------------------------------------------------------------------
  //0.3���������ת����
  if (++Meet_Plugb_Cnt2 >= SLOW_SPEED_TIME)
  {
    Meet_Plugb_Cnt2 = NULL;
    
    if (Current_Current_mA >= (STARTUP_CURRENT+(Meet_Plugb_Level*50)))
    {
      if ((Hall_Pulse_Amount_Cnt < (Hall_Pulse_Amount_Buff + 4))
          && (Hall_Pulse_Amount_Cnt > (Hall_Pulse_Amount_Buff - 4)))
      {
        flag = TRUE;
      }
    }
    Hall_Pulse_Amount_Buff = Hall_Pulse_Amount_Cnt;
  }
  //-----------------------------------------------------------------------------------
  if (!MOTOR_NFAULT())
  {
    if (++Meet_Plugb_Cnt3 >= SLOW_SPEED_TIME)
    {
      Meet_Plugb_Cnt3 = NULL;
      flag = TRUE;
    }
  }
  else
  {
    Meet_Plugb_Cnt3 = NULL;
  }
  //-----------------------------------------------------------------------------------
  return (flag);
}
//-------------------------------------------------------------------------------------
//  ������:   SlowStop
//  ��  ��:   ��ͣ
//-------------------------------------------------------------------------------------
void SlowStop (u32 target)
{
#define NUM     ((REDUCTION_RATIO/10)<320?(REDUCTION_RATIO/10):320)
  u16 temp;

  if ((Hall_Pulse_Amount_Cnt < (target + NUM)) && (Hall_Pulse_Amount_Cnt > (target - NUM)))
  {
    //Flag.slowstop = TRUE;   
    //(Motor_Array[0].Motor_Flag.Run_Direction ^ Motor_Array[0].Motor_Flag.Journey_Direction)
    if(target > Hall_Pulse_Amount_Cnt)
    {
      temp = target - Hall_Pulse_Amount_Cnt;
    }
    else
    {
      temp = Hall_Pulse_Amount_Cnt - target;
    }
    
    temp = (MAX_SPEED-MIN_SPEED) * temp / NUM + MIN_SPEED+2;
    if(Target_Speed > temp)
      Target_Speed = temp;
  }
  else
  {
    Flag.slowstop = NULL;
  }
}

/***************************************************************************
*��װ�������������Χ�жϺ���--20250218ry
*  ��  ��:   ��
//  ��  ��:   ��
//  ��  ��:   ��
//  ��  ����
***************************************************************************/
int IsHallPulseOutOfRange(void)
{
    if (!(MotorFlag.Motor_Direction ^ MotorFlag.Journey_Direction))
    {
        if (Hall_Pulse_Amount_Cnt < (HALL_PULSE_MIN + 100))//������������Ƿ񳬳����趨����С��Χ
        {
            return 1;
        }
    }
    else
    {
        if (Hall_Pulse_Amount_Cnt > (HALL_PULSE_MAX - 100))//������������Ƿ񳬳����趨�����Χ
        {
            return 1;
        }
    }
    return 0;
}
/***************************************************************************
//  ������:   JourneyComper
//  ��  ��:   
//  ��  ��:   ��
//  ��  ��:   ��
//  ��  ��:   ��
//  ��  ����  
***************************************************************************/
void JourneyComper (void)
{
  u8 flag_stop = NULL;
  u8 flag_up_limit=NULL;
  
  //�Ƿ��趨����λ��
  if ((MotorFlag.Motor_Run) && (Limit_Point[0].Flag.F1.Limit_Activate))
  {
    //����λ������ʱ2025-2-17
     if(Meet_Plugb_Rebound_Time != 0)
    {
      // ���跴����־��1
      if(Flag.Meet_Plugb_Rebound)
      {
        if(Meet_Plugb_Rebound_Time != 0xff)                   // λ0xff������ʱ��
        {
          if(++Meet_Plugb_Rebound_Time_Cnt  >= ((100 / SYSTEM_TIME) * Meet_Plugb_Rebound_Time))   //���跴����λ0.1��
          {
            flag_stop = TRUE;
            Flag.Meet_Plugb_Rebound = NULL;//clear flag
            Meet_Plugb_Rebound_Time_Cnt = 0;//���ü�����
          }
        }
      }
    }
    else
    {
      Flag.Meet_Plugb_Rebound = 0;//��� Meet_Plugb_Rebound_Time Ϊ 0������� Flag.Meet_Plugb_Rebound ��־
    }
    //�������������Χ�ж�
    if(IsHallPulseOutOfRange())
    {
      flag_stop = TRUE;//ֹͣ����
    }
    
    if ((!Flag.NO_Comper_Limit_Point) && (!Limit_Point[0].Flag.F1.Fine_Adjust))//�������λ&&û��΢����λ��
    {
      //�Ƿ��ڵ�һ����λ������
      if (Hall_Pulse_Amount_Cnt < Limit_Point[0].Limit_Place+1)
      {
        //�Ƿ�Ҫ��������
        if (!(MotorFlag.Motor_Direction ^ MotorFlag.Journey_Direction))
        {
          flag_stop = TRUE;//ֹͣ����
        }
      }
      else if (Run_To_Mid_Limit_Point != NULL)               // �Ƿ�Ҫ���е��м���λ��
      {
        if ((Hall_Pulse_Amount_Cnt > (Limit_Point[Run_To_Mid_Limit_Point].Limit_Place -2))
        && (Hall_Pulse_Amount_Cnt < (Limit_Point[Run_To_Mid_Limit_Point].Limit_Place + 2)))  
        {
          flag_stop = TRUE;
          Run_To_Mid_Limit_Point = NULL;
        }
      }
      else if (Flag.Goto_PCT)//Ŀ��ٷֱ�λ��
      {
        if ((Hall_Pulse_Amount_Cnt > (Hall_Pulse_Amount_PCT - 1))
        && (Hall_Pulse_Amount_Cnt < (Hall_Pulse_Amount_PCT + 1)))
        {
          flag_stop = TRUE;
          Flag.Goto_PCT = FALSE;
          Flag.Angle_Stop = NULL ;//Ŀ��λ�ùص���LIU 
        }
      }

      else 
      {
         //������λ
        if (Motor_Speed !=0 )   //LHD
          SlowStop (Limit_Point[Target_Limit_Point].Limit_Place);
        TEXT_1 = Target_Limit_Point;

        if (Flag.Journey_Arrive)//
        {
          if ((Hall_Pulse_Amount_Cnt > (Limit_Point[Target_Limit_Point].Limit_Place - 2))
          && (Hall_Pulse_Amount_Cnt < (Limit_Point[Target_Limit_Point].Limit_Place + 2)))
          {
            if ((!Flag.Del_Code_Finish) && (!Flag.Opretion_Finish))
            {
              flag_stop = TRUE;
            }
          }
        }
        
        if ((Hall_Pulse_Amount_Cnt > (Limit_Point[Target_Limit_Point].Limit_Place - 2))
        && (Hall_Pulse_Amount_Cnt < (Limit_Point[Target_Limit_Point].Limit_Place + 2)))  
        {
          if ((!Flag.Del_Code_Finish) && (!Flag.Opretion_Finish))
          {
            flag_stop = TRUE;
            Flag.Journey_Arrive = TRUE;
          }
        }
      } 
      //�Ƿ񳬳���������λ��
      if (Hall_Pulse_Amount_Cnt > Limit_Point[Limit_Point_Amount-1].Limit_Place-1)
      {
        //�Ƿ�Ҫ����λ������
        if (MotorFlag.Motor_Direction ^ MotorFlag.Journey_Direction)
        {
          flag_stop = TRUE;
        }
        else
        {
          //������λ�� ������λ
          if (Motor_Speed !=0 )   //LHD
          SlowStop (Limit_Point[Target_Limit_Point].Limit_Place);
          TEXT_2 = 1;//����
        }
      }
      else //20250217ry
      {
        SlowStop (Limit_Point[Target_Limit_Point].Limit_Place);
        if(Flag.Control_Tighten_Up)               //���ս�����
        {
          if ((Hall_Pulse_Amount_Cnt > (Limit_Point[Target_Limit_Point].Limit_Place - 3))
          && (Hall_Pulse_Amount_Cnt < (Limit_Point[Target_Limit_Point].Limit_Place + 3)))  
          {
            if ((!Flag.Del_Code_Finish) && (!Flag.Opretion_Finish))
            {
              
              flag_stop = TRUE; 
              
            }
          }  
        }
        else//���ս�����Flag.Control_Tighten_Up==0
        {
          if ((Hall_Pulse_Amount_Cnt > (Limit_Point[Target_Limit_Point].Limit_Place - 2))
              && (Hall_Pulse_Amount_Cnt < (Limit_Point[Target_Limit_Point].Limit_Place + 2)))  
          {
            if ((!Flag.Del_Code_Finish) && (!Flag.Opretion_Finish))
            {//������λ���·���2024-10-29
              flag_stop = TRUE;
              if(MotorFlag.Journey_Direction != MotorFlag.Direction)//�г̷����־ !=�����־
              {
                if(Target_Limit_Point == 0)
                  flag_up_limit=1;
              }
              else if(Limit_Point_Amount > 1)
              {
                if(Target_Limit_Point == (Limit_Point_Amount - 1))
                  flag_up_limit=1;
              }
            }
          }
        } 
      }
      
    }
    
    //�������������Χ�ж�
    if(IsHallPulseOutOfRange())
    {
      flag_stop = TRUE;//ֹͣflag
    }
  }
  
  if ((Flag.Del_Code_Finish) || (Flag.Opretion_Finish))//ɾ��ɹ���־||�����ɹ���־
  {
    if (Flag.OprationCue_Run)//������ʾ����
    {
      if ((Hall_Pulse_Amount_Cnt > (OprationCue_Amount_Buf + INCHING_ANGLE_TURNS))
      || (Hall_Pulse_Amount_Cnt < (OprationCue_Amount_Buf - INCHING_ANGLE_TURNS)))
      {
        MotorStop();
        flag_stop = TRUE;
        Flag.OprationCue_Run = NULL;
      }
    }
    //---------------------------------------------------------------------------------
    if (Flag.OprationCue_Stop)//������ʾֹͣ
    {
      if ((Hall_Pulse_Amount_Cnt > (OprationCue_Amount_Buf - 2))
      && (Hall_Pulse_Amount_Cnt < (OprationCue_Amount_Buf + 2)))
      {
        MotorStop();
        flag_stop = TRUE;
        Flag.OprationCue_Stop = NULL;
      }
    }
  }
  else
  {
    if (Flag.Jiggle_Stop)//�㶯ֹͣ��־
    {
      //Target_Speed = MAX_SPEED;
      if (Motor_Duty < MAX_DUTY)
      {
        Motor_Duty++;
      }
      
      if ((Hall_Pulse_Amount_Cnt > (Hall_Pulse_Jiggle_Buff + JIGGLE_ANGLE_TURNS(Joint_Time_Level)))// 5  9��
      || (Hall_Pulse_Amount_Cnt < (Hall_Pulse_Jiggle_Buff - JIGGLE_ANGLE_TURNS(Joint_Time_Level))))
      {
       flag_stop = TRUE;
        Flag.Jiggle_Stop = NULL;
      }
    }
    
       else if (Flag.Angle_Stop)//���нǶ�ֹͣ��־
    {
      //Target_Speed = MAX_SPEED;
      if (Motor_Duty < MAX_DUTY/2)
      {
        Motor_Duty++;
      }
      
      if ((Hall_Pulse_Amount_Cnt > (Hall_Pulse_Jiggle_Buff + Click_the_Angle_of_hall))// 5  9��
      || (Hall_Pulse_Amount_Cnt < (Hall_Pulse_Jiggle_Buff - Click_the_Angle_of_hall)))
      {
       flag_stop = TRUE;
        Flag.Angle_Stop = NULL;
      }
    }
  }
  
  if (flag_stop)
  {
    Flag.Goto_PCT = FALSE;
    Flag.Angle_Stop = NULL ;//�ص���LIU 
    MotorFlag.Motor_Run = NULL;
    Motor.Motor_Status = MOTOR_STOP;
    MotorStop();
    BRAKE_ON();//����ɲ�����Ͽ��̵���    
  }
  
  //����λ����1S2025-2-17
  if(flag_up_limit)
  {
    if(!Flag.Meet_Plugb_Rebound)
    {
      Flag.Meet_Plugb_Rebound = TRUE;                // ����
      Meet_Plugb_Rebound_Time = 10;//����λ����1S
      Meet_Plugb_Rebound_Time_Cnt = 0;
      Motor.Motor_Status = MOTOR_DOWN;//���·���
      Flag.Up_Meet_Plugb = TRUE;//���������������־��1������跴���������������
    }
  }
}

/**********************************************************************************
  ������:   Angle_calculation
  ��  ��:   �����л������к󣬲����Ļ���������>=����ת��180����Ҫ�Ļ���ֵ����Ҷ���Ķ����ָ�180�����0��
            �˶�״̬�ı��ҵ�ǰ״̬Ϊֹͣʱ�ϴ�����
            ��ǰ�Ƕȼ���
*********************************************************************************/
void Angle_calculation   (void)
{
  if((Motor_Status_Buffer_1 != Motor.Motor_Run_Status))//�����л������к󣬲���������������125*1.5����Ҷ���Ķ����ָ�180�����0��
  {      

/*-----------------------------huang----�����ʼ�Ƕ�--------------------------------------------*/
    if(!MotorFlag.Journey_Direction)//Ĭ���г̷���
    {
      if((Motor_Status_Buffer_1 == MOTOR_UP) && (Motor.Motor_Run_Status == MOTOR_STOP))//��һ״̬Ϊ��������һ״̬Ϊֹͣ
      {/*���ֹ֮ͣ��Ž���*/            
        if(MotorFlag.Direction == 0)//Ĭ�Ϸ���OK
        {
          if(Flag.Angle_mark == 0)//��180��Ϊ����������в���
          {
            Hall_difference = (Hall_Pulse_Amount_up - Hall_Pulse_Amount_Cnt);//���ڵĻ���ֵ-�ϴθ����0�Ȼ���180��ʱ�Ļ���ֵ
            
           
          }          
          else//��0��Ϊ�������в���
          {
            Hall_difference = (Hall_Pulse_Amount_Cnt -  Hall_Pulse_Amount_up);
          }            
          if(Hall_difference > CURRENT_ANGLE_180_HALL || ((Hall_Pulse_Amount_Cnt > Hall_Pulse_Amount_up) && Flag.Angle_mark == 0))
          {//���е�0�ȱ���ֵ֮��

            Flag.Angle_mark = 0;//��0��Ϊ����־λ
            Current_Angle  = 180;//�Ƕ���180
            Hall_difference = 0;//������ֵ��0�����¼���
            Hall_Pulse_Amount_up =  Hall_Pulse_Amount_Cnt ;//�洢�µ�һ��180��ʱ��λ��
          }
        } 
        else//����OK
        {            
          if(Flag.Angle_mark == 1)//��0��Ϊ����������в���
          {
            Hall_difference = (Hall_Pulse_Amount_Cnt - Hall_Pulse_Amount_up);//���ڵĻ���ֵ-�ϴθ����0�Ȼ���180��ʱ�Ļ���ֵ
          }          
          else//��180��Ϊ�������в���
          {  
            Hall_difference = (Hall_Pulse_Amount_up -  Hall_Pulse_Amount_Cnt);
          }             
          if(Hall_difference > CURRENT_ANGLE_180_HALL || ((Hall_Pulse_Amount_Cnt < Hall_Pulse_Amount_up) && Flag.Angle_mark == 1))
          {

            Flag.Angle_mark = 1;//��0��Ϊ����־λ
            Current_Angle  = 0; //�Ƕ���0  
            Hall_difference = 0;//������ֵ��0�����¼���
            Hall_Pulse_Amount_up = Hall_Pulse_Amount_Cnt;//�洢�µ�һ��0��ʱ��λ��
          } 
        }
      }        
      if((Motor_Status_Buffer_1 == MOTOR_DOWN) && (Motor.Motor_Run_Status == MOTOR_STOP))//��һ״̬Ϊ��������һ״̬Ϊֹͣ
      {/*���ֹ֮ͣ��Ž���*/                     
        if(MotorFlag.Direction == 0)//Ĭ�Ϸ���OK
        {
          if(Flag.Angle_mark == 1)//��0��Ϊ����������в���
          {
            Hall_difference = (Hall_Pulse_Amount_Cnt - Hall_Pulse_Amount_up);
          }     
          else//��180��Ϊ�������в���
          {
            Hall_difference = (Hall_Pulse_Amount_up -  Hall_Pulse_Amount_Cnt);
          }               
          if(Hall_difference > CURRENT_ANGLE_180_HALL || ((Hall_Pulse_Amount_Cnt < Hall_Pulse_Amount_up) && Flag.Angle_mark == 1))
          {

            Flag.Angle_mark = 1;//��0��Ϊ����־λ
            Current_Angle  = 0; //�Ƕ���0  
            Hall_difference = 0;//������ֵ��0�����¼���
            Hall_Pulse_Amount_up = Hall_Pulse_Amount_Cnt;//�洢�µ�һ��0��ʱ��λ��
          } 
        } 
        else//����OK
        {
          if(Flag.Angle_mark == 0)//��180��Ϊ����������в���
          {
            Hall_difference = (Hall_Pulse_Amount_up - Hall_Pulse_Amount_Cnt);
          }     
          else //��0��Ϊ���
          {
            Hall_difference = (Hall_Pulse_Amount_Cnt -  Hall_Pulse_Amount_up);
          }               
          if(Hall_difference > CURRENT_ANGLE_180_HALL || ((Hall_Pulse_Amount_Cnt > Hall_Pulse_Amount_up) && Flag.Angle_mark == 0))//
          {

            Flag.Angle_mark = 0;//��0��Ϊ����־λ
            Current_Angle  = 180;//�Ƕ���180
            Hall_difference = 0;//������ֵ��0�����¼���
            Hall_Pulse_Amount_up =  Hall_Pulse_Amount_Cnt ;//�洢�µ�һ��180��ʱ��λ��
          } 
        }
      }
    }
    else //���г̷���
    {
      if((Motor_Status_Buffer_1 == MOTOR_UP) && (Motor.Motor_Run_Status == MOTOR_STOP))//��һ״̬Ϊ����
      {            
        if(MotorFlag.Direction == 0)//Ĭ�Ϸ���OK
        {
          if(Flag.Angle_mark == 1)//��0��Ϊ����������в���
          {
            Hall_difference = (Hall_Pulse_Amount_up - Hall_Pulse_Amount_Cnt);//���ڵĻ���ֵ-�ϴθ����0�Ȼ���180��ʱ�Ļ���ֵ
          }          
          else//��180��Ϊ�������в���
          {
            Hall_difference = (Hall_Pulse_Amount_Cnt - Hall_Pulse_Amount_up);
          }            
          if(Hall_difference >= CURRENT_ANGLE_180_HALL/*��0��Ϊ���*/ 
             || ((Hall_Pulse_Amount_up > Hall_Pulse_Amount_Cnt) && Flag.Angle_mark == 0/*��180��Ϊ���*/))
          {//���е�0�ȱ���ֵ֮��
            
     
            Flag.Angle_mark = 0;//��0��Ϊ����־λ
            Current_Angle  = 180; //�Ƕ���0  
            Hall_difference = 0;//������ֵ��0�����¼���
            Hall_Pulse_Amount_up = Hall_Pulse_Amount_Cnt;//�洢�µ�һ��0��ʱ��λ��
          }
        } 
        else//����
        {            
          if(Flag.Angle_mark == 0)//��180��Ϊ����������в���
          {
            Hall_difference = (Hall_Pulse_Amount_Cnt - Hall_Pulse_Amount_up);//���ڵĻ���ֵ-�ϴθ����0�Ȼ���180��ʱ�Ļ���ֵ
          }          
          else//��0��Ϊ�������в���
          {  
            Hall_difference = (Hall_Pulse_Amount_up - Hall_Pulse_Amount_Cnt);      
          }             
          if(Hall_difference > CURRENT_ANGLE_180_HALL/*��0��Ϊ���*/ 
             || ((Hall_Pulse_Amount_Cnt > Hall_Pulse_Amount_up) && Flag.Angle_mark == 1/*��180��Ϊ���*/))
          {
            Flag.Angle_mark = 1;//��180��Ϊ����־λ
            Current_Angle  = 0;//�Ƕ���180
            Hall_difference = 0;//������ֵ��0�����¼���
            Hall_Pulse_Amount_up =  Hall_Pulse_Amount_Cnt ;//�洢�µ�һ��180��ʱ��λ��
          } 
        }
      }        
      else if((Motor_Status_Buffer_1 == MOTOR_DOWN) && (Motor.Motor_Run_Status == MOTOR_STOP))
      {                     
        if(MotorFlag.Direction == 0)//Ĭ�Ϸ���OK
        {
          if(Flag.Angle_mark == 0)//��180��Ϊ����������в���
          {
            Hall_difference = (Hall_Pulse_Amount_Cnt - Hall_Pulse_Amount_up);
          }     
          else//��0��Ϊ�������в���
          {
            Hall_difference = (Hall_Pulse_Amount_up - Hall_Pulse_Amount_Cnt);
          }               
          if(Hall_difference > CURRENT_ANGLE_180_HALL/*��0��Ϊ���*/ 
             || ((Hall_Pulse_Amount_up < Hall_Pulse_Amount_Cnt) && Flag.Angle_mark == 1/*��180��Ϊ���*/))
          { 

            Flag.Angle_mark = 1;//��180��Ϊ����־λ
            Current_Angle  = 0;//�Ƕ���180
            Hall_difference = 0;//������ֵ��0�����¼���
            Hall_Pulse_Amount_up =  Hall_Pulse_Amount_Cnt ;//�洢�µ�һ��180��ʱ��λ��
          } 
        } 
        else//����
        {

          
          if(Flag.Angle_mark == 1)//��0��Ϊ����������в���
          {
            Hall_difference = (Hall_Pulse_Amount_up - Hall_Pulse_Amount_Cnt);//
          }     
          else //��180��Ϊ���
          {
            Hall_difference = (Hall_Pulse_Amount_Cnt - Hall_Pulse_Amount_up);//
            

            
          }               
          if(Hall_difference > CURRENT_ANGLE_180_HALL/*��180��Ϊ���*/ 
             || ((Hall_Pulse_Amount_up > Hall_Pulse_Amount_Cnt) && Flag.Angle_mark == 0/*��0��Ϊ���*/))//1
          {

            Flag.Angle_mark = 0;//��0��Ϊ����־λ
            Current_Angle  = 180;//�Ƕ���0
            Hall_difference = 0;//������ֵ��0�����¼���
            Hall_Pulse_Amount_up = Hall_Pulse_Amount_Cnt ;//�洢�µ�һ��0��ʱ��λ��
          } 
        }
      }
    }
    
/*---------------------------------huang----�Ƕȼ���--------------------------------------------*/
    if(Flag.Angle_mark)                              //��0��Ϊ���İ�Ҷ���Ƕȼ���
    {
      Current_Angle=Hall_difference*180/CURRENT_ANGLE_180_HALL;
      
      //UART_TEXT(Current_Angle,Hall_difference,Flag.Angle_mark,0,0,0);
      
    }
    else                                            //��180��Ϊ���İ�Ҷ���Ƕȼ���
    {
      Current_Angle=180-Hall_difference*180/CURRENT_ANGLE_180_HALL;       //��Ҷ���Ƕȼ���
  
      //UART_TEXT(Current_Angle,Hall_difference,Flag.Angle_mark,0,0,0);
    }
    
/*-----------------------------huang-------�ϴ��Ƕ�����------------------------------------------*/   
    if(Motor.Motor_Run_Status == MOTOR_STOP)//��һ״̬Ϊֹͣ�ϴ�����
    {  
      //Send_Data(1,0x07,8,5);The_network_jitter_once
    }   
    
/*-------------------------huang--------����Ϊ��һ״̬------------------------------------------*/
    Motor_Status_Buffer_1 = Motor.Motor_Run_Status ;           //�洢��һ�ε������ʱ��״̬
  }
}




//-------------------------------------------------------------------------------------
//  ������:   MotorUp
//  ��  ��:   
//-------------------------------------------------------------------------------------
void MotorUp (u16 duty)
{
  if(!MotorFlag.Direction)         //LIU�����˶�״̬�ж�
  {
    Motor.Motor_Run_Status = MOTOR_UP;       //��
  }
  else
  {
    Motor.Motor_Run_Status = MOTOR_DOWN;       //�� 
  }
  MOTOR_WORK();//DRV8256E��nSLEEP������1  
  MOTOR_PH_H();//DRV8256E��PH������1��������з���ѡ��
  
  TB2CCTL1 = OUTMOD_2;//CCR1 toggle/reset
  TB2CCTL2 = OUTMOD_2;//CCR2 toggle/reset
  
  P5SEL0 |= BIT0;//DRV8256E��EN�������PWM
  TB2CCR1 = duty;
   MotorFlag.Motor_Direction = 1;
}
//-------------------------------------------------------------------------------------
//  ������:   MotorDown
//  ��  ��:   
//-------------------------------------------------------------------------------------
void MotorDown (u16 duty)
{
    if(MotorFlag.Direction)         //LIU�����˶�״̬�ж�
  {
    Motor.Motor_Run_Status = MOTOR_UP;       //��
  }
  else
  {
    Motor.Motor_Run_Status = MOTOR_DOWN;       //�� 
  }
  MOTOR_WORK();//DRV8256E��nSLEEP������1
  MOTOR_PH_L();//DRV8256E��PH������0��������з���ѡ��
  
  TB2CCTL1 = OUTMOD_2;//CCR1 toggle/reset
  TB2CCTL2 = OUTMOD_2;//CCR2 toggle/reset
  
  P5SEL0 |= BIT0;//DRV8256E��EN�������PWM
  TB2CCR1 = duty;
   MotorFlag.Motor_Direction = 0;
}
//-------------------------------------------------------------------------------------
//  ������:   MotorStop
//  ��  ��:   
//-------------------------------------------------------------------------------------
void MotorStop (void)
{
  Motor.Motor_Run_Status = MOTOR_STOP;       //ֹͣLIU
  
  MOTOR_SLEEP();
  
  Motor_Duty = 0;
  TB2CCR1 = 0; 
}
//-------------------------------------------------------------------------------------
//  ������:   Duty_Add
//  ��  ��:   
//-------------------------------------------------------------------------------------
void Duty_Add (u16 *p,u8 i)
{
  *p += i;
  if (*p > MAX_DUTY)
  {
    *p = MAX_DUTY;
  }
}
//-------------------------------------------------------------------------------------
//  ������:   Duty_Sub
//  ��  ��:   
//-------------------------------------------------------------------------------------
void Duty_Sub (u16 *p,u8 i)
{
  if (*p > i)
  {
    *p -= i;
  }
}
//-------------------------------------------------------------------------------------
//  ������:   MotorControl
//  ��  ��:   
//  ��  ��:   ��
//  ��  ��:   ��
//  ��  ��:   ��
//  ��  ����  
//-------------------------------------------------------------------------------------
void MotorControl (void)
{
  #define OPRATION_OVERTIME 3000/16
  static u8 Time_Limit_Cnt;
  
  if ((!Flag.Del_Code_Finish) && (!Flag.Opretion_Finish))
  {//ɾ����ɣ�����������ɲŻ�ִ����һ��
    if (MotorFlag.Motor_Run)
    {
      motor_off_time = NULL;
      
      if  (motor_run_time < 6)
      {
        MOTOR_WORK();
        BRAKE_OFF();//��ͨ�̵���
        MotorStop();
        Motor_Speed_Buff = 0;
      }
      //----------------------------------------------------------------------
      else if  (motor_run_time < 8)
      {
        Target_Speed = MIN_SPEED;
        Motor_Duty = MAX_DUTY >> 3;
      }
      //----------------------------------------------------------------------
      else
      {
        signed int En;//,En1;
        u8 flag;
        
        En = Target_Speed - Motor_Speed;//�õ���ǰ�ٶ���� E(n)
        //En1 = Target_Speed - Motor_Speed_Buff;//�õ��ϴ��ٶ���� E(n-1)
        //En += En1;    
        //�����ת��������
        if (En >= 0)//��ǰ�����Ƿ�Ϊ����
        {
          flag = 1;                
        }
        else//����Ϊ����
        {
          En = 0 - En;//ת��������
          flag = 0;
        }
        
        if (Current_Current_mA < MAX_OUT_CURRENT)
        {
          if (flag == 0)//����Ϊ����
          {
              if (En >= 12)
            {
              Duty_Sub(&Motor_Duty,16);
            }
            else if (En >= 6)
            {
              Duty_Sub(&Motor_Duty,8);
            }
            else if (En >= 5)
            {
              Duty_Sub(&Motor_Duty,5);
            }
            else if (En >= 4)
            {
              Duty_Sub(&Motor_Duty,3);
            }
            else if (En >= 3)
            {
              Duty_Sub(&Motor_Duty,2);
            }
            else if (En >= 1)
            {
              Duty_Sub(&Motor_Duty,1);
            }
          }
          else//����Ϊ����
          {
             if (En >= 12)
            {
              Duty_Add(&Motor_Duty,16);
            }
            else if (En >= 6)
            {
              Duty_Add(&Motor_Duty,8);
            }
            else if (En >= 5)
            {
              Duty_Add(&Motor_Duty,5);
            }
            else if (En >= 4)
            {
              Duty_Add(&Motor_Duty,3);
            }
            else if (En >= 3)
            {
              Duty_Add(&Motor_Duty,2);
            }
            else if (En >= 1)
            {
              Duty_Add(&Motor_Duty,1);
            }
          }
        }
        else Duty_Sub(&Motor_Duty,5);        //-----------------------------------------------------------------------------
        //���账��
        if (motor_run_time >= 20)
        {
          //���������оƬ����
          if (MeetPlugbDetect())
          {
            //����ֹͣ
            MotorFlag.Motor_Run = NULL;
            MotorStop();
            BRAKE_ON();//ɲ�����Ͽ��̵���
            Motor.Motor_Status = MOTOR_STOP;
            //----------------------------------------------
            //����оƬ������λоƬ
            if (!MOTOR_NFAULT())
            {
               //����˯�ߣ��ٴβ����������ʱ��������
               MOTOR_SLEEP();
            }
          }
        }
        else
        {
          Hall_Pulse_Amount_Buff = Hall_Pulse_Amount_Cnt;
          Meet_Plugb_Cnt = NULL;
          Meet_Plugb_Cnt1 = NULL;
          Meet_Plugb_Cnt2 = NULL;
          Meet_Plugb_Cnt3 = NULL;
          Flag.slowstop = NULL;
        }
        //--------------------------------------------------------------
        if (!Flag.slowstop)
        {
          if (Target_Speed < MAX_SPEED)
          {
            if (++speed_control_cnt >= 10)
            {
              Target_Speed ++;
              speed_control_cnt = NULL;
            }
          }
        }
        //--------------------------------------------------------------
        if (Flag.Motor_Run_Buffer ^ MotorFlag.Motor_Direction)
        {
          Flag.Motor_Run_Buffer = MotorFlag.Motor_Direction;
          Flag.Journey_Arrive = NULL;
        }
        //--------------------------------------------------------------
        if (Motor_Duty > MAX_DUTY)
        {
          Motor_Duty = MAX_DUTY;
        }
        //--------------------------------------------------------------
        if (Motor_Duty < MIN_DUTY)
        {
          Motor_Duty = MIN_DUTY;
        }
        //--------------------------------------------------------------
        if (MotorFlag.Motor_Run)
        {
          if (MotorFlag.Motor_Direction)
          {
            MotorUp(Motor_Duty);
          }
          else
          {
            MotorDown(Motor_Duty);
          }
        }
      }
      //----------------------------------------------------------------
      if (motor_run_time < 250)
      {
        motor_run_time++;
      }
      //----------------------------------------------------------------
      Motor_Speed_Buff = Motor_Speed;
    }
    //---------------------------------------------------------------------------------
    else//MotorFlag.Motor_Run����0ִ����һ��
    {
      motor_run_time = NULL;
      MotorStop();
      BRAKE_ON();//ɲ�����Ͽ��̵���
      
      if (motor_off_time < 250)
      {
        motor_off_time++;
      }
    }
  }
    else//��Ҫ΢����ʾ
  {//����ɾ�������Ҫ��������ִ����һ��
   //Flag.Del_Code_Finish����1����Flag.Opretion_Finish����1ִ����һ��
    static u8 Jiggle_Direction;
    
    switch (Opretion_Event)
    {
      case 0:
        MotorFlag.Motor_Run = NULL;
        Motor.Motor_Status = MOTOR_STOP;
        MOTOR_WORK();
        
        Opretion_Event++;
        Flag.StatusSwitch_Delay = TRUE; 
        Motor_RunTime_Cnt = MOTOR_SWITCH_LATIME >> 1;
        
        Target_Speed = MAX_SPEED;
        Motor_Duty = MAX_DUTY - 100;
        Flag.Jiggle_Stop = NULL;
        
        Jiggle_Direction = 0;
        
         if (Limit_Point[0].Flag.F1.Limit_Activate)
          {
            if(Hall_Pulse_Amount_Cnt < (Limit_Point[0].Limit_Place + INCHING_ANGLE_TURNS+5))
            {
              Jiggle_Direction = MotorFlag.Journey_Direction;
            }
            else if ((Limit_Point_Amount > 1)&&(Hall_Pulse_Amount_Cnt > (Limit_Point[Limit_Point_Amount-1].Limit_Place-INCHING_ANGLE_TURNS-5)))
            {
              Jiggle_Direction = !MotorFlag.Journey_Direction;
            }
            else{
              Jiggle_Direction = MotorFlag.Motor_Direction;
            }
          }
          else
          {
            Jiggle_Direction = MotorFlag.Motor_Direction;
          }
      break;
      
      case 1://��
      case 5://��
        if (!Flag.StatusSwitch_Delay)
        {
          OprationCue_Amount_Buf = Hall_Pulse_Amount_Cnt;
          Flag.OprationCue_Run = TRUE;
          Flag.StatusSwitch_Delay = TRUE;//�ȴ�500MS
          Opretion_Event++;
          Time_Limit_Cnt = NULL;
          
          MotorStop();
          BRAKE_OFF();//��ͨ�̵�����׼����΢��
          Motor_Duty = MAX_DUTY - 100;
        }
      break;
    
      case 2:
      case 6:
        if (Flag.OprationCue_Run)
        {
          if (++Time_Limit_Cnt > OPRATION_OVERTIME)
          {
            Time_Limit_Cnt = NULL;
            Flag.OprationCue_Run = NULL;
            Flag.StatusSwitch_Delay = TRUE;//�ȴ�200MS
            Motor_RunTime_Cnt = MOTOR_SWITCH_LATIME >> 1;
          }
          else
          {
            if (Motor_Duty < MAX_DUTY)
            {
              Duty_Add(&Motor_Duty,5);
            }
            
            //��΢��
              {
                if (Jiggle_Direction)
                {
                  MotorDown(Motor_Duty); // down
                }
                else
                {
                  MotorUp(Motor_Duty);  // UP
                }
              }
          }
        }
        else if (!Flag.StatusSwitch_Delay)
        {
          Opretion_Event++;
          Flag.StatusSwitch_Delay = TRUE;//��΢��������ȴ�200MS
          Motor_RunTime_Cnt = MOTOR_SWITCH_LATIME >> 1;
          
          MotorStop();//��΢����ʱֹͣ
          BRAKE_ON();//ɲ�����Ͽ��̵���
        }
      break;
    
      case 3://��
      case 7://��
        if (!Flag.StatusSwitch_Delay)
        {
          Opretion_Event++;
          Flag.OprationCue_Stop = TRUE;
          Time_Limit_Cnt = NULL;
          MotorStop();
          BRAKE_OFF();//��ͨ�̵�����׼����΢��
          Motor_Duty = MAX_DUTY - 100;
        }
      break;
    
      case 4: 
      case 8://OFF
        Time_Limit_Cnt++;	  
        if ((!Flag.OprationCue_Stop) || (Time_Limit_Cnt > OPRATION_OVERTIME))
        {
          //��΢��������ʱ
          Time_Limit_Cnt = NULL;
          Flag.OprationCue_Run = NULL;
          if (Flag.Del_Code_Finish)
          {//��������ִ����һ�Σ�����case�����һ����Ϊ�˶�������
            Opretion_Event++;
            Flag.StatusSwitch_Delay = TRUE;//ɾȫ����ȴ�0.5�룬׼����һ��΢��
          }
          else
          {
            Opretion_Event = 9;
          }
        }
        else
        {
          if (Motor_Duty < MAX_DUTY)
          {
            Duty_Add(&Motor_Duty,5);
          }
            
          //��΢��
            {
              if (Jiggle_Direction)
              {
                MotorUp(Motor_Duty);  // UP
              }
              else
              {
                MotorDown(Motor_Duty); // down
              }
            }
        }
      break;
       
      default:
        Opretion_Event = NULL;
        Flag.Del_Code_Finish = NULL;
        Flag.Opretion_Finish = NULL;
        Motor.Motor_Status = MOTOR_STOP;
        motor_off_time = 0;//΢���������ӳ�2��
        MotorStop();
        BRAKE_ON();//ɲ�����Ͽ��̵���
      break;
    }
  }
}
//-------------------------------------------------------------------------------------
//  ������:   MotorStatusSwitch
//  ��  ��:   ����״̬�л�
//  ��  ��:   ��
//  ��  ��:   ��
//  ��  ��:   ��
//  ��  ����  
//-------------------------------------------------------------------------------------
void MotorStatusSwitch (void)
{
  if (!Flag.StatusSwitch_Delay)//�Ƿ����ӳ�500mS
  {
    if (Motor.Motor_Status == Motor.Motor_Status_Buffer)//״̬�Ƿ�û�иı�
    {
      if (++Motor_RunTime_Cnt >= MOTOR_RUN_CNT_VALUE)//�����ʱ���Ƿ�
      {
        Motor.Motor_Status = MOTOR_STOP;
        Motor_RunTime_Cnt = NULL;
      }
    }
    else
    {
      Set_Limit_Point_Cnt = NULL;
      Joint_Level_ADJ_Time = 0;
      Flag.Motor_Status_Switch = TRUE;
      MotorStop();
      
      motor_run_time = NULL;
      motor_off_time = NULL;
      Motor_RunTime_Cnt = NULL;
      Motor_Speed = NULL;

      //Hall_Pulse_Jiggle_Buff = Hall_Pulse_Amount_Cnt;//���浱ǰλ��
      
      
      if (3 == (Motor.Motor_Status_Buffer | Motor.Motor_Status))//�Ƿ������л�
      {
        Flag.StatusSwitch_Delay = TRUE;//�ȴ�500MS
        Motor.Motor_Status_Buffer = MOTOR_STOP;//���ֹͣ
      }
      else
      {
        Motor_RunTime_Cnt = MOTOR_RUN_TIME;
        Motor.Motor_Status_Buffer = Motor.Motor_Status;//���µ�λ
      }
    }
    //---------------------------------------------------------------------------------
    switch (Motor.Motor_Status_Buffer)//��ǰ���״̬��ʲô
    {
      case MOTOR_UP://��
        MotorFlag.Motor_Run = TRUE;
        SET_UP_DIRECTION();
      break;
      case MOTOR_STOP://ͣ
        MotorFlag.Motor_Run = NULL;
      break;
      case MOTOR_DOWN://��
        MotorFlag.Motor_Run = TRUE;
        SET_DOWN_DIRECTION();
        if(!Flag.Meet_Plugb_Rebound){
          Flag.Up_Meet_Plugb = NULL;//���������־�������20250217ry
        }
      break;
    }
    //---------------------------------------------------------------------------------
    if (Flag.Joint_Action)//�Ƿ���Ҫ�㶯������
    {
      if (++Joint_Action_Time_Cnt > JOINT_ACTION_TIME)
      {
//        Joint_Action_Time_Cnt = 0;
//        Motor.Motor_Status = Joint_Action_Motor_Buf;
//        Flag.Joint_Action = NULL;
      }
    }
    else
    {
      Joint_Action_Time_Cnt = 0;
    }
    //---------------------------------------------------------------------------------
   
        if (Flag.Run_To_finishing_point)  // �Ƿ�Ҫ���е��յ�
    {
      Flag.Run_To_finishing_point = NULL;
      if (!(MotorFlag.Motor_Direction ^ MotorFlag.Journey_Direction))
      {
        Target_Limit_Point = NULL; // ֻ����һ����λ��
      }
      else
      {
        Target_Limit_Point = Limit_Point_Amount - 1; // ���е����һ����λ��
      }
    }
    // �Ƿ����л�״̬����������λ��
    else if ((Flag.Motor_Status_Switch) && (Limit_Point[0].Flag.F1.Limit_Activate))
    {
      u8 x;
      
      Flag.Motor_Status_Switch = NULL;
      Flag.NO_Comper_Limit_Point = NULL;
      
      // �Ƿ�͵�һ���г̷���һ��
      if (!(MotorFlag.Motor_Direction ^ MotorFlag.Journey_Direction))
      {
        Flag.NO_Comper_Limit_Point = NULL;
        if (Flag.Set_Limit_Point)  // �Ƿ���������λ״̬
        {
          Target_Limit_Point = NULL; // ֻ����һ����λ��
        }
        else
        {
          x = LoopCompare(Hall_Pulse_Amount_Cnt,LIMIT_PLACE);  /*��ǰ�Ƿ���ĳ����λ��*/
          if (x != NULL)                              
          {
            if (x > 1) // �Ƿ�ǰ�ڵ�һ����λ����
            {
              Target_Limit_Point = x - 2;  // ���е���ǰ��λ�����һ��
            }
            else
            {
              Target_Limit_Point = NULL; // ��ǰ�ڵ�һ����λ�㣬�����һ����λ
            }
          }
          else   // ��ǰ������λ��λ��
          {
            // �����һ����λ�㿪ʼ���
            for (x = MID_LIMIT_POINT_AMOUNT; x > 0; x--)
            {
              if (Limit_Point[x].Flag.F1.Limit_Activate)
              {
                // ��ǰλ���Ƿ��ڸ���λ�����
                if (Hall_Pulse_Amount_Cnt > Limit_Point[x].Limit_Place)
                {
                  break;  //�˳����е�����λ��
                }
              }
            }
            Target_Limit_Point = x;
          }
        }
      }
      else  // ����һ����λ���෴�ķ�����
      {
        if (Flag.Set_Limit_Point) // �Ƿ���������λ��
        {
          Flag.NO_Comper_Limit_Point = TRUE;  // �������λ
        }
        else
        {
          Flag.NO_Comper_Limit_Point = NULL;
          x = LoopCompare(Hall_Pulse_Amount_Cnt,LIMIT_PLACE);  /*��ǰ�Ƿ���ĳ����λ��*/
          if (x != NULL)                              
          {
            if (x >= Limit_Point_Amount) // ��ǰ�Ƿ����һ����λ����
            {
              if (Limit_Point_Amount == 1) // �Ƿ�ֻ��һ����λ��
              {
                Flag.NO_Comper_Limit_Point = TRUE;  // �������λ
              }
              else //��ֻһ����λ�㣬��ǰֹͣ������λ����
              {
                Target_Limit_Point = x - 1;  // ���ּ�����һ����λ��
              }
            }
            else // �������һ����λ����
            {
              Target_Limit_Point = x; // ���е���һ��
            }
          }
          else  // û��ͣ����λ����
          {
            // �ӵ�2����λ�㿪ʼ�Ƚ�
            for (x = 1; x < (MID_LIMIT_POINT_AMOUNT + 2); x++)
            {
              if (Limit_Point[x].Flag.F1.Limit_Activate)
              {
                // �Ƿ�ǰλ���ڸ���λ��ǰ��
                if (Hall_Pulse_Amount_Cnt < Limit_Point[x].Limit_Place)
                {
                  break; // �˳����е�����λ��
                }
              }
              else // ����û����λ����
              {
                if (x > 1) // ���ڵ�һ����λ�㷶Χ
                {
                  x--;  // ���ּ�鵱ǰ��λ��
                }
                else  // ֻ��һ����λ��
                {
                  Flag.NO_Comper_Limit_Point = TRUE;  // �������λ
                }
                break;
              }
            }
            Target_Limit_Point = x;
          }
        }
      }
    }
  }
  else if (++Motor_RunTime_Cnt > MOTOR_SWITCH_LATIME)
  {
    Motor_RunTime_Cnt = NULL;
    Flag.StatusSwitch_Delay = NULL;
  }
}
