/**********************************************************************************
 *  项目编号:    
 *  项目名称:    
 *  客户名称:                                    
 *  程序编写:    
 *  启动日期:    
 *  完成日期:    
 *  版本号码:    V1.0
 *  
 *  Samping.c file
 *  功能描述: AD及电机速度采样
**********************************************************************************/
#include "Define.h"                                          /* 全局宏定义 ------*/
#include "Extern.h"                                          /* 全局变量及函数 --*/
/* 内部宏 -----------------------------------------------------------------------*/
#define VOLTAGE_RECKON(ad_val)   (((ad_val) / 28 ) + ((ad_val)/ 55))//((((ad_val)*60)) / 28)//31
#define CURRENT_RECKON(ad_val)   ((u16) ((ad_val) * 6))//电流放大4倍

/* 内部变量 ---------------------------------------------------------------------*/
//static u8 ADC_VOL_Cnt;
//static u16 ADC_VOL_Buffer;

static u8 ADC_CUR_Cnt;
static u16 ADC_CUR_Buffer;
static u16 ADC_CUR_Array[4];
static u8 ADC_CUR_Array_Index;
/* 内部函数 ---------------------------------------------------------------------*/
/**********************************************************************************
  函数名:   ADCSetting
  功  能:   ADC 设置
**********************************************************************************/
void ADCSetting (void)
{ 
  //Configure ADC A3 通道P1.3
  P1SEL0 |=  BIT3;
  P1SEL1 |=  BIT3;
  
 
  ADCCTL0 |= ADCSHT_2 | ADCON;//采样保持时间为 16 个 ADC 时钟周期,打开 ADC 电源
  ADCCTL1 |= ADCSHP;//ADC 时钟源为 MODOSC,选择采样定时器模式
  //ADCCTL2 |= ADCRES;//10-bit conversion results
  ADCCTL2 &= ~ADCRES;//clear ADCRES in ADCCTL
  ADCCTL2 |= ADCRES_1;//10-bit conversion results
  
  ADCMCTL0 |= ADCINCH_3 | ADCSREF_1;//A3 ADC input select; Vref=2.5V
  //Configure reference
  PMMCTL0_H = PMMPW_H;//Unlock the PMM registers电源管理模块（PMM）的寄存器

  PMMCTL2 |= INTREFEN | REFVSEL_2;//Enable internal 2.5V reference
  
  //ADCIE |= ADCIE0;//Enable ADC conv complete interrupt
  
  ADCCTL0 |= ADCENC | ADCSC;//Sampling and conversion start
}
/**********************************************************************************
  函数名:   SampleAverage
  功  能:   AD值求平均
  输  入:   数组，数组的大小
  输  出:   空
  返  回:   平均值
  描  述：  去最大和最小值，中值求平均
 *********************************************************************************/
u16 SampleAverage (u16 array[],u8 num)
{
    u8 max;//应该为u16
    u8 min;//应该为u16
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
  函数名:   ADSamping
  功  能:   AD采样
  输  入:   空
  输  出:   空
  返  回:   空
  描  述：  
 *********************************************************************************/
void ADSamping (void)
{
  u16 buffer;
  
  if (!(ADCCTL1 & ADCBUSY))//检查 ADC 是否空闲，如果空闲则执行后续的采样操作。
  {
    buffer = ADCMEM0; //将数据存储器其值读取到 buffer 中
    //关闭 ADC 的使能和电源
    ADCCTL0 &= ~ADCENC;
    ADCCTL0 &= ~ADCON; 
    ADCMCTL0 = ADCINCH_3 | ADCSREF_1;//配置 ADC 通道和参考电压
    //处理电流采样值
    if ((ADCMCTL0 & 0x000f) == ADCINCH_3)//检查当前选择的通道是否为通道 3,
    {//
      ADC_CUR_Buffer += buffer; //    
      ADC_CUR_Cnt++;            //将采样计数 ADC_CUR_Cnt 加 1
      if (ADC_CUR_Cnt >> 2)  //判断 ADC_CUR_Cnt 是否大于等于 4（右移 2 位相当于除以 4）            
      {
        ADC_CUR_Cnt = 0;  //计数清零
        ADC_CUR_Array[ADC_CUR_Array_Index] = ADC_CUR_Buffer >> 2;//将累加的采样值除以 4 后存储到 ADC_CUR_Array 数组中
        ADC_CUR_Buffer = 0;//累加器清零
        
        ADC_CUR_Array_Index++;//将数组索引 ADC_CUR_Array_Index 加 1
        //判断数组是否已满
        if (ADC_CUR_Array_Index >> 2) //判断 ADC_CUR_Array_Index 是否大于等于 4          
        {
          u16 temp;
          temp = SampleAverage(ADC_CUR_Array,ADC_CUR_Array_Index);//调用 SampleAverage 函数计算这 4 个平均值的平均值，并将结果存储在 temp 中
  
          Current_Current_mA = CURRENT_RECKON(temp);//根据平均值计算当前的电流值
          ADC_CUR_Array_Index = 0;//数组索引清零
        }
      }
      //关闭 ADC 使能和电源，并重新配置通道和参考电压
      ADCCTL0 &= ~ADCENC;
      ADCCTL0 &= ~ADCON; 
      ADCMCTL0 = ADCINCH_3 | ADCSREF_1;
    }
    //开启 ADC 并开始采样
    ADCCTL0 |= ADCSHT_2 | ADCON;//设置16 ADCCLK cycles同时打开ADC
    // Sampling and conversion start
    ADCCTL0 |= ADCENC + ADCSC;                     // Sampling and conversion start
  }
}