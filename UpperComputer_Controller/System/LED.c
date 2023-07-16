#include "stm32f10x.h"

void Led_Init()
{
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
	GPIO_InitTypeDef GPIO_InitStructure;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;

	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_7 | GPIO_Pin_8 | GPIO_Pin_15;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;

	GPIO_Init(GPIOA, &GPIO_InitStructure);
	GPIO_ResetBits(GPIOA, GPIO_Pin_8); // ³õÊ¼×´Ì¬ÂÌµÆ
	GPIO_SetBits(GPIOA, GPIO_Pin_7);   // ³õÊ¼×´Ì¬ºìµÆ¹Ø±Õ
	GPIO_SetBits(GPIOA, GPIO_Pin_15);  // ³õÊ¼×´Ì¬»ÆµÆ¹Ø±Õ
	// 7-R,8-G,9-Y
}

void Led_R()
{

	GPIO_ResetBits(GPIOA, GPIO_Pin_7); // LOW
	GPIO_SetBits(GPIOA, GPIO_Pin_8);   // HIGH
	GPIO_SetBits(GPIOA, GPIO_Pin_15);  // HIGH
}

void Led_Y()
{
	GPIO_ResetBits(GPIOA, GPIO_Pin_15); // LOW
	GPIO_SetBits(GPIOA, GPIO_Pin_7);	// HIGH
	GPIO_SetBits(GPIOA, GPIO_Pin_8);	// HIGH
}

void Led_G()
{
	GPIO_ResetBits(GPIOA, GPIO_Pin_8); // LOW
	GPIO_SetBits(GPIOA, GPIO_Pin_7);   // HIGH
	GPIO_SetBits(GPIOA, GPIO_Pin_15);  // HIGH
}

void Led_out()
{
	GPIO_SetBits(GPIOA, GPIO_Pin_8);  // HIGH
	GPIO_SetBits(GPIOA, GPIO_Pin_7);  // HIGH
	GPIO_SetBits(GPIOA, GPIO_Pin_15); // HIGH
}
void Led_twinkle()
{
	GPIO_WriteBit(GPIOA, GPIO_Pin_7, (BitAction)(1 - GPIO_ReadOutputDataBit(GPIOA, GPIO_Pin_7))); // low and high
	GPIO_SetBits(GPIOA, GPIO_Pin_8);															  // HIGH
	GPIO_SetBits(GPIOA, GPIO_Pin_15);															  // HIGH
}
