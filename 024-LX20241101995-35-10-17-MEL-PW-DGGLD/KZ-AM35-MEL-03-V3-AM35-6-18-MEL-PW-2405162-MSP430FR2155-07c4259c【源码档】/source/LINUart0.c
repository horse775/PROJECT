/**********************************************************************************
 *  ��Ŀ���:    AC235-01
 *  ��Ŀ����:    ���������������
 *  �ͻ�����:                                    
 *  �����д:    ��С��
 *  ��������:    2013-01-31
 *  �������:    
 *  �汾����:    V1.1
 *
 *  LINUart.c file
 *  ��������: 
**********************************************************************************/
#include "Define.h"                                          /* ȫ�ֺ궨�� ------*/
#include "Extern.h"                                          /* ȫ�ֱ��������� --*/
/* �ڲ��� -----------------------------------------------------------------------*/
#define BAUD_RATE              9600                          // ������
#define BAUD_RATE_VALUE(var)   ((10000/((var)/100)) * SYSTEM_CLOCK)

//#define UART_MAX        35//���ڷ��ͽ��ջ�������С
#define RS485_Tx        (P1OUT&BIT0)

#define MOTOR_TYPE      1
/* �ڲ����� ---------------------------------------------------------------------*/

/* �ڲ����� ---------------------------------------------------------------------*/
static u8  //flag_answer,//����Ӧ���־
            //flag_err,//
            rx_i,//����Byte����
            tx_i,//����Byte����
            tx_len,//�ܷ���Byte��
         //   Uart_buf[UART_MAX];//Uart������
            
            
            
Uart_buf_rx[UART_MAX],
Uart_buf_tx[UART_MAX];
       
u8  tx_BACK_STATUS; //�����������ϱ�
u8  tx_BACK_STATUS_2; //�ָ�����
//const u8 TYPE_ID[21]={"4D47uCTGNqKBSdHm1.0.0"};
//const u8 TYPE_ID[21]={"8zyrliceu859rtqa1.0.0"};//20200831yeznxbktdfwvbjmagc
const u8 TYPE_ID[43] = 
{
  0x7B,0x22,0x70,0x22,0x3A,0x22,0x38,0x7A,0x79,0x72, 
  0x6C,0x69,0x63,0x65,0x75,0x38,0x35,0x39,0x72,0x74,
  0x71,0x61,0x22,0x2C,0x22,0x76,0x22,0x3A,0x22,0x31, 
  0x2E,0x30,0x2E,0x30,0x22,0x2C,0x22,0x6D,0x22,0x3A, 
  0x30,0x7D,
};
/**********************************************************************************
  ������:   LinUartSetting
  ��  ��:   Uart ����
**********************************************************************************/
void LinUart0Init (void)
{
   // Configure UART 1 pins
  P1SEL0 |= BIT6|BIT7;//set 2-UART pin as second function
  // Configure UART
  UCA0CTLW0 |= UCSWRST;
  UCA0CTLW0 |= UCSSEL__SMCLK;
  // Baud Rate calculation
  // 8000000/(16*9600) = 52.083
  // Fractional portion = 0.083
  // User's Guide Table 14-4: UCBRSx = 0x49
  // UCBRFx = int ( (52.083-52)*16) = 1
  UCA0BR0 = BAUD_RATE_VALUE(BAUD_RATE) & 0xFF;         // 
  UCA0BR1 = BAUD_RATE_VALUE(BAUD_RATE) >> 8; 
  //UCA0MCTLW = 0x4900 | UCOS16 | UCBRF_1;

  UCA0CTLW0 &= ~UCSWRST;//Initialize eUSCI
  UCA0IE |= UCRXIE;//Enable USCI_A0 RX interrupt
}

u8 sum(u8 *s,u8 n)                         //У��
{ 
  u8 t=0;
  while(n--)
  { t+=*s++;
  }
  return t;
}

/*********************************************************************************
  ������:   Uart_0_Interrupt
  ��  ��:   ����0�ж�
*********************************************************************************/
#if defined(__TI_COMPILER_VERSION__) || defined(__IAR_SYSTEMS_ICC__)
#pragma vector=USCI_A0_VECTOR
__interrupt void USCI_A0_ISR(void)
#elif defined(__GNUC__)
void __attribute__ ((interrupt(USCI_A0_VECTOR))) USCI_A0_ISR (void)
#else
#error Compiler not supported!
#endif
{
  switch(__even_in_range(UCA0IV,USCI_UART_UCTXCPTIFG))
  {
  case USCI_NONE: break;
  case USCI_UART_UCRXIFG://�����ж�LIU
    {
      t50ms_c = 0;
      if(rx_i<50)
      {
        if(rx_i==0)
        {
          Uart_buf_rx[0]=UCA0RXBUF;
          if(Uart_buf_rx[0]==0x55)rx_i++;
        }
        else if(rx_i==1)
        {
          Uart_buf_rx[1]=UCA0RXBUF;
          if(Uart_buf_rx[1]==0xAA)rx_i++;
          else if(Uart_buf_rx[1]==0x55)rx_i=1;
          else rx_i=0;
        }
        else if(rx_i==2)
        {
          Uart_buf_rx[2]=UCA0RXBUF;
          if(Uart_buf_rx[2]==0x00)rx_i++;
          else if(Uart_buf_rx[2]==0xAA)rx_i=1;
          else rx_i=0;
        }
        else 
        {
          Uart_buf_rx[rx_i++]=UCA0RXBUF;
          if((rx_i >= 7) && (rx_i >= (7+Uart_buf_rx[5])))
          {
            flag_rx = 1;         //�������
          }
        }
      }
      else 
      {
        rx_tc=UCA0RXBUF;
      }
      rx_tc=3;
    }break;
    
  case USCI_UART_UCTXIFG://�����ж�LIU
    {
      t50ms_c = 0;
      if(tx_i < tx_len)
      {
        UCA0TXBUF=Uart_buf_tx[tx_i];            //���ݷ���
        tx_i++;
      }
      else
      {
        UCA0CTLW0 &= ~UCSWRST;//Initialize eUSCI
        UCA0IE &= ~UCTXIE;                       //  Disable USCI_A0 TX interrupt
        UCA0IE |= UCRXIE;                        //  Enable USCI_A0 RX interrupt
        tx_ok_tc=1;
        tx_i =0;
      }
    }break;
    
  case USCI_UART_UCSTTIFG: break;
  case USCI_UART_UCTXCPTIFG: break;
  default: break;
  }
}

