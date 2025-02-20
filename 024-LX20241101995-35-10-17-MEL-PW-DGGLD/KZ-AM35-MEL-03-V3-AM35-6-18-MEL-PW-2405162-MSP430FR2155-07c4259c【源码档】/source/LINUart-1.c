/**********************************************************************************
*  项目编号:    
*  项目名称:    
*  客户名称:    
*  程序编写:   
*  启动日期:    
*  完成日期:    
*  版本号码:    V1.0
*
*  LINUart.c file
*  功能描述: 
**********************************************************************************/
#include "Define.h"//全局宏定义
#include "Extern.h"//全局变量及函数
//内部宏--------------------------------------------------------------------------------
#define BAUD_RATE_VALUE(var)   ((10000/((var)/100)) * SYSTEM_CLOCK)

#define UART1_RX_BYTE              7
#define UART1_TX_BYTE              10
#define EUSART_NO_RX_TIME          4
                                   
#define EUSART_DATA_HEAD           0x9a//接收信号头
#define EUSART_DATA_UP             0xdd//串口上行数据
#define EUSART_DATA_STOP           0xcc//串口停止数据
#define EUSART_DATA_DOWN           0xee//串口下行数据
#define EUSART_DATA_LEARN          0xaa//串口学习数据
#define EUSART_DATA_CLEAR          0xa6//串口清除数据
#define EUSART_DATA_ORDER          0x0a//串口命令标志
#define EUSART_DATA_INQUIRE        0xcc//串口查询标志
                                       
#define INQUIRE_CURTAIN            0xCA//查询开合帘状态
#define INQUIRE_SHUTTER            0xCB//查询卷帘状态
                                       
#define EUSART_DATA_GOTO_PCT       0xDD//运行到百分比
#define EUSART_DATA_SET_SPEED      0xD9//设定最高转速
#define SET_DRY_CONTACT            0xD2//设定手控方式
#define SET_HAND_DRAG              0xD5//设定手拉、方向等
#define SET_BAUD_RATE              0xDA//串口波特率
                                       
#define SET_INCHING_TIME           0xD6//设定点动时间
#define SET_MEET_PLUGB_SENSITIVITY 0xD7//设定遇阻灵敏度
#define SET_MEET_PLUGB_REBOUND     0xD8//设定遇阻反弹
                                       
#define EUSART_JIGGLE_UP           0x0d//点动上行数据
#define EUSART_JIGGLE_DOWN         0x0e//点动下行数据
#define EUSART_DATA_LIMIT_1        0x01//中间限位点1
#define EUSART_DATA_LIMIT_2        0x02//中间限位点2
#define EUSART_DATA_LIMIT_3        0x03//中间限位点3
#define EUSART_DATA_LIMIT_4        0x04//中间限位点4
#define SEND_DATA_HEAD             0xd8//发送信号头
//内部变量------------------------------------------------------------------------------
static u8 Uart1_Byte_Cnt;//字节计数器
static u8 Uart1_TX_Byte_Cnt;//字节计数器
static u8 Uart1_TX_Array[UART1_TX_BYTE];
static u8 Uart1_RX_Array[UART1_RX_BYTE];

static u8 Uart1_NO_Rx_Cnt;
static u8 Uart1_NO_Tx_Cnt;
static u8 Inquire_Function_Flag;
static u8 Meet_Plugb_Level_Buff;

const u16 Baud_Rate_Array [BAUD_RATE_NUM] = 
{
  1200,2400,4800,9600,19200
};
//-------------------------------------------------------------------------------------
//函数名:   LinUartSetting
//功  能:   Uart 设置
//-------------------------------------------------------------------------------------
void LinUartInit (void)
{
  if (Uart_Baud_Rate >= BAUD_RATE_NUM)
  {
    Uart_Baud_Rate = BR9600;
  }
  //Configure UART 1 pins
  P4SEL0 |= BIT2|BIT3;//set 2-UART pin as second function
  
  UCA1CTLW0 |= UCSWRST;//software reset enable
  UCA1CTLW0 |= UCSSEL__SMCLK;//UCSSEL__SMCLK
  
  UCA1BR0 = BAUD_RATE_VALUE(Baud_Rate_Array[Uart_Baud_Rate]) & 0xFF;//INT(32768/9600)
  UCA1BR1 = BAUD_RATE_VALUE(Baud_Rate_Array[Uart_Baud_Rate]) >> 8;
  
  UCA1CTLW0 &= ~UCSWRST;//Initialize eUSCI
  UCA1IE |= UCRXIE;  
  
  RS485_RX();
}

