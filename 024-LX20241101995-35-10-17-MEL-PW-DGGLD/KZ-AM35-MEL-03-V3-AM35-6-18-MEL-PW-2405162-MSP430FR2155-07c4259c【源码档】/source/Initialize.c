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
#include "Define.h"                                          /* ȫ�ֺ궨�� ------*/
#include "Extern.h"                                          /* ȫ�ֱ��������� --*/
/**********************************************************************************
  ������:   Delay
  ��  ��:    
*********************************************************************************/
void Delay (register unsigned int i)
{
  for (; i > 0; i--)
  {
    _NOP();  
  }
}
/*********************************************************************************
  ������:   CLKSetting
  ��  ��:    
*********************************************************************************/
void CLKSetting (void)
{
  #if (SYSTEM_CLOCK == 1)
    __bis_SR_register(SCG0);                    // disable FLL
    CSCTL3 |= SELREF__REFOCLK;                  // Set REFO as FLL reference source
    CSCTL0 = 0;                                 // clear DCO and MOD registers
    CSCTL1 &= ~(DCORSEL_7);                     // Clear DCO frequency select bits first
    CSCTL1 |= DCORSEL_0;                        // Set DCO = 1MHz
    CSCTL2 = FLLD_0 + 29;                       // DCODIV = 1MHz  29 + 1 * 32768
    __delay_cycles(3);
    __bic_SR_register(SCG0);                    // enable FLL
    while(CSCTL7 & (FLLUNLOCK0 | FLLUNLOCK1));  // Poll until FLL is locked
    
    CSCTL4 = SELMS__DCOCLKDIV | SELA__REFOCLK;  // set default REFO(~32768Hz) as ACLK source, ACLK = 32768Hz
    CSCTL5 |= DIVM_0 | DIVS_0;                  // MCLK = DCOCLK = 8MHZ,
                                                // SMCLK = MCLK = 8MHz
  #elif (SYSTEM_CLOCK == 2)
    __bis_SR_register(SCG0);                    // disable FLL
    CSCTL3 |= SELREF__REFOCLK;                  // Set REFO as FLL reference source
    CSCTL0 = 0;                                 // clear DCO and MOD registers
    CSCTL1 &= ~(DCORSEL_7);                     // Clear DCO frequency select bits first
    CSCTL1 |= DCORSEL_1;                        // Set DCO = 2MHz
    CSCTL2 = FLLD_0 + 60;                       // DCODIV = 2MHz  60 + 1 * 32768
    __delay_cycles(3);
    __bic_SR_register(SCG0);                    // enable FLL
    while(CSCTL7 & (FLLUNLOCK0 | FLLUNLOCK1));  // Poll until FLL is locked
    
    CSCTL4 = SELMS__DCOCLKDIV | SELA__REFOCLK;  // set default REFO(~32768Hz) as ACLK source, ACLK = 32768Hz
    CSCTL5 |= DIVM_0 | DIVS_0;                  // MCLK = DCOCLK = 2MHZ,
                                                // SMCLK = MCLK = 2MHz
  #elif (SYSTEM_CLOCK == 4)
    __bis_SR_register(SCG0);                    // disable FLL
    CSCTL3 |= SELREF__REFOCLK;                  // Set REFO as FLL reference source
    CSCTL0 = 0;                                 // clear DCO and MOD registers
    CSCTL1 &= ~(DCORSEL_7);                     // Clear DCO frequency select bits first
    CSCTL1 |= DCORSEL_2;                        // Set DCO = 4MHz
    CSCTL2 = FLLD_0 + 121;                      // DCODIV = 4MHz  121 + 1 * 32768
    __delay_cycles(3);
    __bic_SR_register(SCG0);                    // enable FLL
    while(CSCTL7 & (FLLUNLOCK0 | FLLUNLOCK1));  // Poll until FLL is locked
    
    CSCTL4 = SELMS__DCOCLKDIV | SELA__REFOCLK;  // set default REFO(~32768Hz) as ACLK source, ACLK = 32768Hz
    CSCTL5 |= DIVM_0 | DIVS_0;                  // MCLK = DCOCLK = 4MHZ,
                                                // SMCLK = MCLK = 4MHz
  #elif (SYSTEM_CLOCK == 8)
    __bis_SR_register(SCG0);                    // disable FLL
    CSCTL3 |= SELREF__REFOCLK;                  // Set REFO as FLL reference source
    CSCTL0 = 0;                                 // clear DCO and MOD registers
    CSCTL1 &= ~(DCORSEL_7);                     // Clear DCO frequency select bits first
    CSCTL1 |= DCORSEL_3;                        // Set DCO = 8MHz
    CSCTL2 = FLLD_0 + 243;                      // DCODIV = 8MHz  243 + 1 * 32768
    __delay_cycles(3);
    __bic_SR_register(SCG0);                    // enable FLL
    while(CSCTL7 & (FLLUNLOCK0 | FLLUNLOCK1));  // Poll until FLL is locked
    
    CSCTL4 = SELMS__DCOCLKDIV | SELA__REFOCLK;  // set default REFO(~32768Hz) as ACLK source, ACLK = 32768Hz
    CSCTL5 |= DIVM_0 | DIVS_0;                  // MCLK = DCOCLK = 8MHZ,
                                                // SMCLK = MCLK = 8MHz
  #elif (SYSTEM_CLOCK == 12)
    __bis_SR_register(SCG0);                    // disable FLL
    CSCTL3 |= SELREF__REFOCLK;                  // Set REFO as FLL reference source
    CSCTL0 = 0;                                 // clear DCO and MOD registers
    CSCTL1 &= ~(DCORSEL_7);                     // Clear DCO frequency select bits first
    CSCTL1 |= DCORSEL_4;                        // Set DCO = 12MHz
    CSCTL2 = FLLD_0 + 365;                      // DCODIV = 12MHz  365 + 1 * 32768
    __delay_cycles(3);
    __bic_SR_register(SCG0);                    // enable FLL
    while(CSCTL7 & (FLLUNLOCK0 | FLLUNLOCK1));  // Poll until FLL is locked
    
    CSCTL4 = SELMS__DCOCLKDIV | SELA__REFOCLK;  // set default REFO(~32768Hz) as ACLK source, ACLK = 32768Hz
    CSCTL5 |= DIVM_0 | DIVS_0;                  // MCLK = DCOCLK = 12MHZ,
                                                // SMCLK = MCLK = 12MHz
  #elif (SYSTEM_CLOCK == 16)
    __bis_SR_register(SCG0);                    // disable FLL
    CSCTL3 |= SELREF__REFOCLK;                  // Set REFO as FLL reference source
    CSCTL0 = 0;                                 // clear DCO and MOD registers
    CSCTL1 &= ~(DCORSEL_7);                     // Clear DCO frequency select bits first
    CSCTL1 |= DCORSEL_5;                        // Set DCO = 16MHz
    CSCTL2 = FLLD_0 + 487;                      // DCODIV = 16MHz  487 + 1 * 32768
    __delay_cycles(3);
    __bic_SR_register(SCG0);                    // enable FLL
    while(CSCTL7 & (FLLUNLOCK0 | FLLUNLOCK1));  // Poll until FLL is locked
    
    CSCTL4 = SELMS__DCOCLKDIV | SELA__REFOCLK;  // set default REFO(~32768Hz) as ACLK source, ACLK = 32768Hz
    CSCTL5 |= DIVM_0 | DIVS_0;                  // MCLK = DCOCLK = 16MHZ,
                                                // SMCLK = MCLK = 16MHz
  #else
    #error "Not define system clock!"
  #endif
}
//--------------------------------------------------------------------------------------
//  ������:   TIMXSetting
//  ��  ��:    
//--------------------------------------------------------------------------------------
void TIMXSetting (void)
{
  TB3CTL = TBCLR;//��ʱ������
  // Time3_B0 ����
  TB3CCTL0 = CCIE;//CCR0 interrupt enabled
  TB3CCR0 += RF_DECODE_TIME * SYSTEM_CLOCK;//40 uS
  
  //Time3_B1 ����
  TB3CCTL1 = CCIE;//CCR1 interrupt enabled
  TB3CCR1 += SYSTEM_TIME_VALUE * SYSTEM_CLOCK;//2mS
  TB3CTL = TBSSEL_2 + MC_2;// SMCLK, Continous up
  // Time0 ����
  TB2CCTL0 = 0;
  
  TB2CCR0 = MAX_DUTY;//CCR1 PWM duty cycle 2:100
  
  //TB2CCTL1 = OUTMOD_6;//CCR1 toggle/reset
  //TB2CCTL2 = OUTMOD_6;//CCR2 toggle/reset
  
  TB2CTL = TBSSEL_2 + ID_0 + MC_3;//SMCLK, ����Ƶ up down mode
}
/*********************************************************************************
  ������:   GPIOSetting
  ��  ��:    
*********************************************************************************/
void GPIOSetting (void)
{
  P1OUT = 0x00;
  P2OUT = 0x00;
  P3OUT|= 0xD0;//1101 0000
  P4OUT = 0x00;
  P5OUT = 0x00;
  P6OUT = 0x00;

  
  P1DIR = 0x40;//0100 0000              // 0:in 1:out
  P2DIR = 0x11;//0001 0001              // 0:in 1:out
  P3DIR = 0x28;//0010 1000              // 0:in 1:out
  P4DIR = 0x06;//0000 0110              // 0:in 1:out
  P5DIR = 0x03;//0000 0011              // 0:in 1:out
  P6DIR = 0x00;//0000 0000              // 0:in 1:out
  
  P1SEL0 = 0x00;                      // No 
  P2SEL0 = 0x00;                      // No 
  P3SEL0 = 0x00;                      // No 
  P4SEL0 = 0x00;                      // No 
  P5SEL0 = 0x00;                      // No 
  P6SEL0 = 0x00;                      // No 
  
  P1SEL1 = 0x00;                      // No 
  P2SEL1 = 0x00;                      // No 
  P3SEL1 = 0x00;                      // No 
  P4SEL1 = 0x00;                      // No 
  P5SEL1 = 0x00;                      // No 
  P6SEL1 = 0x00;                      // No 
    (P3OUT &= ~(1 << BRAKE));
  //-------------------------------------------
  P3REN |= 0xD0;//1101 0000
  //-------------------------------------------
  
  PM5CTL0 &= ~LOCKLPM5;//Disable the GPIO power-on default high-impedance mode
                       //to activate 1previously configured port settings
  
  SYSCFG0 = FRWPPW | PFWP;//FRAM DATAʹ�ܣ�PROGRAM ����

}
/*********************************************************************************
  ������:   VariableInitialize
  ��  ��:    
*********************************************************************************/
void VariableInitialize (void)
{
  u8 i;
  
  for (i = 0; i < (MID_LIMIT_POINT_AMOUNT + 2); i++)
  {
    Limit_Point[i].Limit_Place = 0;
    Limit_Point[i].Flag.F8 = 0;
  }
 
  Hall_Pulse_Amount_Cnt  = MIN_JOURNEY_VALUE;
  Hall_Pulse_Width       = 0;
  Hall_Status_Buffer     = 0;
  hall_change_cnt        = 0;
  Hall_Time_Cnt          = 0;

  Power_Down_Cnt         = 0;
  Limit_Point_Amount     = 0;
  Joint_Time_Level       = 4;
    
  MotorFlag.Motor_Direction           = 0;      //��������־
  MotorFlag.Motor_Fact_Direction      = 0;      //���ʵ�ʷ����־
  MotorFlag.Motor_Run                 = 0;      //������б�־
  MotorFlag.Hall_Pulse_edge           = 0;      //����������ر�־
  MotorFlag.Direction                 = 0;      //�����־
  MotorFlag.Single_Journey            = 0;      //�����г̱�־
  MotorFlag.Journey_Set_OK            = 0;      //�г��趨OK��־ 
  MotorFlag.Journey_Direction         = 0;      //�г̷����־
  MotorFlag.EE_Save                   = 0;      //�ϵ��־
  MotorFlag.Motor_Jiggle_Status       = 0;      //����㶯���б�־
  Sweet_Spot = 0;
  Uart_Baud_Rate = BR9600;

  RF_ID_Amount=0;
  RF_ID_Save_Place = 0;
  
  Restrict_Up = 0;                      //��һ���ϵ����
  Flag.Up_Meet_Plugb = NULL;                                       /*���������־20250217*/
  Initialize_FRAM_FLAG = 0X21365458;
}
/*********************************************************************************
  ������:   Initialize
  ��  ��:    
  ��  ��:   ��
  ��  ��:   ��    
  ��  ��:		��
  ��  ����
*********************************************************************************/
void Initialize (void)
{
  _DINT();// �����ж� 
   
  // Stop watchdog timer to prevent time out reset
  WDTCTL = WDTPW + WDTHOLD;
  
  Delay(10000);//�ϵ��ӳ�
  
  GPIOSetting();//IO ������
  ADCSetting();
  CLKSetting();//ʱ������ 
  //--------------------------------------------------------------
  LinUartInit();
  LinUart0Init();
  //--------------------------------------------------------------
  TIMXSetting();//��ʱ������
  
  if (Initialize_FRAM_FLAG != 0X21365458)
  {
    Delay(60000);//�ϵ��ӳ�
    
    ADCSetting();//ADC ����
    VariableInitialize();
    Limit_Point_Amount = LimitCollator(Limit_Point);
  }
   Flag.Power_on = 1;
  Time_Power_On = 0;
  learn_key_cnt = 0;
  Flag.Electrify_Reset = FALSE;                //�ϵ縴λLIU
   Motor.Motor_Status = MOTOR_STOP;
    Motor.Motor_Status_Buffer = MOTOR_STOP;
      u8 Init_Fine_Adjust_i;
      for (Init_Fine_Adjust_i = 0; Init_Fine_Adjust_i < (MID_LIMIT_POINT_AMOUNT + 2); Init_Fine_Adjust_i++)          //���΢����λ�ϵ�֮��������
      {

        Limit_Point[Init_Fine_Adjust_i].Flag.F1.Fine_Adjust = NULL;
      }
      Flag.rest_text_receive = 0;                     //�ϵ��һ�β���
      Flag.statTest = TRUE; 
      BRAKE_ON() ;
MotorFlag.Motor_Run =0;
  _EINT();//�����ж� 
  
}