//-------------------------------------------------------------------------------------
//��������
//-------------------------------------------------------------------------------------
void Start_TX(void)
{
  Uart_buf_tx[0]=0x55;
  Uart_buf_tx[1]=0xAA;
  Uart_buf_tx[2]=0x03;
  //RS485_TX();
  Uart_buf_tx[6 + Uart_buf_tx[5]] = sum(Uart_buf_tx, 6 + Uart_buf_tx[5]); //У��
  tx_len = 6 + Uart_buf_tx[5] + 1;
  tx_i=0;
  SendTX_interval_Time = 0;               //���ͺ��һ��ʱ���������      
  
  UCA0IE &= ~UCRXIE;
  UCA0IE |= UCTXIE;
  UCA0IFG |= UCTXIFG;                     // �����жϱ�־�����뷢���жϳ���
}
//-------------------------------------------------------------------------------------
//������:   DataCRC
//��  ��:   У��
//��  ��:   ptrΪУ�������׵�ַ��lenΪУ�������ܳ���
//��  ��:   ��    
//��  ��:   ��������CRC-8/MAXIMУ��ֵ
//��  ����
//-------------------------------------------------------------------------------------
u8 crc8_chk_value(u8 *ptr, u8 len)
{
  u8 crc = 0;
  u8 i;
  while(len--)
  {
    crc ^= *ptr++;
    for(i = 0;i < 8;i++)
    {
      if(crc& 0x01)
      {
        crc = crc>> 1;
        crc = crc ^ 0x8c;
      }
      else
      {
        crc>>= 1;
      }
    }
  }
  return crc; 
}

void UartControl_WIFI (u8 data)                   //�������  HUAWEIģ��
{
  switch (data)
  {
  case 0:
    Motor.Motor_Status = MOTOR_UP;
    Flag.Jiggle_Stop = NULL;
#ifdef INCHING_FIXED_ANGLE
    if (((MotorFlag.Motor_Jiggle_Status) || (Flag.Set_Limit_Point) ||
         (Flag.Joint_Level_Adj)) && (Limit_Point[1].Flag.F1.Limit_Activate))//Ĭ��Ϊ�㶯ģʽ
    {
      Flag.Jiggle_Stop = TRUE;
      Flag.RF_Data_Disposal = TRUE;
      Hall_Pulse_Jiggle_Buff = Hall_Pulse_Amount_Cnt;//���浱ǰλ��
      
      Joint_Action_Motor_Buf = Motor.Motor_Status;
      Flag.Joint_Action = NULL;
      Joint_Action_Time_Cnt = 0;
    }
#endif
    break;
  case 1:
    Motor.Motor_Status = MOTOR_STOP;
    MotorFlag.Motor_Run = NULL;
    break;
  case 2:
    Motor.Motor_Status = MOTOR_DOWN;
    Flag.Jiggle_Stop = NULL;
    
#ifdef INCHING_FIXED_ANGLE
    if (((MotorFlag.Motor_Jiggle_Status) || (Flag.Set_Limit_Point) ||
         (Flag.Joint_Level_Adj)) && (Limit_Point[1].Flag.F1.Limit_Activate))//Ĭ��Ϊ�㶯ģʽ
    {
      Flag.Jiggle_Stop = TRUE;
      Flag.RF_Data_Disposal = TRUE;
      Hall_Pulse_Jiggle_Buff = Hall_Pulse_Amount_Cnt;//���浱ǰλ��
      
      Joint_Action_Motor_Buf = Motor.Motor_Status;
      Flag.Joint_Action = NULL;
      Joint_Action_Time_Cnt = 0;
    }
#endif
    break;
  }
  
    Flag.Goto_PCT = FALSE;
    Flag.Angle_Stop = NULL ;//�ص���LIU 
    Flag.Motor_Status_Switch = TRUE;//����Ŀ��λ��
}

void UART_TEXT(u8 DATE_1,u8 DATE_2,u8 DATE_3,u8 DATE_4,u8 DATE_5,u8 DATE_6)                //�ϴ�����״̬�����ݴ���
{

        Uart_buf_tx[0]=DATE_1;
        Uart_buf_tx[1]=DATE_2;
        Uart_buf_tx[2]=DATE_3;
        Uart_buf_tx[3]=DATE_4;
        Uart_buf_tx[4]=DATE_5;
        Uart_buf_tx[5]=DATE_6;    
          //tx_len=6; 
           Start_TX();              //���ڷ���ʹ��

}