//-------------------------------------------------------------------------------------
//  函数名:   Uart1_Status_Inspect
//  功  能:   串口通讯状态检测
//-------------------------------------------------------------------------------------
void Uart1_Status_Inspect(void)
{
  if(Uart1_NO_Rx_Cnt)
  {
    Uart1_NO_Rx_Cnt--;
    if(Uart1_NO_Rx_Cnt==0)
    {
      Uart1_NO_Rx_Cnt = 0;
      Uart1_Byte_Cnt = 0;
    }
  }
  //----------------------------------------------------------------------
  if(Uart1_NO_Tx_Cnt)
  { 
    Uart1_NO_Tx_Cnt--;
    if(Uart1_NO_Tx_Cnt==0)
    { 
      RS485_RX();
      //----------------------------------------------------------
      UCA1CTLW0 &= ~UCSWRST;//Initialize eUSCI
      UCA1IE |= UCRXIE;//Enable USCI_A0 RX interrupt
      //----------------------------------------------------------
      Uart1_NO_Tx_Cnt = 0;
    }
  }
}
//-------------------------------------------------------------------------------------
//  函数名:   DataCRC
//  功  能:   循环异或效验
//  输  入:   需效验的数组首址、效验数据的个数
//  输  出:   空    
//  返  回:   异或的结果
//  描  述：
//-------------------------------------------------------------------------------------
u8 DataCRC (u8 array[],u8 amount)
{
  u8 crc = 0;
  u8 i;
  for (i = 1; i < amount; i++)//是否已经全部异或完
  {//取下一个数
    crc ^= array[i];//循环异或
  }
  return (crc);
}

