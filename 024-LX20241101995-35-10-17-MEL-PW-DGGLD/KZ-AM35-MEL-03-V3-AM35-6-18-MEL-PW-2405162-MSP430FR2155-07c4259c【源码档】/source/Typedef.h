

typedef signed long  s32;
typedef signed short s16;
typedef signed char  s8;

typedef signed long  const sc32;//Read Only
typedef signed short const sc16;//Read Only
typedef signed char  const sc8;//Read Only
                                                             
typedef volatile signed long  vs32;                          
typedef volatile signed short vs16;                          
typedef volatile signed char  vs8;                           
                                                             
//typedef volatile signed long const vsc32;//Read Only
//ypedef volatile signed short const vsc16;//Read Only
//typedef volatile signed char const vsc8;//Read Only
                                                             
typedef unsigned long  u32;                                  
typedef unsigned short u16;                                  
typedef unsigned char  u8;                                   
                                                             
typedef unsigned long  const uc32;//Read Only
typedef unsigned short const uc16;//Read Only
typedef unsigned char  const c8;//Read Only
                                                             
typedef volatile unsigned long  vu32;                       
typedef volatile unsigned short vu16;                       
typedef volatile unsigned char  vu8;                        
                                                             
                                                             
//typedef volatile unsigned long  const  vuc32;//Read Only
//typedef volatile unsigned short  const vuc16;//Read Only
//typedef volatile unsigned char  const  vuc8;//Read Only

//typedef struct
//{
//  u8 vol_status    : 1;
//  u8 edge          : 1;
//  u8 edge_change   : 1;
//  u8 const HALL;
//  u8 sample_value;
//  u8 sample_cnt;
//  u16 time_cnt;
//}Hall;
typedef struct
{
  u8 edge   : 1;
  u8 Buf;
  u8 Cnt;
}Hall;
typedef enum
{
  FALSE = 0,
  TRUE = !FALSE
}bool;
typedef enum
{
  RESET = 0,
  SET = !RESET
}FlagStatus, ITStatus, BitStatus;
typedef enum 
{
  DISABLE = 0,
  ENABLE = !DISABLE
}FunctionalState;

typedef union
{
  u8 Flag_Byte;
  struct
  {
    u8 b0 :1;
    u8 b1 :1;
    u8 b2 :1;
    u8 b3 :1;
    u8 b4 :1;
    u8 b5 :1;
    u8 b6 :1;
    u8 b7 :1;
  }Flag_Bit;
}Bit_TypeDef;

typedef enum {
  RF_DATA_HEAD                 = (u8)0xA3,//RF头码
  RF_UP                        = (u8)0x0b,//RF上行命令
  RF_STOP                      = (u8)0x23,//RF停止命令
  RF_STOP_2S                   = (u8)0x5a,//RF停止命令2S
  RF_JIGGLE_STOP               = (u8)0x24,//RF点动停命令
  RF_DOWN                      = (u8)0x43,//RF下行命令
  RF_LEARN                     = (u8)0x53,//RF学习命令
  RF_CLEAR                     = (u8)0x2b,//RF清除命令
  RF_AWOKEN                    = (u8)0x50,//RF唤醒命令
  RF_UP_DOWN_1S                = (u8)0x4a,
  RF_UP_DOWN_5S                = (u8)0x3a,
  RF_UP_STOP                   = (u8)0x1a,
  RF_DOWN_STOP                 = (u8)0x2a,
  RF_ACCESS_LEARN              = (u8)0x55,
  
} RF_Command_TypeDef;

#define LED_HINT_NUM           12
typedef void (*LedHint) (void);
typedef enum {
  LED_OFF                      = (u8)0x00,
  SET_SPEED_STATUS             = (u8)0x01,
  SPEED_HINT                   = (u8)0x02,
  LEARN_STATUS                 = (u8)0x03,
  DELETE_STATUS                = (u8)0x04,
  OPRATION_FINISH              = (u8)0x05,
  DELETE_FINISH                = (u8)0x06,
  OVERTEMPERATURE_PROTECT      = (u8)0x07,
  OVERVOLTAGE_PROTECT          = (u8)0x08,
  UNDERVOLTAGE_PROTECT         = (u8)0x09,
  BLUE_LED_HINT                = (u8)0x0A,
} Led_Display_TypeDef;