/*----------------------------
,����ID,����ID,��������,tagid
0,changeDirection,action,enum,0x2d33             ����  100��  200��
1,rotationAction,action,enum,0xf3a2              �㶯����ģʽ �㶯0 ����1
2,opener,current,int,0xb463                       ���϶Ȱٷֱȣ������ϱ���
3,opener,target,int,0x7124                           ���϶Ȱٷֱȣ����ƣ�
4,rotationAngle,current,int,0xa096                  ����Ƕȣ������ϱ���
5,rotationAngle,target,int,0x2596                   ����Ƕȣ��������ƣ�  //����5
6,action,action,enum,0x486a                         ����ͣ
7,SBZT,SBZT,enum,0x51da                              �豸״̬ 0���� 1����
8,JointLevel,JointLevel,int,0xa79c                     �㶯�ȼ� 1-10
9,Limit,DeletLinit,enum,0xf61a                          ɾ����λ
10,Limit,UpLimit,enum,0xb549                            ��������λ
11,Limit,DownLimit,enum,0x5253                            ��������λ                   
---------------------------*/

void Dispose_All_Date_For_TX(void)                //�ϴ�����״̬�����ݴ���
{
        Uart_buf_tx[0]=0xa5;
        Uart_buf_tx[1]=0x5a;
        Uart_buf_tx[2]=0x01;
        Uart_buf_tx[3]=0x02;
        Uart_buf_tx[4]=0x00;
        Uart_buf_tx[5]=0x36;           //���ݳ���
        
        Uart_buf_tx[6]=0x51;                      //״̬tagid  0x51da       enum  
        Uart_buf_tx[7]=0xda;
        Uart_buf_tx[8]=0x00;
        Uart_buf_tx[9]=0x02;                          //enum��λ
        Uart_buf_tx[10]=0x00;
        if(Motor.Motor_Status == MOTOR_UP) Uart_buf_tx[11]=0x01;                //����״̬
        else if(Motor.Motor_Status == MOTOR_STOP) Uart_buf_tx[11]=0x00;
        else if(Motor.Motor_Status == MOTOR_DOWN) Uart_buf_tx[11]=0x01;             //LIU
        
        //Uart_buf_tx[12]=0xb4;                                       //�ٷֱ�tagid 0xb463
        Uart_buf_tx[12]=0xb4;
        Uart_buf_tx[13]=0x63;
        Uart_buf_tx[14]=0x00;
        Uart_buf_tx[15]=0x04;                                       //int  4λ
        Uart_buf_tx[16]=0x00;
        Uart_buf_tx[17]=0x00;
        Uart_buf_tx[18]=0x00;
        Uart_buf_tx[19]=JourneyPercentCount();                        //�ٷֱ�
        
        Uart_buf_tx[20]=0x2D;                                            //����tagid 0x2d33
        Uart_buf_tx[21]=0x33;
        Uart_buf_tx[22]=0x00;
        Uart_buf_tx[23]=0x02;                                               //enum��λ
        Uart_buf_tx[24]=0x00;
        Uart_buf_tx[25]=(MotorFlag.Direction+1)*100;                     //���� 
        
        Uart_buf_tx[26]=0xa7;                                            //�㶯�ȼ��ϱ�tagid 0xa97c
        Uart_buf_tx[27]=0x9c;
        Uart_buf_tx[28]=0x00;
        Uart_buf_tx[29]=0x04;                                           //int              4λ
        Uart_buf_tx[30]=0x00;
        Uart_buf_tx[31]=0x00;
        Uart_buf_tx[32]=0x00;
        Uart_buf_tx[33]=Joint_Time_Level;                                       //�㶯�ȼ�
        
        Uart_buf_tx[34]=0xa0;                                            //����Ƕ��ϱ�tagid 0xa096
        Uart_buf_tx[35]=0x96;
        Uart_buf_tx[36]=0x00;
        Uart_buf_tx[37]=0x04;                                         //int  4λ
        Uart_buf_tx[38]=0x00;
        Uart_buf_tx[39]=0x00;
        Uart_buf_tx[40]=0x00;  
                Current_Angle_5 =Current_Angle%5;
        if(Current_Angle_5>2)
        {     
          Uart_buf_tx[41]=Current_Angle-Current_Angle_5+5;                                       //0-180�Ƕ�,���ص�ǰλ��      
        }
        else if(Current_Angle_5<=2)
        {
         Uart_buf_tx[41]=Current_Angle-Current_Angle_5;                                       //0-180�Ƕ�,���ص�ǰλ��      
        }
                
        Uart_buf_tx[42]=0x2C;                                            //����ģʽ�ϱ�tagid 0x2d33   //2CE8
        Uart_buf_tx[43]=0xE8;
        Uart_buf_tx[44]=0x00;
        Uart_buf_tx[45]=0x02;                                                     //enum  2λ
        Uart_buf_tx[46]=0x00;
        Uart_buf_tx[47]=!MotorFlag.Motor_Jiggle_Status;                                       //�㶯����ģʽ
        
        Uart_buf_tx[48]=0xb5;                                            //����λtagid 0xb549
        Uart_buf_tx[49]=0x49;
        Uart_buf_tx[50]=0x00;
        Uart_buf_tx[51]=0x02;                                          //enum  ��λ
        Uart_buf_tx[52]=0x00;
        if((Limit_Point[0].Flag.F1.Limit_Activate)&&(MotorFlag.Journey_Direction != MotorFlag.Direction)||(Limit_Point[1].Flag.F1.Limit_Activate)) //��������λ
        {
          Uart_buf_tx[53]=0x00;                                             //����λ�������
        }
        else {
          Uart_buf_tx[53]=0x01;                                             //����λû����
        }
        
        
        Uart_buf_tx[54]=0x52;                                            //����λ�ϱ�tagid 0x5253
        Uart_buf_tx[55]=0x53;
        Uart_buf_tx[56]=0x00;
        Uart_buf_tx[57]=0x02;                                             //enum    2λ
        Uart_buf_tx[58]=0x00;
        if((Limit_Point[0].Flag.F1.Limit_Activate)&&(MotorFlag.Journey_Direction == MotorFlag.Direction)||(Limit_Point[1].Flag.F1.Limit_Activate)) //��������λ
        {
          Uart_buf_tx[59]=0x00;                                             //����λ�������
        }
        else {
          Uart_buf_tx[59]=0x01;                                             //����λû����
        }
        

//        Uart_buf_tx[60]=0x48;                      //����ͣ״̬tagid  0x486a       enum  
//        Uart_buf_tx[61]=0x6a;
//        Uart_buf_tx[62]=0x00;
//        Uart_buf_tx[63]=0x02;                          //enum��λ
//        Uart_buf_tx[64]=0x00;
//        if(Motor.Motor_Status == MOTOR_UP) Uart_buf_tx[65]=0x00;                //����״̬
//        else if(Motor.Motor_Status == MOTOR_STOP) Uart_buf_tx[65]=0x02;
//        else if(Motor.Motor_Status == MOTOR_DOWN) Uart_buf_tx[65]=0x01;
       
        Uart_buf_tx[60]=sum(Uart_buf_tx,60);
        
        tx_len=61; 
Motor_Data_Bak.Motor_Status_1 = 0XFF;//�ϱ�һ������״̬LIU
Motor_Data_Bak.Motor_Mode_Status = 1;      //�����ϱ�����״̬
}

