/**********************************************************************************
 *  项目编号:    AC238-01
 *  项目名称:    AM45遇阻反弹电机
 *  客户名称:    
 *  程序编写:    符小玲
 *  启动日期:    2013-08-15
 *  完成日期:    
 *  版本号码:    V2.4
 *
 *  KeyManage.c file
 *  功能描述: 按键扫描及处理
**********************************************************************************/
#include "Define.h"                                          /* 全局宏定义 ------*/
#include "Extern.h"                                          /* 全局变量及函数 --*/
/* 内部宏 -----------------------------------------------------------------------*/
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

/* 内部函数 ---------------------------------------------------------------------*/
void KeyControl (Motor_Control_TypeDef command);
void NoKey (Motor_Control_TypeDef command);
/* 内部变量 ---------------------------------------------------------------------*/
static u8 Key_ON_Cnt = KEY_DIS_SHAKE_TIME;
static u8 Key_Old_Value;

/* 内部变量 ---------------------------------------------------------------------*/
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
  函数名:   KeyControl
  功  能:   
 *********************************************************************************/
void KeyControl (Motor_Control_TypeDef command)
{
  Flag.Goto_PCT = FALSE;
  Flag.Angle_Stop = NULL ;//干触点关调光LIU   
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
  函数名:   NoKey
  功  能:   
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
  函数名:   NoKey
  功  能:   
 *********************************************************************************/
void DotNoKey (Motor_Control_TypeDef command)
{
  
  Motor.Motor_Status = MOTOR_STOP;
}
/**********************************************************************************
函数名:   KeyScan
功  能:   扫描按键
输  入:   空
输  出:   空
返  回:   空
描  述：  
*********************************************************************************/
void KeyManage (void)
{
  u8 i,new_key_value;
  
  new_key_value = KeyIN();                                 // 整体读键值
  if (new_key_value != Key_Old_Value)                       // 是否有新的按键
  {
    if (--Key_ON_Cnt == 0)
    {
      Key_ON_Cnt = KEY_DIS_SHAKE_TIME;
      for (i = 0; i < KEY_NUM; i++)
      {
        if (new_key_value == KeyArray[i].KEY_VALUE)
        {
          (*KeyArray[i].KeyDisposal) (KeyArray[i].COMMAND);
          Key_Old_Value = new_key_value;                     // 更新当前键值
          break;
        }
      }
    }
  }
  else
  {
    Key_ON_Cnt = KEY_DIS_SHAKE_TIME;
  }
  // 学习键
  if (LEARN_KEY_IN())                                        /* 是否为SET键 -----*/
  {
    if(--Learn_Key_ON_Cnt == 0)                              /* 是否过了消抖时间 */
    {
      Learn_Key_ON_Cnt = KEY_DIS_SHAKE_TIME;
      Learn_Key_OFF_Cnt = KEY_DIS_SHAKE_TIME;
      
      if (++Learn_Key_Cnt > LONG_KEY_MAX_TIME)  // 按键时间是否超长
      {
        Learn_Key_Cnt = 0;
        Flag.Set_Key_ON = 0;                  // 不影响电机工作
        KeyFlag.SET_Dispose = TRUE;
      }
      if ((!KeyFlag.SET_Dispose) && (!Flag.Set_Limit_Point))
      {
        if (!Flag.Set_Key_ON)
        {
          Flag.Set_Key_ON = TRUE;
        }
        if (Learn_Key_Cnt == LEARN_TIME)                     /* 是否处理 --------*/
        {
          Flag.Learn_Code_Statu = TRUE;
          
          //Dispose_All_Date_For_TX();                //上传所有状态的数据处理
          
          Flag.Opretion_Finish = TRUE;
          Opretion_Event = 0;
          Flag.Time_100ms = TRUE;
          Motor.Motor_Status = MOTOR_STOP;
        }
        else if(Learn_Key_Cnt == CLR_Net_TIME)
        {
          if (KeyFlag.CLR_Net_Status)  // 是否清网
          {
            CLR_Net_Key_Num = 0;
            KeyFlag.CLR_Net_Status = FALSE;
            
            Send_Data(3,0x04,0,0);  //发3次     入网 
            Flag.Learn_Code_Statu = FALSE;
            Flag.Opretion_Finish = TRUE;
            Flag.CLR_Net_Oder = TRUE;
            Flag.net_add = TRUE;
            Flag.Uart_tx = TRUE;   // 串口发送清网命令提示
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
    if(--Learn_Key_OFF_Cnt == 0)                                  /* 是否过了消抖时间 */
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
  // 换向键
  if (DIRECTION_KEY_IN())                                    /* 是否为SET键 -----*/
  {
    if(--Direction_Key_ON_Cnt == 0)                          /* 是否过了消抖时间 */
    {
      Direction_Key_ON_Cnt = KEY_DIS_SHAKE_TIME;
      Direction_Key_OFF_Cnt = KEY_DIS_SHAKE_TIME;
      
      if ((!KeyFlag.Direction_Dispose) && (!Flag.Set_Limit_Point) && (!Flag.Learn_Code_Statu))
      {
        if (++Direction_Key_Cnt == SWITCH_DIRECTION_TIME)    /* 是否处理 --------*/
        {
          Direction_Key_Cnt = 0;
          KeyFlag.Direction_Dispose = TRUE;
          Motor.Motor_Flag.Direction = !Motor.Motor_Flag.Direction;
          MotorFlag.Direction = !MotorFlag.Direction;
          Flag.Opretion_Finish = TRUE;//启动抖动提醒
          Opretion_Event = NULL;
          // JourneyDelete();
          
          if(Limit_Point_Amount==1) 
          {
            Switvh_Direction_Order = !Switvh_Direction_Order;  //设完一个限位点后换向
          }
          
          Flag.Uart_tx = TRUE;
          
        }
      }
    }
  }
  else
  {
    if(--Direction_Key_OFF_Cnt == 0)                         /* 是否过了消抖时间 */
    {
      Direction_Key_OFF_Cnt = KEY_DIS_SHAKE_TIME;
      Direction_Key_ON_Cnt = KEY_DIS_SHAKE_TIME;
      KeyFlag.Direction_Dispose = FALSE;
      Direction_Key_Cnt = 0;
    }
  }
}