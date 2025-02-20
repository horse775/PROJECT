/**********************************************************************************
 *  ��Ŀ���:    
 *  ��Ŀ����:    
 *  �ͻ�����:                                    
 *  �����д:    
 *  ��������:    
 *  �������:    
 *  �汾����:    V1.1
 *
 *  Define.h file
 *  ��������: ȫ�ֺ궨��
**********************************************************************************/
#ifndef Define_h
    #define Define_h
    
//ͷ�ļ�--------------------------------------------------------------------------------
    #include "Typedef.h"//���Ͷ���ͷ�ļ�
    #include "MSP430.h"
    #include "string.h"
    
//ȫ�ֺ궨��----------------------------------------------------------------------------
    //#define NULL                   0
    #define TRUE                   1
    #define SYSTEM_CLOCK           8
    #define RF_DECODE_TIME         50
    #define SYSTEM_TIME            2
    #define SYSTEM_TIME_VALUE      SYSTEM_TIME * 1000
                                   
    #define ID_MAX_AMOUNT          9//�ɱ���ID���������
    #define ID_LENGTH              5//���ݳ���

    #define EXTERNAL_POWER_SUPPLY//����12V�����أ�����14.8V﮵�أ�
    #define INCHING_FIXED_ANGLE//�㶯�̶��Ƕ�

    //#define TORSION_RPM            20//6N/20rpm
    #define TORSION_RPM            13//10N/13rpm
    //#define TORSION_RPM            18//6N/18rpm
    //#define TORSION_RPM            25//3N/25rpm

    //3N/25rpm
    #if (TORSION_RPM == 25)  
      #ifdef EXTERNAL_POWER_SUPPLY//����12V������
        #define POWER_DOWN_VAULE   950
        #define LOW_VOL_HINT_VALUE 1080//ʵ��ֵ��С0.2,ʵ��10.8V����
        #define VOL_NORMAL_VALUE   1150//ʵ��11.3V�ָ�
                                   
        #define VOL_RECOVERY_VALUE 1150
      #else   // ����14.8V﮵��   
        #define POWER_DOWN_VAULE   1000
        #define LOW_VOL_HINT_VALUE 1060
        #define VOL_NORMAL_VALUE   1100
                                   
        #define VOL_RECOVERY_VALUE 1130
      #endif                       
                                   
      #define MAX_SPEED            25
      #define MIN_SPEED            8
      #define STARTUP_CURRENT      2800//������������ֵ
      #define MAX_OUT_CURRENT      3300
                                   
      #define REDUCTION_RATIO      1560
      #define SPEED                ((300000000 / REDUCTION_RATIO) / RF_DECODE_TIME)//(60000000/2)/�ٱ�156
      #define SPEED_COUNT(var)     ((SPEED + (var >> 1)) / (var))  

    //10N/13rpm 
    #elif (TORSION_RPM == 13)
      //����12V������ �����20KG  ����0.4Vѹ��  
      //�����طŵ����� 11.3V�½�Ѹ��   10.5V��ֹ
      
      #ifdef EXTERNAL_POWER_SUPPLY     
        #define POWER_DOWN_VAULE   950
        #define LOW_VOL_HINT_VALUE 1080//ʵ��ֵ������10.7V����������11.1
        #define VOL_NORMAL_VALUE   1150//ʵ��11.3V�ָ�
                                   
        #define VOL_RECOVERY_VALUE 1150
      #else //����14.8V﮵��   
        #define POWER_DOWN_VAULE   1000
        #define LOW_VOL_HINT_VALUE 1060
        #define VOL_NORMAL_VALUE   1100
                                   
        #define VOL_RECOVERY_VALUE 1130
      #endif                       
                                   
      #define MAX_SPEED            6//20RPM
      #define MIN_SPEED            1
      #define STARTUP_CURRENT      3950//������������ֵ
      #define MAX_OUT_CURRENT      4500
                                   
      #define REDUCTION_RATIO      4770
      #define SPEED                ((300000000 / REDUCTION_RATIO) / RF_DECODE_TIME)//(60000000/2)/�ٱ�477
      #define SPEED_COUNT(var)     ((SPEED + (var >> 1)) / (var))  
                                    
    //6N/18rpm                      
    #elif (TORSION_RPM == 18)        
      #ifdef EXTERNAL_POWER_SUPPLY//����12V������
        #define POWER_DOWN_VAULE   950
        #define LOW_VOL_HINT_VALUE 1080//ʵ��ֵ��С0.2  ʵ��10.8V����
        #define VOL_NORMAL_VALUE   1150//ʵ��11.3V�ָ�
                                   
        #define VOL_RECOVERY_VALUE 1150
      #else //����14.8V﮵��      
        #define POWER_DOWN_VAULE   1000
        #define LOW_VOL_HINT_VALUE 1060
        #define VOL_NORMAL_VALUE   1100
                                   
        #define VOL_RECOVERY_VALUE 1130
      #endif                       
                                   
      #define MAX_SPEED            18
      #define MIN_SPEED            6
      #define STARTUP_CURRENT      3800//������������ֵ
      #define MAX_OUT_CURRENT      4500
                                   
      #define REDUCTION_RATIO      1560
      #define SPEED                ((300000000 / REDUCTION_RATIO) / RF_DECODE_TIME)//(60000000/2)/�ٱ�156
      #define SPEED_COUNT(var)     ((SPEED + (var >> 1)) / (var))  

    //6N/20rpm  ˳����������޵��ɲ����
    #elif (TORSION_RPM == 20)       
      #ifdef EXTERNAL_POWER_SUPPLY//����12V������
        #define POWER_DOWN_VAULE   950
        #define LOW_VOL_HINT_VALUE 1080//ʵ��ֵ��С0.2  ʵ��10.8V����
        #define VOL_NORMAL_VALUE   1150//ʵ��11.3V�ָ�

        #define VOL_RECOVERY_VALUE 1150
      #else //����14.8V﮵��
        #define POWER_DOWN_VAULE   1000
        #define LOW_VOL_HINT_VALUE 1060
        #define VOL_NORMAL_VALUE   1100
      
        #define VOL_RECOVERY_VALUE 1130
      #endif

      #define MAX_SPEED            20
      #define MIN_SPEED            5
      #define STARTUP_CURRENT      4000//������������ֵ
      #define MAX_OUT_CURRENT      5500

      #define REDUCTION_RATIO      2410//1800
      #define SPEED                ((300000000 / REDUCTION_RATIO) / RF_DECODE_TIME)//(60000000/2)/�ٱ�241
      #define SPEED_COUNT(var)     ((SPEED + (var >> 1)) / (var))  
    #endif

    #define NO_VOL_SAMPLING_TIME   (500 / (TIME_10MS_VALUE * SYSTEM_TIME))
    //Ϊ�˴�������һ��ѭ����ͬʱ����50��100MS�����飬���Խ�ʱ����΢��
    #define TIME_200US_VALUE       (400  / RF_DECODE_TIME)
    #define TIME_10MS_VALUE        (16  / SYSTEM_TIME)
    #define TIME_50MS_VALUE        (45  / SYSTEM_TIME)
    #define TIME_100MS_VALUE       (103 / SYSTEM_TIME)
    #define TIME_500MS_VALUE       (500 / SYSTEM_TIME)
    #define TIME_1S_VALUE          (1000 / 500)

    #define HALL_PULSE_MAX         16777210
    #define MIN_JOURNEY_VALUE      8000000
    #define HALL_PULSE_MIN         1000
    #define MID_LIMIT_POINT_AMOUNT 4

    #define LIMIT_ACTIVATE         1
    #define LIMIT_ADJUST           2
    #define LIMIT_PLACE            3
    #define LIMIT_SET_RANGE        4
    
    #define SET_UP_DIRECTION()     (MotorFlag.Motor_Direction = !MotorFlag.Direction)
    #define SET_DOWN_DIRECTION()   (MotorFlag.Motor_Direction = MotorFlag.Direction)

    
    #define PWM_CYC                16
    #define MAX_DUTY               ((u16) (((SYSTEM_CLOCK * 1000) / PWM_CYC) / 2))// up down mode ��Ҫ�ٳ�2
    #define MIN_DUTY               10

    //RF��������
    #define RF_DATA_HEAD           0xA3//RFͷ��
    #define RF_DATA_UP             0x0b//RF��������
    #define RF_DATA_STOP           0x23//RFֹͣ����
    #define RF_STOP_2S             0x5a//RFֹͣ����2S
    #define RF_JIGGLE_STOP         0x24//RF�㶯ͣ����
    #define RF_DATA_DOWN           0x43//RF��������
    #define RF_DATA_LEARN          0x53//RFѧϰ����
    #define RF_DATA_CLEAR          0x2b//RF�������
    #define RF_DATA_AWOKEN         0x50//RF��������
    #define RF_DATA_UP_DOWN_1S     0x4a
    
    #define RF_UP_DOWN_5S          0x3a
    #define RF_UP_STOP             0x1a
    #define RF_DOWN_STOP           0x2a
    #define RF_ACCESS_LEARN        0x55
                                                             
    #define RF_CONSECUTIVE_UP      0x8B//RF������������
    #define RF_CONSECUTIVE_DOWN    0xC3//RF������������                                                             
    #define RF_DATA_RESET_NETWORK  0x28
    //�������ʱ��
    #define MOTOR_RUN_TIME         5//�����8����
    #define MOTOR_RUN_CNT_VALUE    (MOTOR_RUN_TIME * (60000 / (TIME_10MS_VALUE * SYSTEM_TIME)))
    #define MOTOR_SWITCH_LATIME    (500 / (TIME_10MS_VALUE * SYSTEM_TIME))
    #define JOINT_ACTION_TIME      (1500 / (TIME_10MS_VALUE * SYSTEM_TIME))

    //IO �˿ڶ���----------------------------------------------------------------------
    #define RF_SIGNAL_SAMPLE()    (P4IN & BIT0)//P4.0����״̬

    #define PWRC                   4//P2.4���״̬
