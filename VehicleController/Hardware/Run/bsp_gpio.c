/**
* @par Copyright (C): 2010-2019, Shenzhen Yahboom Tech
* @file         bsp_gpio.c
* @author       liusen
* @version      V1.0
* @date         2015.01.03
* @brief        ����gpioԴ�ļ�
* @details      
* @par History  ������˵��
*                 
* version:	liusen_20170717
*/

#include "bsp_gpio.h"
#include "bsp_motor.h"
#include "bsp_ultrasonic.h"
#include "stm32f10x.h"

/**
* Function       MOTOR_GPIO_Config
* @author        liusen
* @date          2015.01.03    
* @brief         ���GPIO�ڳ�ʼ��
* @param[in]     void
* @param[out]    void
* @retval        void
* @par History   ��
*/

void MOTOR_GPIO_Init(void) {
    /*����һ��GPIO_InitTypeDef���͵Ľṹ��*/
    GPIO_InitTypeDef GPIO_InitStructure;

    /*����GPIOB������ʱ��*/
    RCC_APB2PeriphClockCmd(Motor_RCC, ENABLE);

    /*ѡ��Ҫ���Ƶ�GPIOB����*/
    GPIO_InitStructure.GPIO_Pin = Left_MotoA_Pin | Left_MotoB_Pin | Right_MotoA_Pin | Right_MotoB_Pin;

    /*��������ģʽΪͨ���������*/
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;

    /*������������Ϊ50MHz */
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;

    /*���ÿ⺯������ʼ��GPIOB*/
    GPIO_Init(Motor_Port, &GPIO_InitStructure);

    /* �͵�ƽ	*/
    GPIO_ResetBits(Motor_Port, Left_MotoA_Pin | Left_MotoB_Pin | Right_MotoA_Pin | Right_MotoB_Pin);

    //	//PB4�ر�JTAG
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB | RCC_APB2Periph_AFIO, ENABLE);  //ʹ��GPIO�����AFIO���ù���ģ��ʱ��
    GPIO_PinRemapConfig(GPIO_Remap_SWJ_JTAGDisable, ENABLE);                     //����jtag
}

/**
* Function       Ultrasonic_GPIO_Init
* @author        liusen
* @date          2017.07.20    
* @brief         ������������GPIO��ʼ���ӿ�
* @param[in]     void
* @param[out]    void
* @retval        void
* @par History   ��
*/
void Ultrasonic_GPIO_Init(void) {
    /*����һ��GPIO_InitTypeDef���͵Ľṹ��*/
    GPIO_InitTypeDef GPIO_InitStructure;
    /*��������ʱ��*/
    RCC_APB2PeriphClockCmd(TRIG_RCC | RCC_APB2Periph_AFIO, ENABLE);
    GPIO_PinRemapConfig(GPIO_Remap_SWJ_JTAGDisable, ENABLE);  //����jtag
                                                              /*TRIG�����ź�*/
    GPIO_InitStructure.GPIO_Pin = TRIG_PIN;
    /*��������ģʽΪͨ���������*/
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
    /*������������Ϊ50MHz */
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    /*���ÿ⺯������ʼ��*/
    GPIO_Init(TRIG_PORT, &GPIO_InitStructure);
    GPIO_ResetBits(TRIG_PORT, TRIG_PIN);

    /*��������ʱ��*/
    RCC_APB2PeriphClockCmd(ECHO_RCC, ENABLE);
    /*ECOH�����ź�*/
    GPIO_InitStructure.GPIO_Pin = ECHO_PIN;
    /*��������ģʽΪͨ���������*/
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
    /*������������Ϊ50MHz */
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    /*���ÿ⺯������ʼ��PORT*/
    GPIO_Init(ECHO_PORT, &GPIO_InitStructure);
}