/*********************************************************************************
������:   Send_Data
��  ��:   
*********************************************************************************/
void  Send_Data(u8 TX_Times,u8 TxKind,u8 Tx_end,u8 Tx_first)          //������Ӧʱ�䣬�����������ͣ�����ͱ�־(0X05ר�����ֻ��һ��ֱ��д1)����һ��Ҫ��������(0X05ר��)
{
  flag_tx = 1;
  tx_c = TX_Times;
  tx_data = TxKind;
  tx_data_i = Tx_first;
  tx_data_n = Tx_end;
  respons_tc = 0;
}

void UartStartupTX(void)
{
  u8 data;
  if(flag_tx && !tx_ok_tc && !respons_tc)
  {
    switch(tx_data)
    {
    case 0x04://����WIFI
      {
        u8 Func_data[] = {0x04,0x00,0x00};
        memcpy(&Uart_buf_tx[3],&Func_data,sizeof(Func_data));
      }break;
      
    case 0x05://����WIFIѡ��ģʽ
      {
        u8 Func_data[] = {0x05,0x00,0x01,0x00};
        memcpy(&Uart_buf_tx[3],&Func_data,sizeof(Func_data));
      }break;
      
    case 0x06://DP�������ϱ�
      {
        switch(tx_data_i)
        {
        case 0x02://����
          {
            if(Motor.Motor_Status == MOTOR_UP)data = 0;//��
            else if(Motor.Motor_Status == MOTOR_DOWN)data = 2;//��
            else if(Motor.Motor_Status == MOTOR_STOP)data = 1;//ֹͣ
            u8 Func_data[] = {0x07,0x00,0x05,0x01,0x04,0x00,0x01,data};
            memcpy(&Uart_buf_tx[3],&Func_data,sizeof(Func_data));
          }break;
          
        case 0x03://�����ٷֱȿ���
          {
            data = JourneyPercentCount();
            u8 Func_data[] = {0x07,0x00,0x08,0x02,0x02,0x00,0x04,0x00,0x00,0x00,data};
            memcpy(&Uart_buf_tx[3],&Func_data,sizeof(Func_data));
          }break;
          
        case 0x04://�����ٷֱ�״̬
          {
            data = JourneyPercentCount();
            u8 Func_data[] = {0x07,0x00,0x08,0x03,0x02,0x00,0x04,0x00,0x00,0x00,data};
            memcpy(&Uart_buf_tx[3],&Func_data,sizeof(Func_data));
          }break;
          
        case 0x05://�������
          {
            u8 Func_data[] = {0x07,0x00,0x05,0x05,0x04,0x00,0x01,MotorFlag.Direction};
            memcpy(&Uart_buf_tx[3],&Func_data,sizeof(Func_data));
          }break;
          
        case 0x01://����״̬
          {
            if(Motor.Motor_Status == MOTOR_UP)data = 0;//��
            else if(Motor.Motor_Status == MOTOR_DOWN)data = 1;//��
            u8 Func_data[] = {0x07,0x00,0x05,0x07,0x04,0x00,0x01,data};
            memcpy(&Uart_buf_tx[3],&Func_data,sizeof(Func_data));
          }break;
                    
        case 0x06://���λ��
          {
            u8 Func_data[] = {0x07,0x00,0x08,0x13,0x02,0x00,0x04,0x00,0x00,0x00,Sweet_Spot};
            memcpy(&Uart_buf_tx[3],&Func_data,sizeof(Func_data));
          }break;
                    
        case 0x07://����ģʽ
          {
            if(MotorFlag.Motor_Jiggle_Status)data = 0;
            else if(MotorFlag.Motor_Jiggle_Status == 0)data = 1;
            u8 Func_data[] = {0x07,0x00,0x05,0x65,0x04,0x00,0x01,data};
            memcpy(&Uart_buf_tx[3],&Func_data,sizeof(Func_data));
          }break;
          
        case 0x08://���õ㶯�ȼ�
          {
            u8 Func_data[] = {0x07,0x00,0x08,0x6d,0x02,0x00,0x04,0x00,0x00,0x00,Joint_Time_Level};
            memcpy(&Uart_buf_tx[3],&Func_data,sizeof(Func_data));
          }break;
          
        default:flag_tx=0;
        }
      }break;
      
    case 0x0e://����
      {
        u8 Func_data[] = {0x0e,0x00,0x00};
        memcpy(&Uart_buf_tx[3],&Func_data,sizeof(Func_data));
      }break;

    default:flag_tx=0;
    }
    
    if(flag_tx)
    {
      Start_TX();//���ڷ���ʹ��
      tx_ok_tc = 50;//�����ٴν���
//      Time_50ms_Cnt = TIME_50MS_VALUE;
//      Flag.Time_50ms = NULL;
    }
  }
}

