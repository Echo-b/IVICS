/**
* @par Copyright (C): 2010-2019, Shenzhen Yahboom Tech
* @file         app_ultrasonic.C
* @author       liusen
* @version      V1.0
* @date         2017.07.21
* @brief        ������������ͷ�ļ�
* @details      
* @par History  ������˵��
*                 
* version:	liusen_20170717
*/

#include "bsp_ultrasonic.h"
#include "delay.h"
#include "stm32f10x.h"

/*��¼��ʱ���������*/
unsigned int overcount = 0;

/**
* Function       bsp_getUltrasonicDistance
* @author        liusen
* @date          2017.07.20    
* @brief         ��ȡ��������
* @param[in]     void
* @param[out]    void
* @return        ���븡��ֵ
* @par History   ��
*/

float bsp_getUltrasonicDistance(void) {
    float        length = 0, sum = 0;
    u16          tim, count;
    unsigned int i = 0;

    while (i != 5) {
        /*�����ź�*/
        GPIO_SetBits(TRIG_PORT, TRIG_PIN);
        delay_us(20);
        GPIO_ResetBits(TRIG_PORT, TRIG_PIN);

        while (GPIO_ReadInputDataBit(ECHO_PORT, ECHO_PIN) == RESET)
            ;
        TIM_Cmd(TIM2, ENABLE);

        //i+=1;
        i++;
        while (GPIO_ReadInputDataBit(ECHO_PORT, ECHO_PIN) == SET) {
            count = overcount;
            if (count >= 40)  //
            {
                TIM_Cmd(TIM2, DISABLE);
                TIM2->CNT = 0;
                overcount = 0;
                return 0;
            }
        }
        TIM_Cmd(TIM2, DISABLE);

        tim = TIM_GetCounter(TIM2);

        length = (tim + overcount * 1000) / 58.0;

        sum       = length + sum;
        TIM2->CNT = 0;
        overcount = 0;
        delay_ms(10);
    }
    //length = sum / 5;
    length = sum;
    return length;
}

/**
* Function       bsp_Ultrasonic_Timer2_Init
* @author        liusen
* @date          2017.07.21    
* @brief         ��ʼ����ʱ��TIM2
* @param[in]     void
* @param[out]    void
* @return        ���븡��ֵ
* @par History   ��
*/

void bsp_Ultrasonic_Timer2_Init(void) {
    /*��ʼ��ʱ����Ԫ�ṹ��*/
    TIM_TimeBaseInitTypeDef TIM_TimeBaseInitStructer;
    /*��ʼ��NVIC�ṹ��*/
    NVIC_InitTypeDef NVIC_InitStructer;

    /*����TIM2��ʱ��*/
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2, ENABLE);

    /*��ʱ��TIM2��ʼ��*/
    TIM_DeInit(TIM2);
    TIM_TimeBaseInitStructer.TIM_Period            = 999;           //��ʱ����Ϊ1000
    TIM_TimeBaseInitStructer.TIM_Prescaler         = 71;            //��Ƶϵ��72
    TIM_TimeBaseInitStructer.TIM_ClockDivision     = TIM_CKD_DIV1;  //����Ƶ
    TIM_TimeBaseInitStructer.TIM_CounterMode       = TIM_CounterMode_Up;
    TIM_TimeBaseInitStructer.TIM_RepetitionCounter = 0;  //�߼���ʱ����ʹ��
    TIM_TimeBaseInit(TIM2, &TIM_TimeBaseInitStructer);

    /*�����жϣ�����ʱ�����ж�*/
    TIM_ITConfig(TIM2, TIM_IT_Update, ENABLE);

    /*��ʱ���жϳ�ʼ��*/
    //NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);

    NVIC_InitStructer.NVIC_IRQChannelPreemptionPriority = 0;
    NVIC_InitStructer.NVIC_IRQChannelSubPriority        = 0;
    NVIC_InitStructer.NVIC_IRQChannel                   = TIM2_IRQn;
    NVIC_InitStructer.NVIC_IRQChannelCmd                = ENABLE;

    NVIC_Init(&NVIC_InitStructer);
    TIM_Cmd(TIM2, DISABLE);  //�رն�ʱ��ʹ��
}

void TIM2_IRQHandler(void)  //�жϣ��������źźܳ�ʱ������ֵ������ظ����������ж��������������
{
    if (TIM_GetITStatus(TIM2, TIM_IT_Update) != RESET) {
        TIM_ClearITPendingBit(TIM2, TIM_IT_Update);  //����жϱ�־
        overcount++;
    }
}
