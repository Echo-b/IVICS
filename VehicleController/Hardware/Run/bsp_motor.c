
#include "bsp_motor.h"

//TIM3 PWM部分初始化
//PWM输出初始化
//arr：自动重装值
//psc：时钟预分频数

/**
* Function       Motor_PWM_Init
* @author        liusen
* @date          2017.07.17    
* @brief         电机PWM口初始化
* @param[in]     左电机 arr：自动重装值  psc：时钟预分频数	 右电机 arr2：自动重装值  psc2：时钟预分频数
* @param[out]    void
* @retval        void
* @par History   无
*/
void Motor_PWM_Init(u16 arr, u16 psc, u16 arr2, u16 psc2) {

    //开启TIM4的时钟
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM4, ENABLE);                         //使能定时器3时钟
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB | RCC_APB2Periph_AFIO, ENABLE);  //使能GPIO外设和AFIO复用功能模块时钟

    //GPIO初始化结构体
    GPIO_InitTypeDef GPIO_InitStructure;
    //设置该引脚为复用输出功能,输出TIM4 CH2的PWM脉冲波形	GPIOB.7
    GPIO_InitStructure.GPIO_Pin   = GPIO_Pin_7;       //TIM_CH2
    GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_AF_PP;  //复用推挽输出
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOB, &GPIO_InitStructure);  //初始化GPIO

    //设置该引脚为复用输出功能,输出TIM4 CH1的PWM脉冲波形	GPIOB.6
    GPIO_InitStructure.GPIO_Pin   = GPIO_Pin_6;       //TIM4_CH1
    GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_AF_PP;  //复用推挽输出
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOB, &GPIO_InitStructure);  //初始化GPIO

    //定时器时基单元初始化结构体
    TIM_TimeBaseInitTypeDef TIM_TimeBaseStructure;
    //初始化TIM4的时基单元
    TIM_TimeBaseStructure.TIM_Period            = arr - 1;             //设置在下一个更新事件装入活动的自动重装载寄存器周期的值
    TIM_TimeBaseStructure.TIM_Prescaler         = psc - 1;             //设置用来作为TIMx时钟频率除数的预分频值
    TIM_TimeBaseStructure.TIM_ClockDivision     = TIM_CKD_DIV1;        //设置时钟分割:TDTS = Tck_tim
    TIM_TimeBaseStructure.TIM_CounterMode       = TIM_CounterMode_Up;  //TIM向上计数模式
    TIM_TimeBaseStructure.TIM_RepetitionCounter = 0;
    TIM_TimeBaseInit(TIM4, &TIM_TimeBaseStructure);  //根据TIM_TimeBaseInitStruct中指定的参数初始化TIMx的时间基数单位

    //比较输出单元结构体
    TIM_OCInitTypeDef TIM_OCInitStructure;
    TIM_OCStructInit(&TIM_OCInitStructure);  //进行一次基本的初始化
    //初始化TIM4 Channel1 PWM模式
    TIM_OCInitStructure.TIM_OCMode      = TIM_OCMode_PWM1;         //选择定时器模式:TIM脉冲宽度调制模式1
    TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;  //比较输出使能
    TIM_OCInitStructure.TIM_OCPolarity  = TIM_OCPolarity_High;     //输出极性:TIM输出比较极性高
    TIM_OCInitStructure.TIM_Pulse       = 0;                       //CCR
    TIM_OC2Init(TIM4, &TIM_OCInitStructure);                       //根据T指定的参数初始化外设TIM4 OC2
    //TIM_OC2PreloadConfig(TIM4, TIM_OCPreload_Enable);              //使能TIM4在CCR2上的预装载寄存器
    //TIM_Cmd(TIM4, ENABLE);  //使能TIM4

    //PWMB PB6
    //RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM4, ENABLE);                         //使能定时器3时钟

    //初始化TIM4
    // TIM_TimeBaseStructure.TIM_Period        = arr2;                //设置在下一个更新事件装入活动的自动重装载寄存器周期的值
    // TIM_TimeBaseStructure.TIM_Prescaler     = psc2;                //设置用来作为TIMx时钟频率除数的预分频值
    // TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1;        //设置时钟分割:TDTS = Tck_tim
    // TIM_TimeBaseStructure.TIM_CounterMode   = TIM_CounterMode_Up;  //TIM向上计数模式
    // TIM_TimeBaseInit(TIM4, &TIM_TimeBaseStructure);                //根据TIM_TimeBaseInitStruct中指定的参数初始化TIMx的时间基数单位

    //初始化TIM3 Channel2 PWM模式
    // TIM_OCInitStructure.TIM_OCMode      = TIM_OCMode_PWM1;         //选择定时器模式:TIM脉冲宽度调制模式1
    // TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;  //比较输出使能
    // TIM_OCInitStructure.TIM_OCPolarity  = TIM_OCPolarity_High;     //输出极性:TIM输出比较极性高
    // TIM_OCInitStructure.TIM_Pulse       = 0;
    TIM_OC1Init(TIM4, &TIM_OCInitStructure);  //根据T指定的参数初始化外设TIM3 OC2
    //TIM_OC1PreloadConfig(TIM4, TIM_OCPreload_Enable);  //使能TIM4在CCR2上的预装载寄存器
    TIM_Cmd(TIM4, ENABLE);  //使能TIM3
}