void UartDataDisposal(void)//485���ܴ�����
{
  static u8 i=0;
  u8 flag_answer=1;
  if(flag_rx)
  {
    if(sum(Uart_buf_rx,6+Uart_buf_rx[5])==Uart_buf_rx[6+Uart_buf_rx[5]])
    {
      switch(Uart_buf_rx[3])
      {
      case 0x00://����
        {
          Uart_buf_tx[3]=0x00;
          Uart_buf_tx[4]=0x00;
          Uart_buf_tx[5]=0x01;
          if(!flag_power_on)
          { 
            Uart_buf_tx[6]=0x00;
            flag_power_on=1;
          }
          else Uart_buf_tx[6]=0x01;
          Uart_buf_tx[7]=sum(Uart_buf_tx,7);
        }break;
        
      case 0x01://PID
        {
          Uart_buf_tx[3]=0x01;
          Uart_buf_tx[4]=0x00;
          Uart_buf_tx[5]=0x2A;
          for(i=0;i<42;i++) //0��20Ϊ21
          {
            Uart_buf_tx[6+i]=TYPE_ID[i];
          }
          Uart_buf_tx[48]=sum(Uart_buf_tx,48);//27
        }break;
        
      case 0x02://��ѯMCU�趨ģ�鹤����ʽ
        {
          u8 Func_data[] = {0x02,0x00,0x00};
          memcpy(&Uart_buf_tx[3],&Func_data,sizeof(Func_data));
        }break;
        
      case 0x03://����WIFI����״̬
        {
          if(Uart_buf_rx[6]==0x04) //�����ɹ�
          {
            if(Flag.net_add) 
            {
              Flag.net_add  = 0;
              Flag.Opretion_Finish = TRUE;//������������
              Opretion_Event = NULL;
            }
            
          }
          u8 Func_data[] = {0x03,0x00,0x00};
          memcpy(&Uart_buf_tx[3],&Func_data,sizeof(Func_data));
        }break;
        
      case 0x04://����WIFI   
        {
          flag_answer=0;
          if(tx_data==0x04)
          {
            flag_tx=0;
          }
        }break;
        
      case 0x05://����WIFIѡ��ģʽ
        {
          flag_answer=0;
          if(tx_data==0x05)
          { 
            flag_tx=0;
          }
        }break;
    
      case 0x06://����Э��
        {
          tx_data=0x07;
          if(Uart_buf_rx[6]==0x01 && Uart_buf_rx[7]==0x04)//���״̬
          {flag_answer=0;
            UartControl_WIFI(Uart_buf_rx[10]);
//            u8 Func_data[] = {0x07,0x00,0x05,0x01,0x04,0x00,0x01,Uart_buf_rx[10]};
//            memcpy(&Uart_buf_tx[3],&Func_data,sizeof(Func_data));
          }
          else if(Uart_buf_rx[6]==0x02 && Uart_buf_rx[7]==0x02)//�ٷֱȿ���
          {
            /*if(Uart_buf_rx[13]<=100 && Limit_Point[1].Flag.F1.Limit_Activate)
            {
                Flag.Goto_PCT = TRUE;
                Flag.Angle_Stop = NULL ;//�ٷֱȿ��ƹص���LIU 
                Flag.Motor_Status_Switch = TRUE;//����Ŀ��λ��
                Hall_Pulse_Amount_PCT = CountPercentPlace(100-Uart_buf_rx[13]);
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
                if ((Hall_Pulse_Amount_Cnt > (Hall_Pulse_Amount_PCT - 5))
                    && (Hall_Pulse_Amount_Cnt < (Hall_Pulse_Amount_PCT + 5)))
                {
                  MotorFlag.Motor_Run = FALSE;
                  Motor.Motor_Status = MOTOR_STOP;
                  Flag.Goto_PCT = FALSE;
                }
                //Motor_Status_bak=Motor.Motor_Status;
            }*/
            
            if (Limit_Point_Amount >= 2)       /*�Ƿ�������λ�� */
            {
              Flag.Goto_PCT = TRUE;
              Flag.Motor_Status_Switch = TRUE;     //����Ŀ��λ��
              
              Hall_Pulse_Amount_PCT = CountPercentPlace (Uart_buf_rx[13]);
              
              Motor.Motor_Status = MOTOR_UP;
              if (!(MotorFlag.Journey_Direction ^ MotorFlag.Direction))
              {
                if (Hall_Pulse_Amount_Cnt > Hall_Pulse_Amount_PCT)
                {
                  Motor.Motor_Status = MOTOR_DOWN;
                }
              }
              else
              {
                if (Hall_Pulse_Amount_Cnt < Hall_Pulse_Amount_PCT)
                {
                  Motor.Motor_Status = MOTOR_DOWN;
                }
              }
              
              if ((Hall_Pulse_Amount_Cnt > (Hall_Pulse_Amount_PCT - 10))
                  && (Hall_Pulse_Amount_Cnt < (Hall_Pulse_Amount_PCT + 10)))
              {
                  MotorFlag.Motor_Run = FALSE;
                  Motor.Motor_Status = MOTOR_STOP;
                  Flag.Goto_PCT = FALSE;
              }
            }
            u8 Func_data[] = {0x07,0x00,0x08,0x02,0x02,0x00,0x04,0x00,0x00,0x00,Uart_buf_rx[13]};
            memcpy(&Uart_buf_tx[3],&Func_data,sizeof(Func_data));
          }
          else if(Uart_buf_rx[6]==0x05 && Uart_buf_rx[7]==0x04)//�����������
          {
            flag_answer=0;
            Flag.Opretion_Finish = TRUE;//������ʾ
            if(Uart_buf_rx[10] == 0X00)//0����
            {
              if(MotorFlag.Direction)
              {
                MotorFlag.Direction = 0;
              }
            }
            else if (Uart_buf_rx[10] == 0X01)//����
            {
              if(!MotorFlag.Direction)
              {
                MotorFlag.Direction = 1;
              }
            }
//            u8 Func_data[] = {0x07,0x00,0x05,0x05,0x04,0x00,0x01,MotorFlag.Direction};
//            memcpy(&Uart_buf_tx[3],&Func_data,sizeof(Func_data));
          }
          else if(Uart_buf_rx[6]==0x10 && Uart_buf_rx[7]==0x04)//������λ
          {
            if(Uart_buf_rx[10] == 0X00)//��������λ��
            {
              Motor.Motor_Status = MOTOR_STOP;//���ͣ
              MotorFlag.Motor_Run = NULL;
              if (((!Limit_Point[1].Flag.F1.Limit_Activate)&&(Restrict_Up!=1))||(!Limit_Point[0].Flag.F1.Limit_Activate))      //û������λ���ǵ�һ����λ�㲻����
              {
                if(!Limit_Point[0].Flag.F1.Limit_Activate)
                {
                  MotorFlag.Journey_Direction = !MotorFlag.Direction;
                  Restrict_Up = 1;                                              //����������λ
                }  
                Flag.Set_Journey_Direction = FALSE;
                Flag.Save_Limit_Point = TRUE;      //������λ
                Flag.Opretion_Finish = TRUE;       //LIU����
                Flag.Time_50ms = NULL;
                Time_50ms_Cnt = TIME_50MS_VALUE;
              }  
            }
            else if(Uart_buf_rx[10] == 0X01)//��������λ��
            {
              Motor.Motor_Status = MOTOR_STOP;//���ͣ
              MotorFlag.Motor_Run = NULL;
              if (((!Limit_Point[1].Flag.F1.Limit_Activate)&&(Restrict_Up!=2))||(!Limit_Point[0].Flag.F1.Limit_Activate))                //û������λ���ǵ�һ����λ�㲻����
              {
                if(!Limit_Point[0].Flag.F1.Limit_Activate)
                {
                  MotorFlag.Journey_Direction = MotorFlag.Direction;
                  Restrict_Up = 2;                                            //����������λ
                }
                Flag.Set_Journey_Direction = FALSE;    
                Flag.Save_Limit_Point = TRUE;      //������λ
                Flag.Opretion_Finish = TRUE;       //LIU����
                Flag.Time_50ms = NULL;
                Time_50ms_Cnt = TIME_50MS_VALUE;
              }
            }
            else if(Uart_buf_rx[10] == 0X02)//ɾ����λ��
            {
              JourneyDelete();
              Flag.Opretion_Finish = TRUE;       //LIU����
            }
            u8 Func_data[] = {0x07,0x00,0x05,0x10,0x04,0x00,0x01,Uart_buf_rx[10]};
            memcpy(&Uart_buf_tx[3],&Func_data,sizeof(Func_data));
          }
          else if(Uart_buf_rx[6]==0x13 && Uart_buf_rx[7]==0x02)//���λ��
          {
            Sweet_Spot = Uart_buf_rx[13];
            Flag.Opretion_Finish = TRUE;         //������ʾ
            u8 Func_data[] = {0x07,0x00,0x08,0x13,0x02,0x00,0x04,0x00,0x00,0x00,Sweet_Spot};
            memcpy(&Uart_buf_tx[3],&Func_data,sizeof(Func_data));
          }
          else if(Uart_buf_rx[6]==0x14 && Uart_buf_rx[7]==0x04)//�㶯����
          {
            Flag.Jiggle_Stop = 0;
            if(Uart_buf_rx[10] == 0)//��
            {
              Flag.Jiggle_Stop = TRUE;
              Motor.Motor_Status = MOTOR_UP;
              Hall_Pulse_Jiggle_Buff = Hall_Pulse_Amount_Cnt;//���浱ǰλ��
              
              Joint_Action_Motor_Buf = Motor.Motor_Status;
              Flag.Joint_Action = NULL;
              Joint_Action_Time_Cnt = 0;
            }
            else if(Uart_buf_rx[10] == 1)//��
            {
              Flag.Jiggle_Stop = TRUE;
              Motor.Motor_Status = MOTOR_DOWN;
              Hall_Pulse_Jiggle_Buff = Hall_Pulse_Amount_Cnt;//���浱ǰλ��
              
              Joint_Action_Motor_Buf = Motor.Motor_Status;
              Flag.Joint_Action = NULL;
              Joint_Action_Time_Cnt = 0;
            }
            u8 Func_data[] = {0x07,0x00,0x05,0x14,0x04,0x00,0x01,Uart_buf_rx[10]};//
            memcpy(&Uart_buf_tx[3],&Func_data,sizeof(Func_data));
          }
          else if(Uart_buf_rx[6]==0x65 && Uart_buf_rx[7]==0x04)//�㶯��������
          {
            MotorFlag.Motor_Jiggle_Status =!Uart_buf_rx[10];               
            Flag.Opretion_Finish = TRUE;         //������ʾ
            u8 Func_data[] = {0x07,0x00,0x05,0x65,0x04,0x00,0x01,!MotorFlag.Motor_Jiggle_Status};
            memcpy(&Uart_buf_tx[3],&Func_data,sizeof(Func_data));
          }
          else if(Uart_buf_rx[6]==0x66 && Uart_buf_rx[7]==0x04)//����ң����
          {
            Flag.Learn_Code_Statu = TRUE;
            Flag.Opretion_Finish = TRUE;
            Opretion_Event = 0;
            Flag.Time_100ms = TRUE;
            Motor.Motor_Status = MOTOR_STOP;
            u8 Func_data[] = {0x07,0x00,0x05,0x66,0x04,0x00,0x01,0x00};
            memcpy(&Uart_buf_tx[3],&Func_data,sizeof(Func_data));
          }
          else if(Uart_buf_rx[6]==0x67 && Uart_buf_rx[7]==0x04)//�ָ�����
          {
            Flag.Delete_Code = TRUE;
            u8 Func_data[] = {0x07,0x00,0x05,0x67,0x04,0x00,0x01,0x00};
            memcpy(&Uart_buf_tx[3],&Func_data,sizeof(Func_data));
          }
          else if(Uart_buf_rx[6]==0x6d && Uart_buf_rx[7]==0x02)//���õ㶯�Ƕȵȼ�
          {
            if (Uart_buf_rx[13]<=20 && Uart_buf_rx[13]>=1)//1-10
            {
              Joint_Time_Level=Uart_buf_rx[13]; 
              Flag.Opretion_Finish = TRUE;//������ʾ
            }
            u8 Func_data[] = {0x07,0x00,0x08,0x6d,0x02,0x00,0x04,0x00,0x00,0x00,Joint_Time_Level};
            memcpy(&Uart_buf_tx[3],&Func_data,sizeof(Func_data));
          }
        }break;

      case 0x08://��ѯMCU����״̬
        {
          flag_answer=0;
          Send_Data(1,0x06,10,2);
        }
        break;
        
       case 0x0e:
        if(Flag.WifiTest)
        {
          if(Uart_buf_rx[4]==0x00&&Uart_buf_rx[5]==0x02)
          {
            if(Uart_buf_rx[6]==0x01) //���ݳ���Ϊ2�ֽڣ� Data[0]:0x00ʧ��, 0x01�ɹ�; 
            {
               Flag.Opretion_Finish = TRUE;
            } 
            else if(Uart_buf_rx[6]==0x00)  
            { 
              Flag.Del_Code_Finish = TRUE;
            }  
          }
          Send_Data(1,0x06,10,2);
          Flag.WifiTest = 0;
        }
        break;
        
      default:flag_answer=0;
      }
      
      if(flag_answer)
      {
        Start_TX();              //���ڷ���ʹ��
      }
    }
    rx_i=0;
    flag_rx=0;
  }
}

