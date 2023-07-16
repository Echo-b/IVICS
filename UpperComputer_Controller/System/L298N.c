#include "stm32f10x.h"
#include "PWM2.h"

#define IN1 GPIO_Pin_5
#define IN2 GPIO_Pin_6

void L298N_Init(void)
{
	PWM2_Init();
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC, ENABLE);
	GPIO_InitTypeDef GPIO_InitStructure;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;

	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_5 | GPIO_Pin_6;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;

	GPIO_Init(GPIOC, &GPIO_InitStructure);
}

void L298N_RUN(uint16_t speed)
{

	GPIO_SetBits(GPIOC, IN2);
	GPIO_ResetBits(GPIOC, IN1);
	PWM2_SetCompare2(speed);
}

void L298N_BACK(uint16_t speed)
{

	GPIO_SetBits(GPIOC, IN1);
	GPIO_ResetBits(GPIOC, IN2);
	PWM2_SetCompare2(speed);
}