#define VCC_ON()              (P2OUT |=  (1 << PWRC))
#define VCC_OFF()             (P2OUT &= ~(1 << PWRC))

#define MOTOR_EN               3//P3.3���״̬
#define MOTOR_WORK()          (P3OUT |=  (1 << MOTOR_EN))
#define MOTOR_SLEEP()         (P3OUT &= ~(1 << MOTOR_EN))

#define BRAKE                  5//P3.5���״̬ Clutch
#define BRAKE_OFF()           (P3OUT |=  (1 << BRAKE))
#define BRAKE_ON()            (P3OUT &= ~(1 << BRAKE))

#define MOTOR_PH               1//P5.1���״̬
#define MOTOR_PH_H()          (P5OUT |=  (1 << MOTOR_PH))
#define MOTOR_PH_L()          (P5OUT &= ~(1 << MOTOR_PH))

#define MOTOR_nFault           4//P3.4����״̬
#define MOTOR_NFAULT()        (P3IN & (1<<MOTOR_nFault))

//#define SYSTEM_POWER_IO        0//P3.0����״̬
//#define SYSTEM_POWER_IN()     (P3IN & (1<<SYSTEM_POWER_IO))

#define RS485                  1//P4.1���״̬
#define RS485_TX()            (P4OUT |=  (1 << RS485))
#define RS485_RX()            (P4OUT &= ~(1 << RS485))


#define UART_MAX        70  //���ڷ��ͽ��ջ�������С

    #define CURRENT_ANGLE_180_HALL        120  //���ڷ��ͽ��ջ�������С

#define POWER_IN_NUM           3

#define POWER_UP               0
#define POWER_DOWN             1
#define POWER_IN               2

#define POWER_UP_IN            BIT1
#define POWER_DOWN_IN          BIT2
#define SYSTEM_POWER_IO        BIT0

#define CONTROL_UP_IN()        (((~P2IN) & BIT2) >> 1)
#define CONTROL_DOWN_IN()      ((~P3IN) & POWER_DOWN_IN)
#define /*SYSTEM_POWER_IN()*/POWER_DETECT()       ((~P3IN) & SYSTEM_POWER_IO)
#endif