//-------------------------------------------------------------------------------------
//函数名:   MotorControl
//功  能:   
//-------------------------------------------------------------------------------------
u8 JourneyPercentCount (void)
{
  u8 percent;
  u32 i;
  u32 temp;
  
    if(!Limit_Point[1].Flag.F1.Limit_Activate) return 0x00;                      //LIU
  
  temp = Limit_Point[Limit_Point_Amount-1].Limit_Place - Limit_Point[0].Limit_Place;
  i = Hall_Pulse_Amount_Cnt - Limit_Point[0].Limit_Place;
  if(Hall_Pulse_Amount_Cnt > Limit_Point[Limit_Point_Amount-1].Limit_Place-5)
  {
    percent = 100;   
  }
  else if(Hall_Pulse_Amount_Cnt < Limit_Point[0].Limit_Place+5)
  {
    percent = 0;       
  }
  else
  {
    percent = ((i * 100) / temp);
  }
  
  if (percent > 100)
  {
    percent = 100;
  }
    if (!(MotorFlag.Journey_Direction ^ MotorFlag.Direction))                         //百分比限制
  {
    percent = 100 - percent;
  }
  
  
 // percent = 100 - percent;
  
  return (percent);
}
//-------------------------------------------------------------------------------------
//函数名:   MotorControl
//功  能:   
//-------------------------------------------------------------------------------------
u32 CountPercentPlace (u8 percent)
{
  u32 temp;
  u32 place;
  
  temp = Limit_Point[Limit_Point_Amount-1].Limit_Place - Limit_Point[0].Limit_Place;
  place = Limit_Point[0].Limit_Place;

  if (MotorFlag.Journey_Direction ^ MotorFlag.Direction)
  {
    percent = 100 - percent;
  }
  
  if (temp < 600)
  {
    place =  Limit_Point[Limit_Point_Amount-1].Limit_Place - ((u16) ((temp * percent) / 100));
  }
  else if (temp < 2400)
  {
    place = Limit_Point[Limit_Point_Amount-1].Limit_Place - ((u16) (((temp >> 2) * percent) / 25));
  }
  else if (temp < 6000)
  {
    place = Limit_Point[Limit_Point_Amount-1].Limit_Place - ((u16) (((temp / 10) * percent) / 10));
  }
  else
  {
    place = Limit_Point[Limit_Point_Amount-1].Limit_Place - ((u16) (((temp / 100) * percent)));
  }
  return ((u32) place);
}
//-------------------------------------------------------------------------------------
//函数名:   UartControl
//功  能:   
//-------------------------------------------------------------------------------------
void UartControl (u8 data)
{
  switch (data)
  {
    case EUSART_DATA_UP://0xdd
      Motor.Motor_Status = MOTOR_UP;
      break;
    case EUSART_DATA_STOP://0xcc
      Motor.Motor_Status = MOTOR_STOP;
      MotorFlag.Motor_Run = FALSE;
      break;
    case EUSART_DATA_DOWN://0xee
      Motor.Motor_Status = MOTOR_DOWN;
      break;
    case EUSART_JIGGLE_UP://点动上行数据 0x0d
      Motor.Motor_Status = MOTOR_UP;
      Flag.Jiggle_Stop = TRUE;
    break;
    case EUSART_JIGGLE_DOWN://点动下行数据 0x0e
      Motor.Motor_Status = MOTOR_DOWN;
      Flag.Jiggle_Stop = TRUE;
      break;  
    case EUSART_DATA_LIMIT_1://0x01
    case EUSART_DATA_LIMIT_2://0x02
    case EUSART_DATA_LIMIT_3://0x03
    case EUSART_DATA_LIMIT_4://0x04
      if (Limit_Point_Amount >= data+2)
      {
        Run_To_Mid_Limit_Point = data;
        Motor.Motor_Status = MOTOR_DOWN;
        //-------------------------------------------------------------------------------------------
        if(Hall_Pulse_Amount_Cnt < Limit_Point[Run_To_Mid_Limit_Point].Limit_Place)
        { 
          if(MotorFlag.Journey_Direction ^ MotorFlag.Direction)//电机正向
          {
            Motor.Motor_Status = MOTOR_DOWN;
          }
          else//电机反向
          {
            Motor.Motor_Status = MOTOR_UP;
          }
        }
        else
        {
          if(MotorFlag.Journey_Direction ^ MotorFlag.Direction)//电机正向
          {
            Motor.Motor_Status = MOTOR_UP;
          }
          else//电机反向
          {
            Motor.Motor_Status = MOTOR_DOWN;
          }
        }
        //-------------------------------------------------------------------------------------------
        if ((Hall_Pulse_Amount_Cnt > (Limit_Point[Run_To_Mid_Limit_Point].Limit_Place - 10))
        && (Hall_Pulse_Amount_Cnt < (Limit_Point[Run_To_Mid_Limit_Point].Limit_Place + 10)))
        {
          MotorFlag.Motor_Run = FALSE;
          Motor.Motor_Status = MOTOR_STOP;
          Run_To_Mid_Limit_Point = 0;
        }
      }
      break;
  }
  //-----------------------------------------------------------------------------------
  //if (Flag.Goto_PCT)
  {
    Flag.Angle_Stop = NULL ;//上下控制关调光LIU 
    Flag.Goto_PCT = FALSE;
    Flag.Motor_Status_Switch = TRUE;//更新目标位置
  }
  //-----------------------------------------------------------------------------------
  Motor_Status_bak=Motor.Motor_Status;
}
//-------------------------------------------------------------------------------------
//函数名:   Uart1StartupTX
//功  能:   准备数据，启动发送
//-------------------------------------------------------------------------------------
void Uart1StartupTX(void)
{
  RS485_TX();
  
  Bit_TypeDef Uart;
  
  Uart1_TX_Array[0] = SEND_DATA_HEAD;//0xd8
  Uart1_TX_Array[1] = Uart_ID;
  Uart1_TX_Array[2] = Uart_CH_L;
  Uart1_TX_Array[3] = Uart_CH_H;

  if (Inquire_Function_Flag == INQUIRE_CURTAIN)//查询开合帘状态 0xca
  {
    Uart1_TX_Array[4] = Uart_Baud_Rate;
    Uart1_TX_Array[5] = Dry_Contact_Control_Mode;
    Uart1_TX_Array[6] = Motor_Max_Speed;
    Uart1_TX_Array[7] = INQUIRE_CURTAIN;//反馈功能状态 0xca
    Uart.Flag_Byte = 0;
    Uart.Flag_Bit.b0 = 1;//无手拉启动
    Uart.Flag_Bit.b1 = MotorFlag.Direction;//Motor_Flag.Direction
    Uart.Flag_Bit.b2 = MotorFlag.Motor_Jiggle_Status;//点动 连动状态
    Uart.Flag_Bit.b3 = 0;
    Uart.Flag_Bit.b4 = 0;
    Uart.Flag_Bit.b5 = 0;
    Uart.Flag_Bit.b6 = 0;
    Uart.Flag_Bit.b7 = 1;
    Uart1_TX_Array[8] = Uart.Flag_Byte;
    
  }
  //------------------------------------------------------------------------------
  else if (Inquire_Function_Flag == INQUIRE_SHUTTER)//查询卷帘状态 0xcb
  {
    Uart1_TX_Array[4] = Joint_Time_Level;//点动时间
    //------------------------------------------------------------------
    if (Meet_Plugb_Level_Buff & 0x80)//遇阻灵敏度
    {
      Uart1_TX_Array[5] = (Meet_Plugb_Level | 0x80);
    }
    else
    {
      Uart1_TX_Array[5] = Meet_Plugb_Level;
    }
    //------------------------------------------------------------------
    Uart1_TX_Array[6] = 0;//遇阻反弹
    
    Uart1_TX_Array[7] = INQUIRE_SHUTTER;//反馈功能状态 0xcb
    Uart1_TX_Array[8] = 0;
  }
  //------------------------------------------------------------------------------
  else
  {
    Uart1_TX_Array[4] = 0;
    Uart1_TX_Array[5] = 0;
    Uart1_TX_Array[6] = 0;
    
    if (Limit_Point[1].Flag.F1.Limit_Activate)
    {
      Uart1_TX_Array[7] = JourneyPercentCount();
    }
    else
    {
      Uart1_TX_Array[7] = 0;
    }
    
    Uart.Flag_Byte = 0;
    
    Uart.Flag_Bit.b0 = Motor.Motor_Status;
    Uart.Flag_Bit.b1 = Limit_Point[1].Flag.F1.Limit_Activate;
    //---------------------------------------------------------------
    if (Limit_Point_Amount == 6)
    {
      Uart.Flag_Bit.b2 = TRUE;
      Uart.Flag_Bit.b3 = TRUE;
      Uart.Flag_Bit.b4 = TRUE;
      Uart.Flag_Bit.b5 = TRUE;
    }
    else if (Limit_Point_Amount == 5)
    {
      Uart.Flag_Bit.b2 = TRUE;
      Uart.Flag_Bit.b3 = TRUE;
      Uart.Flag_Bit.b4 = TRUE;
    }
    else if (Limit_Point_Amount == 4)
    {
      Uart.Flag_Bit.b2 = TRUE;
      Uart.Flag_Bit.b3 = TRUE;
    }
    else if (Limit_Point_Amount == 3)
    {
      Uart.Flag_Bit.b2 = TRUE;
    }
    //---------------------------------------------------------------
    Uart.Flag_Bit.b6 = Flag.Goto_PCT;
    
    Uart1_TX_Array[8] = Uart.Flag_Byte;
  }
  
  Inquire_Function_Flag = 0;
  Uart1_TX_Array[9] = DataCRC(Uart1_TX_Array,9);
  
  Uart1_TX_Byte_Cnt = 0;//发送次数清0
  UCA1IE &= ~UCRXIE;
  UCA1IE |= UCTXCPTIE;
  UCA1IE |= UCTXIE;//Enable USCI_A0 TX interrupt
  UCA1IFG |= UCTXIFG;//设置中断标志，进入发送中断程序
}
//-------------------------------------------------------------------------------------
//函数名:   Uart_TX_Interrupt
//功  能:   串口发送中断
//-------------------------------------------------------------------------------------
#if defined(__TI_COMPILER_VERSION__) || defined(__IAR_SYSTEMS_ICC__)
#pragma vector=USCI_A1_VECTOR
__interrupt void USCI_A1_ISR(void)
#elif defined(__GNUC__)
void __attribute__ ((interrupt(USCI_A1_VECTOR))) USCI_A1_ISR (void)
#else
#error Compiler not supported!
#endif
{
  switch(__even_in_range(UCA1IV,USCI_UART_UCTXCPTIFG))
  {
    case USCI_NONE: break;
    case USCI_UART_UCRXIFG:
         if (!Flag.Uart_Read_OK)
         {
           Uart1_RX_Array[Uart1_Byte_Cnt] = UCA1RXBUF;
           if (++Uart1_Byte_Cnt >= UART1_RX_BYTE )//是否已接收完成
           {
             Uart1_Byte_Cnt = 0;//清接收字节计数器
             Flag.Uart_Read_OK = TRUE;//置接收完成标志
           }
           //--------------------------------------------------------
           if (Uart1_RX_Array[0] != EUSART_DATA_HEAD)
           {
             Flag.Uart_Read_OK = FALSE;//接收NO OK
             Uart1_Byte_Cnt = 0;
           }
           else
           {
             Uart1_NO_Rx_Cnt = 5;
           }
         }
         else
         {
           UCA1RXBUF;
         }
         break;
    case USCI_UART_UCTXIFG: 
         if (Uart1_TX_Byte_Cnt < UART1_TX_BYTE)
         {
           UCA1TXBUF = Uart1_TX_Array[Uart1_TX_Byte_Cnt];//发送
           Uart1_TX_Byte_Cnt++;
         }
         else if(Uart1_TX_Byte_Cnt==UART1_TX_BYTE)
        {
          Uart1_NO_Tx_Cnt=5;
          Uart1_TX_Byte_Cnt = 0;//发送次数清0
        }
         break;
    case USCI_UART_UCSTTIFG: break;
    case USCI_UART_UCTXCPTIFG: 
         if(Uart1_TX_Byte_Cnt >= UART1_TX_BYTE)
         {
           UCA1CTLW0 &= ~UCSWRST;//Initialize eUSCI
           UCA1IE |= UCRXIE;//Enable USCI_A0 RX interrupt
         }
         break;  
    default: break;
  }
}
//-------------------------------------------------------------------------------------
//函数名:   LearnUartID
//功  能:   
//-------------------------------------------------------------------------------------
void LearnUartID (u8 array[])
{
  if(array[5] == EUSART_DATA_LEARN)//0xaa
  {
    Uart_ID = array[1];
    Uart_CH_L = array[2];
    Uart_CH_H = array[3];
  }
  //-------------------------------------------------
  else if(array[5] == EUSART_DATA_CLEAR)//0xa6
  { 
    Uart_ID = 0;
    Uart_CH_L = 0;
    Uart_CH_H = 0;
  }
  //-------------------------------------------------
  Flag.Learn_Code_Statu = FALSE;
}
//-------------------------------------------------------------------------------------
//函数名:   Uart1DataDisposal
//功  能:   串口接收的数据处理
//输  入:   串口接收的数据缓存数组首址
//-------------------------------------------------------------------------------------
void Uart1DataDisposal (void)
{
  u8 id_same = 0;
  Bit_TypeDef Uart;

  if(DataCRC(Uart1_RX_Array,UART1_RX_BYTE)== 0)
  {
    if(Uart1_RX_Array[1] == Uart_ID)//ID是否相等
    {
      //通道是否相等
      if ((Uart1_RX_Array[2] & Uart_CH_L) || (Uart1_RX_Array[3] & Uart_CH_H))
      {
        id_same = TRUE;
      }
    }
    //---------------------------------------------------------------------------------
    //是否强制设定或清除ID
    if ((Uart1_RX_Array[4] == EUSART_DATA_LEARN)||//0xaa
        (Uart1_RX_Array[4] == EUSART_DATA_CLEAR))//0xa6
    {
      LearnUartID(Uart1_RX_Array);
      Flag.Opretion_Finish = TRUE;
    }
    //---------------------------------------------------------------------------------
    if ((Uart1_RX_Array[4] == EUSART_DATA_INQUIRE)&&//0xcc
        (Uart1_RX_Array[5] == EUSART_DATA_INQUIRE))//0xcc
    {
      Flag.Uart_tx = TRUE;//不管ID直接反馈
      Flag.Opretion_Finish = TRUE;
    }    
    //---------------------------------------------------------------------------------
    if ((Uart1_RX_Array[4] == EUSART_DATA_ORDER)&&//0x0a
        (Uart1_RX_Array[5] == EUSART_DATA_LEARN))//是否为学码 0xaa
    {
      //学习ID和通道
      if (Flag.Learn_Code_Statu)
      {
        LearnUartID(Uart1_RX_Array);
      }
      Flag.Opretion_Finish = TRUE;
    }
    //---------------------------------------------------------------------------------
    if ((id_same == TRUE)||(Uart1_RX_Array[1] == 0))//群控
    {
      if (Uart1_RX_Array[4] == SET_DRY_CONTACT)//是否设定手控方式 0xd2
      {
        if (Uart1_RX_Array[5] <= 4)
        {
          Dry_Contact_Control_Mode = Uart1_RX_Array[5];
          Flag.Opretion_Finish = TRUE;
        }
      } 
      //-------------------------------------------------------------------------------
      else if (Uart1_RX_Array[4] == EUSART_DATA_SET_SPEED)//0xd9
      {//是否设定转速,交流电机设定无效
        if ((Uart1_RX_Array[5] <= 130) && (Uart1_RX_Array[5] >= 50))
        {
          Motor_Max_Speed = Uart1_RX_Array[5];
          Flag.Opretion_Finish = TRUE;
        }
      }
      //-------------------------------------------------------------------------------
      else if (Uart1_RX_Array[4] == SET_HAND_DRAG)//设定方向、点动/连动,开合帘 0xd5
      {
        Uart.Flag_Byte = Uart1_RX_Array[5];
        MotorFlag.Direction = Uart.Flag_Bit.b1;
        MotorFlag.Motor_Run = Uart.Flag_Bit.b2;

        Flag.Opretion_Finish = TRUE;
      }
      //-------------------------------------------------------------------------------
      else if (Uart1_RX_Array[4] == SET_BAUD_RATE)//是否设定波特率 0xda
      {
        if (Uart1_RX_Array[5] < BAUD_RATE_NUM)
        {
          Uart_Baud_Rate = (Uart_Baud_Rate_TypeDef) Uart1_RX_Array[5];
          LinUartInit();
          Flag.Opretion_Finish = TRUE;
        }
      }
      //-------------------------------------------------------------------------------
      else if (Uart1_RX_Array[4] == SET_INCHING_TIME)//设定点动时间 0xd6
      {
        if (Uart1_RX_Array[5] <= 20)//1-20点动时间由短到长二十个等级
        {
          Joint_Time_Level = Uart1_RX_Array[5];
          Flag.Opretion_Finish = TRUE;
        }
      }
      //-------------------------------------------------------------------------------
      else if (Uart1_RX_Array[4] == SET_MEET_PLUGB_SENSITIVITY)//设定遇阻灵敏度 0xd7
      {
        Meet_Plugb_Level_Buff = Uart1_RX_Array[5];//暂时保存，返回数据的时候判断遇阻精确度
        //-------------------------------------------------------------------
        if ((Uart1_RX_Array[5] >= 1) && (Uart1_RX_Array[5] <= 3))
        {//1\2\3 低中高三个等级
          Meet_Plugb_Level = Uart1_RX_Array[5];
          Flag.Opretion_Finish = TRUE;
        }
        //-------------------------------------------------------------------
        else if (Uart1_RX_Array[5] & 0x80)//是否细调灵敏度
        {
          Uart1_RX_Array[5] &= 0x0f;
          if ((Uart1_RX_Array[5] >= 1) && (Uart1_RX_Array[5] <= 10))
          {//是否灵敏度1-10
            Meet_Plugb_Level = Uart1_RX_Array[5];
            Flag.Opretion_Finish = TRUE;
          }
        }
      }
      //-------------------------------------------------------------------------------
      else if ((Uart1_RX_Array[4] == INQUIRE_CURTAIN)||//0xca
               (Uart1_RX_Array[4] == INQUIRE_SHUTTER))//查询功能状态 0xcb
      {
        Inquire_Function_Flag = Uart1_RX_Array[4];//查询功能状态
        Flag.Uart_tx = TRUE;//反馈
      }
      //-------------------------------------------------------------------------------
      else if (Uart1_RX_Array[4] == EUSART_DATA_ORDER)//是否为控制命令 0x0a
      {
        if((Uart1_RX_Array[5] == EUSART_DATA_CLEAR)&&(id_same == TRUE))//0xa6
        {
          LearnUartID(Uart1_RX_Array);
        }
        else UartControl(Uart1_RX_Array[5]);//控制输出
      }
      //-------------------------------------------------------------------------------
      else if (Uart1_RX_Array[4] == EUSART_DATA_INQUIRE)//查询 0xcc
      {
        if ((Uart1_RX_Array[5] == 0) && (id_same == TRUE))//查询
        {
          Flag.Uart_tx = TRUE;//ID相等反馈
        }
      }
      //-------------------------------------------------------------------------------
      else if (Uart1_RX_Array[4] == EUSART_DATA_GOTO_PCT)//控制运行百分比 0xdd
      {
        if(Limit_Point[1].Flag.F1.Limit_Activate && Uart1_RX_Array[5]<=100)
        {      
          Flag.Goto_PCT = TRUE;
          Flag.Angle_Stop = NULL ;//百分比控制关调光LIU 
          Flag.Motor_Status_Switch = TRUE;//更新目标位置
          Hall_Pulse_Amount_PCT = CountPercentPlace(Uart1_RX_Array[5]);
          
          if(Hall_Pulse_Amount_Cnt < Hall_Pulse_Amount_PCT)
          {  
            if(MotorFlag.Journey_Direction ^ MotorFlag.Direction)//电机正向
            {
              Motor.Motor_Status = MOTOR_DOWN;
            }
            else//电机反向
            {
              Motor.Motor_Status = MOTOR_UP;
            }
          }
          else
          {
            if(MotorFlag.Journey_Direction ^ MotorFlag.Direction)//电机正向
            {
              Motor.Motor_Status = MOTOR_UP;
            }
            else//电机反向
            {
              Motor.Motor_Status = MOTOR_DOWN;
            }
          }
          //------------------------------------------------------------------
          if ((Hall_Pulse_Amount_Cnt > (Hall_Pulse_Amount_PCT - 5))
              && (Hall_Pulse_Amount_Cnt < (Hall_Pulse_Amount_PCT + 5)))
          { 
            MotorFlag.Motor_Run = FALSE;
            Motor.Motor_Status = MOTOR_STOP;
            Flag.Goto_PCT = FALSE;

          }
          //------------------------------------------------------------------
          Motor_Status_bak=Motor.Motor_Status;
        }
      }
    }
  }
}
