/**********************************************************************************
*  ��Ŀ���:    
*  ��Ŀ����:    
*  �ͻ�����:    
*  �����д:   
*  ��������:    
*  �������:    
*  �汾����:    V1.0
*
*  LINUart.c file
*  ��������: 
**********************************************************************************/
#include "Define.h"//ȫ�ֺ궨��
#include "Extern.h"//ȫ�ֱ���������
//�ڲ���--------------------------------------------------------------------------------
#define BAUD_RATE_VALUE(var)   ((10000/((var)/100)) * SYSTEM_CLOCK)

#define UART1_RX_BYTE              7
#define UART1_TX_BYTE              10
#define EUSART_NO_RX_TIME          4
                                   
#define EUSART_DATA_HEAD           0x9a//�����ź�ͷ
#define EUSART_DATA_UP             0xdd//������������
#define EUSART_DATA_STOP           0xcc//����ֹͣ����
#define EUSART_DATA_DOWN           0xee//������������
#define EUSART_DATA_LEARN          0xaa//����ѧϰ����
#define EUSART_DATA_CLEAR          0xa6//�����������
#define EUSART_DATA_ORDER          0x0a//���������־
#define EUSART_DATA_INQUIRE        0xcc//���ڲ�ѯ��־
                                       
#define INQUIRE_CURTAIN            0xCA//��ѯ������״̬
#define INQUIRE_SHUTTER            0xCB//��ѯ����״̬
                                       
#define EUSART_DATA_GOTO_PCT       0xDD//���е��ٷֱ�
#define EUSART_DATA_SET_SPEED      0xD9//�趨���ת��
#define SET_DRY_CONTACT            0xD2//�趨�ֿط�ʽ
#define SET_HAND_DRAG              0xD5//�趨�����������
#define SET_BAUD_RATE              0xDA//���ڲ�����
                                       
#define SET_INCHING_TIME           0xD6//�趨�㶯ʱ��
#define SET_MEET_PLUGB_SENSITIVITY 0xD7//�趨����������
#define SET_MEET_PLUGB_REBOUND     0xD8//�趨���跴��
                                       
#define EUSART_JIGGLE_UP           0x0d//�㶯��������
#define EUSART_JIGGLE_DOWN         0x0e//�㶯��������
#define EUSART_DATA_LIMIT_1        0x01//�м���λ��1
#define EUSART_DATA_LIMIT_2        0x02//�м���λ��2
#define EUSART_DATA_LIMIT_3        0x03//�м���λ��3
#define EUSART_DATA_LIMIT_4        0x04//�м���λ��4
#define SEND_DATA_HEAD             0xd8//�����ź�ͷ
//�ڲ�����------------------------------------------------------------------------------
static u8 Uart1_Byte_Cnt;//�ֽڼ�����
static u8 Uart1_TX_Byte_Cnt;//�ֽڼ�����
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
//������:   LinUartSetting
//��  ��:   Uart ����
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
//  ������:   Uart1_Status_Inspect
//  ��  ��:   ����ͨѶ״̬���
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
//  ������:   DataCRC
//  ��  ��:   ѭ�����Ч��
//  ��  ��:   ��Ч���������ַ��Ч�����ݵĸ���
//  ��  ��:   ��    
//  ��  ��:   ���Ľ��
//  ��  ����
//-------------------------------------------------------------------------------------
u8 DataCRC (u8 array[],u8 amount)
{
  u8 crc = 0;
  u8 i;
  for (i = 1; i < amount; i++)//�Ƿ��Ѿ�ȫ�������
  {//ȡ��һ����
    crc ^= array[i];//ѭ�����
  }
  return (crc);
}

