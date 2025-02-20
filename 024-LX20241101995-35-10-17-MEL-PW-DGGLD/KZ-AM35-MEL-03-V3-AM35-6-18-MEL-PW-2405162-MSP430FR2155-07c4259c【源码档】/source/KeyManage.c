/**********************************************************************************
 *  ��Ŀ���:    AC238-01
 *  ��Ŀ����:    AM45���跴�����
 *  �ͻ�����:    
 *  �����д:    ��С��
 *  ��������:    2013-08-15
 *  �������:    
 *  �汾����:    V2.4
 *
 *  KeyManage.c file
 *  ��������: ����ɨ�輰����
**********************************************************************************/
#include "Define.h"                                          /* ȫ�ֺ궨�� ------*/
#include "Extern.h"                                          /* ȫ�ֱ��������� --*/
/* �ڲ��� -----------------------------------------------------------------------*/
#define KEY_DIS_SHAKE_TIME     40     / SYSTEM_TIME
#define LONG_KEY_TIME          500    / SYSTEM_TIME
#define LEARN_TIME             (500   / (SYSTEM_TIME * (KEY_DIS_SHAKE_TIME)))
#define CLR_Net_TIME           (3000   / (SYSTEM_TIME * (KEY_DIS_SHAKE_TIME)))
#define DELETE_TIME            (7000  / (SYSTEM_TIME * (KEY_DIS_SHAKE_TIME)))
#define SWITCH_DIRECTION_TIME  (7000  / (SYSTEM_TIME * (KEY_DIS_SHAKE_TIME)))
#define LONG_KEY_MAX_TIME      (10000 / (SYSTEM_TIME * (KEY_DIS_SHAKE_TIME)))
#define CLR_TIME               (2000  / (SYSTEM_TIME * (KEY_DIS_SHAKE_TIME)))
#define ENTER_DELETE_TIME      (7000  / (SYSTEM_TIME * (KEY_DIS_SHAKE_TIME)))

#define LEARN_KEY              BIT6
#define DIRECTION_KEY          BIT7

#define LEARN_KEY_IN()        (((P4IN) & LEARN_KEY)? 0 : 1)
#define DIRECTION_KEY_IN()    (((P4IN) & DIRECTION_KEY)? 0 : 1)

#define KEY_NUM                4
#define UP_KEY                 BIT4 
#define DOWN_KEY               BIT5
#define STOP_KEY               BIT4+BIT5
#define NO_KEY                 0x00

/* �ڲ����� ---------------------------------------------------------------------*/
void KeyControl (Motor_Control_TypeDef command);
void NoKey (Motor_Control_TypeDef command);
/* �ڲ����� ---------------------------------------------------------------------*/
static u8 Key_ON_Cnt = KEY_DIS_SHAKE_TIME;
static u8 Key_Old_Value;

/* �ڲ����� ---------------------------------------------------------------------*/
static u8 Learn_Key_ON_Cnt = KEY_DIS_SHAKE_TIME;
static u8 Learn_Key_OFF_Cnt = KEY_DIS_SHAKE_TIME;
static u16 Learn_Key_Cnt;
static u8 Direction_Key_ON_Cnt = KEY_DIS_SHAKE_TIME;
static u8 Direction_Key_OFF_Cnt = KEY_DIS_SHAKE_TIME;
static u16 Direction_Key_Cnt;

static u8 Key_Status_Cnt;
static u8 CLR_Net_Key_Num;
static u16 CLR_Net_Key_Time_Out_Cnt;


Motor_Control_TypeDef const KeyLoopArray[4] =
{
    MOTOR_UP,MOTOR_STOP,MOTOR_DOWN,MOTOR_STOP,
};

struct 
{
  u8 SET_Dispose               : 1;
  u8 Direction_Dispose         : 1;
  u8 CLR_Net_Disposal          : 1;
  u8 CLR_Net_Status            : 1;
}KeyFlag;

typedef struct
{
  u8 const KEY_VALUE;
  Motor_Control_TypeDef const COMMAND;
  void (*KeyDisposal) (Motor_Control_TypeDef command);
}Key;

Key const KeyArray [KEY_NUM] =
{
  {UP_KEY,   MOTOR_UP,   KeyControl},
  {DOWN_KEY, MOTOR_DOWN, KeyControl},
  {STOP_KEY, MOTOR_STOP, KeyControl},
  {NO_KEY,   MOTOR_STOP, NoKey}
};

