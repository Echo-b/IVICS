#include "stm32f10x.h"                  // Device header
#include "MySPI.H"
void MySPI_Init(void)
{
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB ,ENABLE);
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_SPI2,ENABLE);
	GPIO_InitTypeDef GPIO_InitStructure;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_13 | GPIO_Pin_15;//PB13 SCK;PB15 MOSI;PB14 MISO
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOB,&GPIO_InitStructure);
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_14;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOB,&GPIO_InitStructure);
	GPIO_ResetBits(GPIOB,GPIO_Pin_13);
	
	SPI_InitTypeDef SPI_InitStructure;
	SPI_InitStructure.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_256;
	SPI_InitStructure.SPI_CPHA = SPI_CPHA_1Edge;
	SPI_InitStructure.SPI_CPOL = SPI_CPOL_Low;
	SPI_InitStructure.SPI_CRCPolynomial = 7;
	SPI_InitStructure.SPI_DataSize = SPI_DataSize_8b;
	SPI_InitStructure.SPI_Direction = SPI_Direction_2Lines_FullDuplex;
	SPI_InitStructure.SPI_FirstBit = SPI_FirstBit_MSB;
	SPI_InitStructure.SPI_Mode = SPI_Mode_Master;
	SPI_InitStructure.SPI_NSS = SPI_NSS_Soft;
	SPI_Init(SPI2,&SPI_InitStructure);
	
	SPI_Cmd(SPI2,ENABLE);
	
	SPI_RW(0XFF);//发送一个0xff以开启SPI;
}

uint8_t SPI_RW(uint8_t Byte)
{
	uint8_t retry=0;
	while(SPI_I2S_GetFlagStatus(SPI2,SPI_I2S_FLAG_TXE) == RESET)
	{
		retry++;
		if(retry>200) return 0x00;
	}
	SPI_I2S_SendData(SPI2,Byte);
	retry=0;
	while(SPI_I2S_GetFlagStatus(SPI2,SPI_I2S_FLAG_RXNE) == RESET)
	{
		retry++;
		if(retry>200) return 0x00;
	}
	return SPI_I2S_ReceiveData(SPI2);
}