//-------------------------------------------------------------------------------------
//������:   MotorControl
//��  ��:   
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
    if (!(MotorFlag.Journey_Direction ^ MotorFlag.Direction))                         //�ٷֱ�����
  {
    percent = 100 - percent;
  }
  
  
 // percent = 100 - percent;
  
  return (percent);
}
//-------------------------------------------------------------------------------------
//������:   MotorControl
//��  ��:   
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
//������:   UartControl
//��  ��:   
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
    case EUSART_JIGGLE_UP://�㶯�������� 0x0d
      Motor.Motor_Status = MOTOR_UP;
      Flag.Jiggle_Stop = TRUE;
    break;
    case EUSART_JIGGLE_DOWN://�㶯�������� 0x0e
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
          if(MotorFlag.Journey_Direction ^ MotorFlag.Direction)//�������
          {
            Motor.Motor_Status = MOTOR_DOWN;
          }
          else//�������
          {
            Motor.Motor_Status = MOTOR_UP;
          }
        }
        else
        {
          if(MotorFlag.Journey_Direction ^ MotorFlag.Direction)//�������
          {
            Motor.Motor_Status = MOTOR_UP;
          }
          else//�������
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
    Flag.Angle_Stop = NULL ;//���¿��ƹص���LIU 
    Flag.Goto_PCT = FALSE;
    Flag.Motor_Status_Switch = TRUE;//����Ŀ��λ��
  }
  //-----------------------------------------------------------------------------------
  Motor_Status_bak=Motor.Motor_Status;
}
//-------------------------------------------------------------------------------------
//������:   Uart1StartupTX
//��  ��:   ׼�����ݣ���������
//-------------------------------------------------------------------------------------
void Uart1StartupTX(void)
{
  RS485_TX();
  
  Bit_TypeDef Uart;
  
  Uart1_TX_Array[0] = SEND_DATA_HEAD;//0xd8
  Uart1_TX_Array[1] = Uart_ID;
  Uart1_TX_Array[2] = Uart_CH_L;
  Uart1_TX_Array[3] = Uart_CH_H;

  if (Inquire_Function_Flag == INQUIRE_CURTAIN)//��ѯ������״̬ 0xca
  {
    Uart1_TX_Array[4] = Uart_Baud_Rate;
    Uart1_TX_Array[5] = Dry_Contact_Control_Mode;
    Uart1_TX_Array[6] = Motor_Max_Speed;
    Uart1_TX_Array[7] = INQUIRE_CURTAIN;//��������״̬ 0xca
    Uart.Flag_Byte = 0;
    Uart.Flag_Bit.b0 = 1;//����������
    Uart.Flag_Bit.b1 = MotorFlag.Direction;//Motor_Flag.Direction
    Uart.Flag_Bit.b2 = MotorFlag.Motor_Jiggle_Status;//�㶯 ����״̬
    Uart.Flag_Bit.b3 = 0;
    Uart.Flag_Bit.b4 = 0;
    Uart.Flag_Bit.b5 = 0;
    Uart.Flag_Bit.b6 = 0;
    Uart.Flag_Bit.b7 = 1;
    Uart1_TX_Array[8] = Uart.Flag_Byte;
    
  }
  //------------------------------------------------------------------------------
  else if (Inquire_Function_Flag == INQUIRE_SHUTTER)//��ѯ����״̬ 0xcb
  {
    Uart1_TX_Array[4] = Joint_Time_Level;//�㶯ʱ��
    //------------------------------------------------------------------
    if (Meet_Plugb_Level_Buff & 0x80)//����������
    {
      Uart1_TX_Array[5] = (Meet_Plugb_Level | 0x80);
    }
    else
    {
      Uart1_TX_Array[5] = Meet_Plugb_Level;
    }
    //------------------------------------------------------------------
    Uart1_TX_Array[6] = 0;//���跴��
    
    Uart1_TX_Array[7] = INQUIRE_SHUTTER;//��������״̬ 0xcb
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
  
  Uart1_TX_Byte_Cnt = 0;//���ʹ�����0
  UCA1IE &= ~UCRXIE;
  UCA1IE |= UCTXCPTIE;
  UCA1IE |= UCTXIE;//Enable USCI_A0 TX interrupt
  UCA1IFG |= UCTXIFG;//�����жϱ�־�����뷢���жϳ���
}
//-------------------------------------------------------------------------------------
//������:   Uart_TX_Interrupt
//��  ��:   ���ڷ����ж�
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
           if (++Uart1_Byte_Cnt >= UART1_RX_BYTE )//�Ƿ��ѽ������
           {
             Uart1_Byte_Cnt = 0;//������ֽڼ�����
             Flag.Uart_Read_OK = TRUE;//�ý�����ɱ�־
           }
           //--------------------------------------------------------
           if (Uart1_RX_Array[0] != EUSART_DATA_HEAD)
           {
             Flag.Uart_Read_OK = FALSE;//����NO OK
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
           UCA1TXBUF = Uart1_TX_Array[Uart1_TX_Byte_Cnt];//����
           Uart1_TX_Byte_Cnt++;
         }
         else if(Uart1_TX_Byte_Cnt==UART1_TX_BYTE)
        {
          Uart1_NO_Tx_Cnt=5;
          Uart1_TX_Byte_Cnt = 0;//���ʹ�����0
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
//������:   LearnUartID
//��  ��:   
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
//������:   Uart1DataDisposal
//��  ��:   ���ڽ��յ����ݴ���
//��  ��:   ���ڽ��յ����ݻ���������ַ
//-------------------------------------------------------------------------------------
void Uart1DataDisposal (void)
{
  u8 id_same = 0;
  Bit_TypeDef Uart;

  if(DataCRC(Uart1_RX_Array,UART1_RX_BYTE)== 0)
  {
    if(Uart1_RX_Array[1] == Uart_ID)//ID�Ƿ����
    {
      //ͨ���Ƿ����
      if ((Uart1_RX_Array[2] & Uart_CH_L) || (Uart1_RX_Array[3] & Uart_CH_H))
      {
        id_same = TRUE;
      }
    }
    //---------------------------------------------------------------------------------
    //�Ƿ�ǿ���趨�����ID
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
      Flag.Uart_tx = TRUE;//����IDֱ�ӷ���
      Flag.Opretion_Finish = TRUE;
    }    
    //---------------------------------------------------------------------------------
    if ((Uart1_RX_Array[4] == EUSART_DATA_ORDER)&&//0x0a
        (Uart1_RX_Array[5] == EUSART_DATA_LEARN))//�Ƿ�Ϊѧ�� 0xaa
    {
      //ѧϰID��ͨ��
      if (Flag.Learn_Code_Statu)
      {
        LearnUartID(Uart1_RX_Array);
      }
      Flag.Opretion_Finish = TRUE;
    }
    //---------------------------------------------------------------------------------
    if ((id_same == TRUE)||(Uart1_RX_Array[1] == 0))//Ⱥ��
    {
      if (Uart1_RX_Array[4] == SET_DRY_CONTACT)//�Ƿ��趨�ֿط�ʽ 0xd2
      {
        if (Uart1_RX_Array[5] <= 4)
        {
          Dry_Contact_Control_Mode = Uart1_RX_Array[5];
          Flag.Opretion_Finish = TRUE;
        }
      } 
      //-------------------------------------------------------------------------------
      else if (Uart1_RX_Array[4] == EUSART_DATA_SET_SPEED)//0xd9
      {//�Ƿ��趨ת��,��������趨��Ч
        if ((Uart1_RX_Array[5] <= 130) && (Uart1_RX_Array[5] >= 50))
        {
          Motor_Max_Speed = Uart1_RX_Array[5];
          Flag.Opretion_Finish = TRUE;
        }
      }
      //-------------------------------------------------------------------------------
      else if (Uart1_RX_Array[4] == SET_HAND_DRAG)//�趨���򡢵㶯/����,������ 0xd5
      {
        Uart.Flag_Byte = Uart1_RX_Array[5];
        MotorFlag.Direction = Uart.Flag_Bit.b1;
        MotorFlag.Motor_Run = Uart.Flag_Bit.b2;

        Flag.Opretion_Finish = TRUE;
      }
      //-------------------------------------------------------------------------------
      else if (Uart1_RX_Array[4] == SET_BAUD_RATE)//�Ƿ��趨������ 0xda
      {
        if (Uart1_RX_Array[5] < BAUD_RATE_NUM)
        {
          Uart_Baud_Rate = (Uart_Baud_Rate_TypeDef) Uart1_RX_Array[5];
          LinUartInit();
          Flag.Opretion_Finish = TRUE;
        }
      }
      //-------------------------------------------------------------------------------
      else if (Uart1_RX_Array[4] == SET_INCHING_TIME)//�趨�㶯ʱ�� 0xd6
      {
        if (Uart1_RX_Array[5] <= 20)//1-20�㶯ʱ���ɶ̵�����ʮ���ȼ�
        {
          Joint_Time_Level = Uart1_RX_Array[5];
          Flag.Opretion_Finish = TRUE;
        }
      }
      //-------------------------------------------------------------------------------
      else if (Uart1_RX_Array[4] == SET_MEET_PLUGB_SENSITIVITY)//�趨���������� 0xd7
      {
        Meet_Plugb_Level_Buff = Uart1_RX_Array[5];//��ʱ���棬�������ݵ�ʱ���ж����辫ȷ��
        //-------------------------------------------------------------------
        if ((Uart1_RX_Array[5] >= 1) && (Uart1_RX_Array[5] <= 3))
        {//1\2\3 ���и������ȼ�
          Meet_Plugb_Level = Uart1_RX_Array[5];
          Flag.Opretion_Finish = TRUE;
        }
        //-------------------------------------------------------------------
        else if (Uart1_RX_Array[5] & 0x80)//�Ƿ�ϸ��������
        {
          Uart1_RX_Array[5] &= 0x0f;
          if ((Uart1_RX_Array[5] >= 1) && (Uart1_RX_Array[5] <= 10))
          {//�Ƿ�������1-10
            Meet_Plugb_Level = Uart1_RX_Array[5];
            Flag.Opretion_Finish = TRUE;
          }
        }
      }
      //-------------------------------------------------------------------------------
      else if ((Uart1_RX_Array[4] == INQUIRE_CURTAIN)||//0xca
               (Uart1_RX_Array[4] == INQUIRE_SHUTTER))//��ѯ����״̬ 0xcb
      {
        Inquire_Function_Flag = Uart1_RX_Array[4];//��ѯ����״̬
        Flag.Uart_tx = TRUE;//����
      }
      //-------------------------------------------------------------------------------
      else if (Uart1_RX_Array[4] == EUSART_DATA_ORDER)//�Ƿ�Ϊ�������� 0x0a
      {
        if((Uart1_RX_Array[5] == EUSART_DATA_CLEAR)&&(id_same == TRUE))//0xa6
        {
          LearnUartID(Uart1_RX_Array);
        }
        else UartControl(Uart1_RX_Array[5]);//�������
      }
      //-------------------------------------------------------------------------------
      else if (Uart1_RX_Array[4] == EUSART_DATA_INQUIRE)//��ѯ 0xcc
      {
        if ((Uart1_RX_Array[5] == 0) && (id_same == TRUE))//��ѯ
        {
          Flag.Uart_tx = TRUE;//ID��ȷ���
        }
      }
      //-------------------------------------------------------------------------------
      else if (Uart1_RX_Array[4] == EUSART_DATA_GOTO_PCT)//�������аٷֱ� 0xdd
      {
        if(Limit_Point[1].Flag.F1.Limit_Activate && Uart1_RX_Array[5]<=100)
        {      
          Flag.Goto_PCT = TRUE;
          Flag.Angle_Stop = NULL ;//�ٷֱȿ��ƹص���LIU 
          Flag.Motor_Status_Switch = TRUE;//����Ŀ��λ��
          Hall_Pulse_Amount_PCT = CountPercentPlace(Uart1_RX_Array[5]);
          
          if(Hall_Pulse_Amount_Cnt < Hall_Pulse_Amount_PCT)
          {  
            if(MotorFlag.Journey_Direction ^ MotorFlag.Direction)//�������
            {
              Motor.Motor_Status = MOTOR_DOWN;
            }
            else//�������
            {
              Motor.Motor_Status = MOTOR_UP;
            }
          }
          else
          {
            if(MotorFlag.Journey_Direction ^ MotorFlag.Direction)//�������
            {
              Motor.Motor_Status = MOTOR_UP;
            }
            else//�������
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