u8 KeyIN (void)
{
  u8 temp;
  
  temp  = (~P4IN) & UP_KEY;
  temp |= (~P4IN) & DOWN_KEY;
  temp |= (~P4IN) & STOP_KEY;
  
  return (temp);
}
/**********************************************************************************
  ������:   KeyControl
  ��  ��:   
 *********************************************************************************/
void KeyControl (Motor_Control_TypeDef command)
{
  Flag.Goto_PCT = FALSE;
  Flag.Angle_Stop = NULL ;//�ɴ���ص���LIU   
  switch (Dry_Contact_Control_Mode)
  {
    case NORMAL:
      Motor.Motor_Status = command;
    break;
    case STXT_LOOP:
      Motor.Motor_Status = KeyLoopArray[Key_Status_Cnt];
      if (++Key_Status_Cnt >= 4)
      {
        Key_Status_Cnt = 0;
      }
    break;
    case SST_XXT_SXQH:
      if (Motor.Motor_Status == command)
      {
        Motor.Motor_Status = MOTOR_STOP;
      }
      else
      {
        Motor.Motor_Status = command;
      }
    break;
    case SONG_SHOU_TING:
      Motor.Motor_Status = command;
    break;
    case REN_YI_JIAN_TING:
      if (Motor.Motor_Status != MOTOR_STOP)
      {
        Motor.Motor_Status = MOTOR_STOP;
      }
      else
      {
        Motor.Motor_Status = command;
      }
    break;
    default:
      Dry_Contact_Control_Mode = NORMAL;
    break;
  }
}
/**********************************************************************************
  ������:   NoKey
  ��  ��:   
 *********************************************************************************/
void NoKey (Motor_Control_TypeDef command)
{
  if (Dry_Contact_Control_Mode == SONG_SHOU_TING)
  {
    if ((Key_Old_Value == UP_KEY) || (Key_Old_Value == DOWN_KEY))
    {
      Motor.Motor_Status = MOTOR_STOP;
    }
  }
}
/**********************************************************************************
  ������:   NoKey
  ��  ��:   
 *********************************************************************************/