////-------------------------------------------------------------------------------------
////  ������:   Uart0_Status_Inspect
////  ��  ��:   ����ͨѶ״̬���
////-------------------------------------------------------------------------------------
//void Uart0_Status_Inspect (void)
//{
//  if(Uart0_NO_Rx_Cnt)
//  {
//    Uart0_NO_Rx_Cnt--;
//    if(Uart0_NO_Rx_Cnt==0)
//    {
//      Uart0_Byte_Cnt = 0;
//    }
//  }
//}

//void Motor_Data_Change_Send(void)
//{
//  if(Flag.Send_JourneyPercentCount)                         //����ٷֱ�  //����֮�����̻�
//  {
//    
//    {
//      Flag.Send_JourneyPercentCount = 0;
//      Send_Data(3,0x63,0,0);
//      Motor_Data_Bak.Journey_Percent_Count = JourneyPercentCount ();
//    }
//    
//  }
//  
//  if(Flag.Send_JourneyPercentCount_1)                         //����ٷֱ�  Ŀ��ֵ
//  {
//    
//    {
//      Flag.Send_JourneyPercentCount_1 = 0;
//      Send_Data(3,0x24,0,0);
//      // Motor_Data_Bak.Journey_Percent_Count = JourneyPercentCount ();
//    }
//    
//  }
//  else if (Flag.Send_Current_Angle)                         //��ת�Ƕ�   //����֮�����̻�
//  {
//    
//    {
//      Flag.Send_Current_Angle = 0;
//      Send_Data(3,0x96,0,0);
//      Motor_Data_Bak.Flip_Angle = Current_Angle;
//    }
//  }
//  else if (Motor_Data_Bak.Motor_Status != Motor.Motor_Status)                         //�������ͣ����״̬
//  {
//    Send_Data(3,0xDA,0,0);
//    Motor_Data_Bak.Motor_Status = Motor.Motor_Status;
//  }
//  else if (Motor_Data_Bak.Motor_Status_1 != Motor.Motor_Status)                         //�������ͣ״̬
//  {
//    Send_Data(3,0x6A,0,0);
//    Motor_Data_Bak.Motor_Status_1 = Motor.Motor_Status;
//  }
//  else if(Motor_Data_Bak.Motor_Direction != MotorFlag.Direction)                         //�������
//  {
//    Send_Data(3,0x33,0,0);
//    Motor_Data_Bak.Motor_Direction = MotorFlag.Direction;
//    
//  } 
//  
//  else if(Motor_Data_Bak.Motor_Mode_Status)                         //�������ģʽ
//  {
//    Motor_Data_Bak.Motor_Mode_Status = 0;
//    Send_Data(3,0x1e,0,0);
//    //Motor_Data_Bak.Motor_Direction = MotorFlag.Direction;
//    
//  }
//  
//  else if(Motor_Data_Bak.Journey_Percent_Count != JourneyPercentCount ())                         //����ٷֱ�
//  {
//    if(Motor.Motor_Status == MOTOR_STOP)                             //ֹͣ�ŷ���
//    {
//      Send_Data(3,0x63,0,0);
//      Motor_Data_Bak.Journey_Percent_Count = JourneyPercentCount ();
//    }
//    
//  }
//  else if (Motor_Data_Bak.Flip_Angle != Current_Angle)                         //��ת�Ƕ�
//  {
//    if(Motor.Motor_Status == MOTOR_STOP)                             //ֹͣ�ŷ���
//    {
//      flag_tx=1;
//      //tx_data=0x96;
//      //tx_c=3;
//      Motor_Data_Bak.Flip_Angle = Current_Angle;
//    }
//  }
//  
//  
//  else if (Motor_Data_Bak.Joint_Level != Joint_Time_Level)                         //�㶯�ȼ�
//  {
//    
//    Send_Data(3,0x9c,0,0);
//    Motor_Data_Bak.Joint_Level = Joint_Time_Level;
//    
//  }
//  else if (Motor_Data_Bak.Joint_Status != MotorFlag.Motor_Jiggle_Status)                         //�㶯����״̬
//  {
//    Send_Data(3,0xe8,0,0);
//    Motor_Data_Bak.Joint_Status = MotorFlag.Motor_Jiggle_Status;
//    
//  }
//  else                                                                                         //ɨ����λ��
//  {
//    if(MotorFlag.Journey_Direction != MotorFlag.Direction)                                      //  ���г�
//    {
//      if(Motor_Data_Bak.Up_Limit_Status != Limit_Point[0].Flag.F1.Limit_Activate)                     //���г̼������Ϊ����λ
//      {
//        Send_Data(3,0x49,0,0);
//        Motor_Data_Bak.Up_Limit_Status = Limit_Point[0].Flag.F1.Limit_Activate;
//      }
//      else if (Motor_Data_Bak.Down_Limit_Status != Limit_Point[1].Flag.F1.Limit_Activate)                     //���г̼���һ��Ϊ����λ
//      {
//        Send_Data(3,0x53,0,0);
//        Motor_Data_Bak.Down_Limit_Status = Limit_Point[1].Flag.F1.Limit_Activate;
//        
//      }
//    }
//    else if(MotorFlag.Journey_Direction == MotorFlag.Direction)                                    //���г�              
//    {
//      
//      if(Motor_Data_Bak.Up_Limit_Status != Limit_Point[1].Flag.F1.Limit_Activate)                     //���г̼���һ��Ϊ����λ
//      {
//        Send_Data(3,0x49,0,0);
//        Motor_Data_Bak.Up_Limit_Status = Limit_Point[1].Flag.F1.Limit_Activate;
//      }
//      else if (Motor_Data_Bak.Down_Limit_Status != Limit_Point[0].Flag.F1.Limit_Activate)                     //���г̼������Ϊ����λ
//      {
//        Send_Data(3,0x53,0,0);
//        Motor_Data_Bak.Down_Limit_Status = Limit_Point[0].Flag.F1.Limit_Activate;
//        
//      }
//    }
//  }
//}
