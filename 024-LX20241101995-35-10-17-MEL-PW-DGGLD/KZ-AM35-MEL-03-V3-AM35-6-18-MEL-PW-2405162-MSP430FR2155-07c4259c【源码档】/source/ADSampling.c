/**********************************************************************************
 *  ��Ŀ���:    
 *  ��Ŀ����:    
 *  �ͻ�����:                                    
 *  �����д:    
 *  ��������:    
 *  �������:    
 *  �汾����:    V1.0
 *  
 *  Samping.c file
 *  ��������: AD������ٶȲ���
**********************************************************************************/
#include "Define.h"                                          /* ȫ�ֺ궨�� ------*/
#include "Extern.h"                                          /* ȫ�ֱ��������� --*/
/* �ڲ��� -----------------------------------------------------------------------*/
#define VOLTAGE_RECKON(ad_val)   (((ad_val) / 28 ) + ((ad_val)/ 55))//((((ad_val)*60)) / 28)//31
#define CURRENT_RECKON(ad_val)   ((u16) ((ad_val) * 6))//�����Ŵ�4��

/* �ڲ����� ---------------------------------------------------------------------*/
//static u8 ADC_VOL_Cnt;
//static u16 ADC_VOL_Buffer;

static u8 ADC_CUR_Cnt;
static u16 ADC_CUR_Buffer;
static u16 ADC_CUR_Array[4];
static u8 ADC_CUR_Array_Index;
/* �ڲ����� ---------------------------------------------------------------------*/
/**********************************************************************************
  ������:   ADCSetting
  ��  ��:   ADC ����
**********************************************************************************/
void ADCSetting (void)
{ 
  //Configure ADC A3 ͨ��P1.3
  P1SEL0 |=  BIT3;
  P1SEL1 |=  BIT3;
  
 
  ADCCTL0 |= ADCSHT_2 | ADCON;//��������ʱ��Ϊ 16 �� ADC ʱ������,�� ADC ��Դ
  ADCCTL1 |= ADCSHP;//ADC ʱ��ԴΪ MODOSC,ѡ�������ʱ��ģʽ
  //ADCCTL2 |= ADCRES;//10-bit conversion results
  ADCCTL2 &= ~ADCRES;//clear ADCRES in ADCCTL
  ADCCTL2 |= ADCRES_1;//10-bit conversion results
  
  ADCMCTL0 |= ADCINCH_3 | ADCSREF_1;//A3 ADC input select; Vref=2.5V
  //Configure reference
  PMMCTL0_H = PMMPW_H;//Unlock the PMM registers��Դ����ģ�飨PMM���ļĴ���

  PMMCTL2 |= INTREFEN | REFVSEL_2;//Enable internal 2.5V reference
  
  //ADCIE |= ADCIE0;//Enable ADC conv complete interrupt
  
  ADCCTL0 |= ADCENC | ADCSC;//Sampling and conversion start
}
/**********************************************************************************
  ������:   SampleAverage
  ��  ��:   ADֵ��ƽ��
  ��  ��:   ���飬����Ĵ�С
  ��  ��:   ��
  ��  ��:   ƽ��ֵ
  ��  ����  ȥ������Сֵ����ֵ��ƽ��
 *********************************************************************************/
u16 SampleAverage (u16 array[],u8 num)
{
    u8 max;//Ӧ��Ϊu16
    u8 min;//Ӧ��Ϊu16
    u8 i;
    u8 j;
    u16 value;
    
    j = 0;
    max = 0;
    min = 0;
    value = 0;
    
    for (i = num - 1; i > 0; i--)  
    {
        j++;
        if (array[j] > array[max])   
        {
            max = j;
        }
        if (array[j] < array[min])   
        {
            min = j;
        }
    }
    for  (i = NULL; i < num; i++)   
    {
        if ((i != max) && (i != min))     
        {
            value += array[i];  
        }    
    }    
    
    if (max == min)      
    {
        value = array[max];  
    }
    else
    {
        value = value / (num - 2);
    }    
    return (value);
}
/**********************************************************************************
  ������:   ADSamping
  ��  ��:   AD����
  ��  ��:   ��
  ��  ��:   ��
  ��  ��:   ��
  ��  ����  
 *********************************************************************************/
void ADSamping (void)
{
  u16 buffer;
  
  if (!(ADCCTL1 & ADCBUSY))//��� ADC �Ƿ���У����������ִ�к����Ĳ���������
  {
    buffer = ADCMEM0; //�����ݴ洢����ֵ��ȡ�� buffer ��
    //�ر� ADC ��ʹ�ܺ͵�Դ
    ADCCTL0 &= ~ADCENC;
    ADCCTL0 &= ~ADCON; 
    ADCMCTL0 = ADCINCH_3 | ADCSREF_1;//���� ADC ͨ���Ͳο���ѹ
    //�����������ֵ
    if ((ADCMCTL0 & 0x000f) == ADCINCH_3)//��鵱ǰѡ���ͨ���Ƿ�Ϊͨ�� 3,
    {//
      ADC_CUR_Buffer += buffer; //    
      ADC_CUR_Cnt++;            //���������� ADC_CUR_Cnt �� 1
      if (ADC_CUR_Cnt >> 2)  //�ж� ADC_CUR_Cnt �Ƿ���ڵ��� 4������ 2 λ�൱�ڳ��� 4��            
      {
        ADC_CUR_Cnt = 0;  //��������
        ADC_CUR_Array[ADC_CUR_Array_Index] = ADC_CUR_Buffer >> 2;//���ۼӵĲ���ֵ���� 4 ��洢�� ADC_CUR_Array ������
        ADC_CUR_Buffer = 0;//�ۼ�������
        
        ADC_CUR_Array_Index++;//���������� ADC_CUR_Array_Index �� 1
        //�ж������Ƿ�����
        if (ADC_CUR_Array_Index >> 2) //�ж� ADC_CUR_Array_Index �Ƿ���ڵ��� 4          
        {
          u16 temp;
          temp = SampleAverage(ADC_CUR_Array,ADC_CUR_Array_Index);//���� SampleAverage ���������� 4 ��ƽ��ֵ��ƽ��ֵ����������洢�� temp ��
  
          Current_Current_mA = CURRENT_RECKON(temp);//����ƽ��ֵ���㵱ǰ�ĵ���ֵ
          ADC_CUR_Array_Index = 0;//������������
        }
      }
      //�ر� ADC ʹ�ܺ͵�Դ������������ͨ���Ͳο���ѹ
      ADCCTL0 &= ~ADCENC;
      ADCCTL0 &= ~ADCON; 
      ADCMCTL0 = ADCINCH_3 | ADCSREF_1;
    }
    //���� ADC ����ʼ����
    ADCCTL0 |= ADCSHT_2 | ADCON;//����16 ADCCLK cyclesͬʱ��ADC
    // Sampling and conversion start
    ADCCTL0 |= ADCENC + ADCSC;                     // Sampling and conversion start
  }
}