void DotNoKey (Motor_Control_TypeDef command)
{
  
  Motor.Motor_Status = MOTOR_STOP;
}
/**********************************************************************************
������:   KeyScan
��  ��:   ɨ�谴��
��  ��:   ��
��  ��:   ��
��  ��:   ��
��  ����  
*********************************************************************************/
void KeyManage (void)
{
  u8 i,new_key_value;
  
  new_key_value = KeyIN();                                 // �������ֵ
  if (new_key_value != Key_Old_Value)                       // �Ƿ����µİ���
  {
    if (--Key_ON_Cnt == 0)
    {
      Key_ON_Cnt = KEY_DIS_SHAKE_TIME;
      for (i = 0; i < KEY_NUM; i++)
      {
        if (new_key_value == KeyArray[i].KEY_VALUE)
        {
          (*KeyArray[i].KeyDisposal) (KeyArray[i].COMMAND);
          Key_Old_Value = new_key_value;                     // ���µ�ǰ��ֵ
          break;
        }
      }
    }
  }
  else
  {
    Key_ON_Cnt = KEY_DIS_SHAKE_TIME;
  }
  // ѧϰ��
  if (LEARN_KEY_IN())                                        /* �Ƿ�ΪSET�� -----*/
  {
    if(--Learn_Key_ON_Cnt == 0)                              /* �Ƿ��������ʱ�� */
    {
      Learn_Key_ON_Cnt = KEY_DIS_SHAKE_TIME;
      Learn_Key_OFF_Cnt = KEY_DIS_SHAKE_TIME;
      
      if (++Learn_Key_Cnt > LONG_KEY_MAX_TIME)  // ����ʱ���Ƿ񳬳�
      {
        Learn_Key_Cnt = 0;
        Flag.Set_Key_ON = 0;                  // ��Ӱ��������
        KeyFlag.SET_Dispose = TRUE;
      }
      if ((!KeyFlag.SET_Dispose) && (!Flag.Set_Limit_Point))
      {
        if (!Flag.Set_Key_ON)
        {
          Flag.Set_Key_ON = TRUE;
        }
        if (Learn_Key_Cnt == LEARN_TIME)                     /* �Ƿ��� --------*/
        {
          Flag.Learn_Code_Statu = TRUE;
          
          //Dispose_All_Date_For_TX();                //�ϴ�����״̬�����ݴ���
          
          Flag.Opretion_Finish = TRUE;
          Opretion_Event = 0;
          Flag.Time_100ms = TRUE;
          Motor.Motor_Status = MOTOR_STOP;
        }
        else if(Learn_Key_Cnt == CLR_Net_TIME)
        {
          if (KeyFlag.CLR_Net_Status)  // �Ƿ�����
          {
            CLR_Net_Key_Num = 0;
            KeyFlag.CLR_Net_Status = FALSE;
            
            Send_Data(3,0x04,0,0);  //��3��     ���� 
            Flag.Learn_Code_Statu = FALSE;
            Flag.Opretion_Finish = TRUE;
            Flag.CLR_Net_Oder = TRUE;
            Flag.net_add = TRUE;
            Flag.Uart_tx = TRUE;   // ���ڷ�������������ʾ
          }
        }
        else if (Learn_Key_Cnt >= DELETE_TIME)
        {
          Flag.Learn_Code_Statu = FALSE;
          Flag.Delete_Code = TRUE;
          KeyFlag.SET_Dispose = TRUE;
          Motor.Motor_Status = MOTOR_STOP;
        }
        else if ((!KeyFlag.CLR_Net_Disposal))
        {
          KeyFlag.CLR_Net_Disposal = TRUE;
          CLR_Net_Key_Time_Out_Cnt = 0;
          Motor.Motor_Status = MOTOR_STOP;
          if (++CLR_Net_Key_Num >= 4)
          {
            KeyFlag.CLR_Net_Status = TRUE;
          }
        }
      }
    }
  }
  else
  {
    if(--Learn_Key_OFF_Cnt == 0)                                  /* �Ƿ��������ʱ�� */
    {
      Learn_Key_OFF_Cnt = KEY_DIS_SHAKE_TIME;
      Learn_Key_ON_Cnt = KEY_DIS_SHAKE_TIME;
      KeyFlag.SET_Dispose = FALSE;
      Learn_Key_Cnt = 0;
      Flag.Set_Key_ON = FALSE;
      KeyFlag.CLR_Net_Disposal = FALSE;
      
      if (++CLR_Net_Key_Time_Out_Cnt >= CLR_TIME)
      {
        KeyFlag.CLR_Net_Status = FALSE;
        CLR_Net_Key_Num = 0;
        CLR_Net_Key_Time_Out_Cnt = 0;
      }
    }
  }
  // �����
  if (DIRECTION_KEY_IN())                                    /* �Ƿ�ΪSET�� -----*/
  {
    if(--Direction_Key_ON_Cnt == 0)                          /* �Ƿ��������ʱ�� */
    {
      Direction_Key_ON_Cnt = KEY_DIS_SHAKE_TIME;
      Direction_Key_OFF_Cnt = KEY_DIS_SHAKE_TIME;
      
      if ((!KeyFlag.Direction_Dispose) && (!Flag.Set_Limit_Point) && (!Flag.Learn_Code_Statu))
      {
        if (++Direction_Key_Cnt == SWITCH_DIRECTION_TIME)    /* �Ƿ��� --------*/
        {
          Direction_Key_Cnt = 0;
          KeyFlag.Direction_Dispose = TRUE;
          Motor.Motor_Flag.Direction = !Motor.Motor_Flag.Direction;
          MotorFlag.Direction = !MotorFlag.Direction;
          Flag.Opretion_Finish = TRUE;//������������
          Opretion_Event = NULL;
          // JourneyDelete();
          
          if(Limit_Point_Amount==1) 
          {
            Switvh_Direction_Order = !Switvh_Direction_Order;  //����һ����λ�����
          }
          
          Flag.Uart_tx = TRUE;
          
        }
      }
    }
  }
  else
  {
    if(--Direction_Key_OFF_Cnt == 0)                         /* �Ƿ��������ʱ�� */
    {
      Direction_Key_OFF_Cnt = KEY_DIS_SHAKE_TIME;
      Direction_Key_ON_Cnt = KEY_DIS_SHAKE_TIME;
      KeyFlag.Direction_Dispose = FALSE;
      Direction_Key_Cnt = 0;
    }
  }
}