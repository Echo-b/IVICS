/**
* @par Copyright (C): 2010-2019, Shenzhen Yahboom Tech
* @file         app_ultrasonic.C
* @author       liusen
* @version      V1.0
* @date         2017.07.21
* @brief        超声波传感器头文件
* @details      
* @par History  见如下说明
*                 
* version:	liusen_20170717
*/

#include "bsp_ultrasonic.h"
#include "delay.h"
#include "stm32f10x.h"

/*记录定时器溢出次数*/
unsigned int overcount = 0;

/**
* Function       bsp_getUltrasonicDistance
* @author        liusen
* @date          2017.07.20    
* @brief         获取超声距离
* @param[in]     void
* @param[out]    void
* @return        距离浮点值
* @par History   无
*/

float bsp_getUltrasonicDistance(void) {
    float        length = 0, sum = 0;
    u16          tim, count;
    unsigned int i = 0;

    while (i != 5) {
        /*触发信号*/
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
* @brief         初始化定时器TIM2
* @param[in]     void
* @param[out]    void
* @return        距离浮点值
* @par History   无
*/

void bsp_Ultrasonic_Timer2_Init(void) {
    /*初始化时基单元结构体*/
    TIM_TimeBaseInitTypeDef TIM_TimeBaseInitStructer;
    /*初始化NVIC结构体*/
    NVIC_InitTypeDef NVIC_InitStructer;

    /*开启TIM2的时钟*/
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2, ENABLE);

    /*定时器TIM2初始化*/
    TIM_DeInit(TIM2);
    TIM_TimeBaseInitStructer.TIM_Period            = 999;           //定时周期为1000
    TIM_TimeBaseInitStructer.TIM_Prescaler         = 71;            //分频系数72
    TIM_TimeBaseInitStructer.TIM_ClockDivision     = TIM_CKD_DIV1;  //不分频
    TIM_TimeBaseInitStructer.TIM_CounterMode       = TIM_CounterMode_Up;
    TIM_TimeBaseInitStructer.TIM_RepetitionCounter = 0;  //高级定时器才使用
    TIM_TimeBaseInit(TIM2, &TIM_TimeBaseInitStructer);

    /*开启中断，更新时触发中断*/
    TIM_ITConfig(TIM2, TIM_IT_Update, ENABLE);

    /*定时器中断初始化*/
    //NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);

    NVIC_InitStructer.NVIC_IRQChannelPreemptionPriority = 0;
    NVIC_InitStructer.NVIC_IRQChannelSubPriority        = 0;
    NVIC_InitStructer.NVIC_IRQChannel                   = TIM2_IRQn;
    NVIC_InitStructer.NVIC_IRQChannelCmd                = ENABLE;

    NVIC_Init(&NVIC_InitStructer);
    TIM_Cmd(TIM2, DISABLE);  //关闭定时器使能
}

void TIM2_IRQHandler(void)  //中断，当回响信号很长时，计数值溢出后重复计数，用中断来保存溢出次数
{
    if (TIM_GetITStatus(TIM2, TIM_IT_Update) != RESET) {
        TIM_ClearITPendingBit(TIM2, TIM_IT_Update);  //清除中断标志
        overcount++;
    }
}
