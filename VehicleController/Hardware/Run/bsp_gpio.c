/**
* @par Copyright (C): 2010-2019, Shenzhen Yahboom Tech
* @file         bsp_gpio.c
* @author       liusen
* @version      V1.0
* @date         2015.01.03
* @brief        驱动gpio源文件
* @details      
* @par History  见如下说明
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
* @brief         电机GPIO口初始化
* @param[in]     void
* @param[out]    void
* @retval        void
* @par History   无
*/

void MOTOR_GPIO_Init(void) {
    /*定义一个GPIO_InitTypeDef类型的结构体*/
    GPIO_InitTypeDef GPIO_InitStructure;

    /*开启GPIOB的外设时钟*/
    RCC_APB2PeriphClockCmd(Motor_RCC, ENABLE);

    /*选择要控制的GPIOB引脚*/
    GPIO_InitStructure.GPIO_Pin = Left_MotoA_Pin | Left_MotoB_Pin | Right_MotoA_Pin | Right_MotoB_Pin;

    /*设置引脚模式为通用推挽输出*/
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;

    /*设置引脚速率为50MHz */
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;

    /*调用库函数，初始化GPIOB*/
    GPIO_Init(Motor_Port, &GPIO_InitStructure);

    /* 低电平	*/
    GPIO_ResetBits(Motor_Port, Left_MotoA_Pin | Left_MotoB_Pin | Right_MotoA_Pin | Right_MotoB_Pin);

    //	//PB4关闭JTAG
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB | RCC_APB2Periph_AFIO, ENABLE);  //使能GPIO外设和AFIO复用功能模块时钟
    GPIO_PinRemapConfig(GPIO_Remap_SWJ_JTAGDisable, ENABLE);                     //禁能jtag
}

/**
* Function       Ultrasonic_GPIO_Init
* @author        liusen
* @date          2017.07.20    
* @brief         超声波传感器GPIO初始化接口
* @param[in]     void
* @param[out]    void
* @retval        void
* @par History   无
*/
void Ultrasonic_GPIO_Init(void) {
    /*定义一个GPIO_InitTypeDef类型的结构体*/
    GPIO_InitTypeDef GPIO_InitStructure;
    /*开启外设时钟*/
    RCC_APB2PeriphClockCmd(TRIG_RCC | RCC_APB2Periph_AFIO, ENABLE);
    GPIO_PinRemapConfig(GPIO_Remap_SWJ_JTAGDisable, ENABLE);  //禁能jtag
                                                              /*TRIG触发信号*/
    GPIO_InitStructure.GPIO_Pin = TRIG_PIN;
    /*设置引脚模式为通用推挽输出*/
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
    /*设置引脚速率为50MHz */
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    /*调用库函数，初始化*/
    GPIO_Init(TRIG_PORT, &GPIO_InitStructure);
    GPIO_ResetBits(TRIG_PORT, TRIG_PIN);

    /*开启外设时钟*/
    RCC_APB2PeriphClockCmd(ECHO_RCC, ENABLE);
    /*ECOH回响信号*/
    GPIO_InitStructure.GPIO_Pin = ECHO_PIN;
    /*设置引脚模式为通用推挽输出*/
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
    /*设置引脚速率为50MHz */
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    /*调用库函数，初始化PORT*/
    GPIO_Init(ECHO_PORT, &GPIO_InitStructure);
}