typedef enum {
  NORMAL                       = (u8)0x00,//正常模式
  STXT_LOOP                    = (u8)0x01,//上停下停循环
  SST_XXT_SXQH                 = (u8)0x02,//上上停下下停
  SONG_SHOU_TING               = (u8)0x03,//松手停
  REN_YI_JIAN_TING             = (u8)0x04,//任意停
} Dry_Contact_TypeDef;

#define BAUD_RATE_NUM          6
typedef enum {
  BR1200                       = (u8)0x00,
  BR2400                       = (u8)0x01,
  BR4800                       = (u8)0x02,
  BR9600                       = (u8)0x03,
  BR19200                      = (u8)0x04,
  BR115200                     = (u8)0x05,
} Uart_Baud_Rate_TypeDef;

typedef enum {
  MOTOR_STOP                   = (u8)0x00,//停止
  MOTOR_UP                     = (u8)0x01,//上行
  MOTOR_DOWN                   = (u8)0x02,//下行
  MOTOR_UP_INCHING             = (u8)0x04,//点动上
  MOTOR_DOWN_INCHING           = (u8)0x08,//点动下
} Motor_Control_TypeDef;

typedef struct 
{
  u8 Direction                 : 1;//电机方向标志
  u8 Run_Status                : 1;//电机运行状态
  u8 Run_Direction             : 1;//电机运行方向标志
  u8 Fact_Direction            : 1;//电机实际方向标志
                                   
  u8 StatusSwitch              : 1;//状态切换延迟标志
  u8 StatusSwitch_Delay        : 1;//状态切换延迟标志
                                   
  u8 Single_Journey            : 1;//单个行程标志
  u8 Journey_Set_OK            : 1;//行程设定OK标志
  u8 Journey_Direction         : 1;//行程方向标志
                                   
  u8 Hand_Drag_Status          : 1;//手拉状态标志
  u8 No_Slow_Start             : 1;//无慢启动标志
  u8 Serpentine_Curtain        : 1;//蛇形帘标志
  u8 To_Journey_Meet_Plugb     : 1;//到行程遇阻标志
  u8 PowerDown_Remember        : 1;//断电记忆标志
  u8 Manual_Control_Synchro    : 1;//手控同步标志
  
}Flag_M;
typedef void (*Motor_Diver) (void);
typedef struct
{
  Flag_M Motor_Flag;
  Motor_Control_TypeDef  Motor_Status;
  Motor_Control_TypeDef  Motor_Status_Buffer;
  Motor_Control_TypeDef  Motor_Run_Status;              //电机实际运行状态

  u16 RunTime_Cnt;
  Motor_Diver Motor_Diver_Array[3];
}Motor_TypeDef;


typedef enum {
  START                         = (u8)0x00,
  DATA                          = (u8)0x01,
  STOP                          = (u8)0x02,
  END                           = (u8)0x03
} VUart_Event_TypeDef;
typedef struct
{
  u8 Read_Write                 : 1;//读写标志
  u8 Start_RX                   : 1;//开始接收标志
  u8 Bit_Data                   : 1;//数据位
	                                                           
  u8 Bit_Time_Cnt;//BIT时间计数器
  u8 Bit_High_Cnt;//高电平计数器
  u8 RW_Bit_Cnt;//BIT计数器  
  u8 RW_Byte_Cnt;//BYTE计数器
  u8 RXTime_Cnt;//接收时间计数器
	                                                           
  VUart_Event_TypeDef RW_Event;//读写事件
  u8 *Data_Buffer;//数据缓存
	
} VUart_TypeDef;

typedef struct
{
  u8 Control_Finish              : 1;
  u8 const VALUE;
  u8 Old_Value;
  u8 In_Cnt;
  u8 N0_In_Cnt;
  
  Motor_Control_TypeDef  Control_Command;
}Power_In_TypeDef;
//-------------------------------------------------------------------------------------
typedef struct
{
  union
  {
    struct
    {
      u8 Limit_Activate          : 1;
      u8 Fine_Adjust             : 1;//微调限位点标志
    }F1;
    u8 F8;
  }Flag;
  
  u32 Limit_Place;
}Limit;

#define WORK_STATUS_NUM           4
typedef void (*WorkStatusManage) (void);
typedef enum {
  NORMAL_MODE                    = (u8)0x00,//正常模式
  SLEEP_MODE                     = (u8)0x01,//睡眠模式
  POWER_OFF_MODE                 = (u8)0x02,//关机模式
  POWER_DOWN_MODE                = (u8)0x03,//掉电模式
} Work_Status_TypeDef;