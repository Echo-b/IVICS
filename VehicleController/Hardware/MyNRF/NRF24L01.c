#include "NRF24L01.h"
#include "delay.h"
#include "spi.h"
#include "stm32f10x.h"  // Device header

//引脚连接 ：CSN->PA12（PA8）  CE->PB15（PA1）  SCK->PA11（PB13）  MOSI->PA10（PB15）  MISO->PA9（PB14）  IRQ->PA8
#define CSN_1 GPIO_SetBits(GPIOA, GPIO_Pin_8);
#define CSN_0 GPIO_ResetBits(GPIOA, GPIO_Pin_8);
#define CE_1 GPIO_SetBits(GPIOA, GPIO_Pin_1);
#define CE_0 GPIO_ResetBits(GPIOA, GPIO_Pin_1);
//#define SCK(x) GPIO_WriteBit(GPIOA,GPIO_Pin_11,(BitAction)x)
//#define MOSI(x) GPIO_WriteBit(GPIOA,GPIO_Pin_10,(BitAction)x)
//#define MISO() GPIO_ReadInputDataBit(GPIOA,GPIO_Pin_9)
#define IRQ() GPIO_ReadInputDataBit(GPIOG, GPIO_Pin_6)

#define CHANAL 20
#define RF_Config 0x27
unsigned char ADDR[TX_ADR_WIDTH] = { 0x00, 0x00, 0xA3, 0xA5, 0xA6 };

void NRF_Init(void) {
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
    GPIO_InitTypeDef GPIO_Initstrucure;
    GPIO_Initstrucure.GPIO_Mode  = GPIO_Mode_Out_PP;
    GPIO_Initstrucure.GPIO_Pin   = GPIO_Pin_1 | GPIO_Pin_8;
    GPIO_Initstrucure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOA, &GPIO_Initstrucure);

    //	GPIO_Initstrucure.GPIO_Mode = GPIO_Mode_Out_PP;
    //	GPIO_Initstrucure.GPIO_Pin =GPIO_Pin_15;
    //	GPIO_Initstrucure.GPIO_Speed = GPIO_Speed_50MHz;
    //	GPIO_Init(GPIOB,&GPIO_Initstrucure);//这里注意CE为PB引脚

    //	GPIO_Initstrucure.GPIO_Mode = GPIO_Mode_IN_FLOATING;//将IRQ对应的引脚设置为浮空输入模式
    //	GPIO_Initstrucure.GPIO_Pin =GPIO_Pin_8;
    //	GPIO_Initstrucure.GPIO_Speed = GPIO_Speed_50MHz;
    //	GPIO_Init(GPIOA,&GPIO_Initstrucure);
    //	GPIO_Initstrucure.GPIO_Mode = GPIO_Mode_IPU;
    //	GPIO_Initstrucure.GPIO_Pin =GPIO_Pin_9;
    //	GPIO_Initstrucure.GPIO_Speed = GPIO_Speed_50MHz;
    //	GPIO_Init(GPIOA,&GPIO_Initstrucure);

    GPIO_ResetBits(GPIOA, GPIO_Pin_1 | GPIO_Pin_8);  //PA1,8上拉

    SPI_InitTypeDef SPI_InitStructure;
    SPI2_Init();  //初始化SPI

    SPI_Cmd(SPI2, DISABLE);  // SPI外设不使能

    SPI_InitStructure.SPI_Direction         = SPI_Direction_2Lines_FullDuplex;  //SPI设置为双线双向全双工
    SPI_InitStructure.SPI_Mode              = SPI_Mode_Master;                  //SPI主机
    SPI_InitStructure.SPI_DataSize          = SPI_DataSize_8b;                  //发送接收8位帧结构
    SPI_InitStructure.SPI_CPOL              = SPI_CPOL_Low;                     //时钟悬空低
    SPI_InitStructure.SPI_CPHA              = SPI_CPHA_1Edge;                   //数据捕获于第1个时钟沿
    SPI_InitStructure.SPI_NSS               = SPI_NSS_Soft;                     //NSS信号由软件控制
    SPI_InitStructure.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_16;         //定义波特率预分频的值:波特率预分频值为16
    SPI_InitStructure.SPI_FirstBit          = SPI_FirstBit_MSB;                 //数据传输从MSB位开始
    SPI_InitStructure.SPI_CRCPolynomial     = 7;                                //CRC值计算的多项式
    SPI_Init(SPI2, &SPI_InitStructure);                                         //根据SPI_InitStruct中指定的参数初始化外设SPIx寄存器

    SPI_Cmd(SPI2, ENABLE);  //使能SPI外设

    CSN_1;
    CE_0;
}

//unsigned char SPI_RW(unsigned char Byte)
//{
//	unsigned char i;
//	for(i=0;i<8;i++)
//	{
//		MOSI(Byte&(0x80));
//		Byte = (Byte<<1);
//		SCK(1);
//		Byte |= (unsigned char)MISO();
//		SCK (0);
//	}
//	return Byte;
//}

unsigned char NRF_WriteReg(unsigned char reg, unsigned char value) {
    uint8_t status = 0;
    CSN_0;
    delay_us(1);
    status = SPI2_ReadWriteByte(reg);
    delay_us(1);
    SPI2_ReadWriteByte(value);
    delay_us(1);
    CSN_1;
    return status;
}

unsigned char NRF_ReadReg(unsigned char reg) {
    uint8_t value = 0;
    CSN_0;
    delay_us(1);
    SPI2_ReadWriteByte(reg);
    delay_us(1);
    value = SPI2_ReadWriteByte(NOP);
    delay_us(1);
    CSN_1;
    return value;
}

unsigned char NRF_WriteBuff(unsigned char reg, unsigned char *buf, unsigned char Length) {
    uint8_t status, i;
    CSN_0;
    status = SPI2_ReadWriteByte(reg);
    delay_us(1);
    for (i = 0; i < Length; i++) {
        SPI2_ReadWriteByte(buf[i]);
        delay_us(1);
    }
    CSN_1;
    return status;
}

unsigned char NRF_ReadBuff(unsigned char reg, unsigned char *buf, unsigned char Length) {
    uint8_t status, i;
    CSN_0;
    delay_us(1);
    status = SPI2_ReadWriteByte(reg);
    delay_us(1);
    for (i = 0; i < Length; i++) {
        buf[i] = SPI2_ReadWriteByte(NOP);
        delay_us(1);
    }
    CSN_1;
    return status;
}

void NRF_RXMode(void) {
    CE_0;
    delay_us(1);
    NRF_WriteBuff(NRF_WRITE_REG + RX_ADDR_P0, ADDR, TX_ADR_WIDTH);  //写命令，数据管道0接收地址
    delay_us(1);
    NRF_WriteReg(FLUSH_RX, NOP);
    delay_us(1);
    NRF_WriteReg(NRF_WRITE_REG + EN_AA, 0x01);  //  使能数据管道0自动确认（应答）
    delay_us(1);
    NRF_WriteReg(NRF_WRITE_REG + EN_RXADDR, 0x01);  //使能接受数据管道0地址
    delay_us(1);
    NRF_WriteReg(NRF_WRITE_REG + RF_CH, CHANAL);  //设置信道
    delay_us(1);
    NRF_WriteReg(NRF_WRITE_REG + RX_PW_P0, TX_PLOAD_WIDTH);  //配置接收字节数
    delay_us(1);
    NRF_WriteReg(NRF_WRITE_REG + RF_SETUP, RF_Config);  //发射配置
    delay_us(1);
    NRF_WriteReg(NRF_WRITE_REG + CONFIG, 0x0f);  //寄存器配置
    delay_us(1);
    NRF_WriteReg(NRF_WRITE_REG + STATUS, 0xff);  //将所有状态寄存器清零
    delay_us(1);
    NRF_WriteReg(FLUSH_TX, NOP);
    delay_us(1);
    NRF_WriteReg(FLUSH_RX, NOP);
    delay_us(1);
    CE_1;
    delay_us(10);  //等待其稳定
}

void NRF_TXMode(void) {
    CE_0;
    delay_us(1);
	
    NRF_WriteBuff(NRF_WRITE_REG + TX_ADDR, ADDR, TX_ADR_WIDTH);  //发射方的发射地址
    delay_us(1);
    NRF_WriteBuff(NRF_WRITE_REG + RX_ADDR_P0, ADDR, TX_ADR_WIDTH);  //为了接收ACK
    delay_us(1);
    NRF_WriteReg(NRF_WRITE_REG + EN_AA, 0x01);  //  使能数据管道0自动确认（应答）
	//NRF_WriteReg(NRF_WRITE_REG + EN_AA, 0x00);  //  使能数据管道0自动确认（应答）
    delay_us(1);
    NRF_WriteReg(NRF_WRITE_REG + EN_RXADDR, 0x01);  //使能接受数据管道0地址
	//NRF_WriteReg(NRF_WRITE_REG + EN_RXADDR, 0x00);  //使能接受数据管道0地址
    delay_us(1);
    NRF_WriteReg(NRF_WRITE_REG + SETUP_RETR, 0x2a);  //自动重发配置
	//NRF_WriteReg(NRF_WRITE_REG + SETUP_RETR, 0x00);  //自动重发配置
    delay_us(1);
    NRF_WriteReg(NRF_WRITE_REG + RF_CH, CHANAL);  //设置信道
    delay_us(1);
    NRF_WriteReg(NRF_WRITE_REG + RF_SETUP, RF_Config);  //发射配置
    delay_us(1);
    NRF_WriteReg(NRF_WRITE_REG + CONFIG, 0x0e);  //寄存器配置
    delay_us(1);
    NRF_WriteReg(FLUSH_RX, NOP);
    delay_us(1);
    NRF_WriteReg(FLUSH_TX, NOP);
    delay_us(1);
    CE_1;
    delay_us(10);  //等待其稳定
}

unsigned char NRF_RXPacket(unsigned char *rxbuf) {
    uint8_t status;
    status = NRF_ReadReg(STATUS);
    delay_us(1);
    NRF_WriteReg(NRF_WRITE_REG + STATUS, status);  //清除中断标志位
    delay_us(1);
    if (status & RX_DR) {
        NRF_ReadBuff(RD_RX_PLOAD, rxbuf, TX_PLOAD_WIDTH);
        delay_us(1);
        NRF_WriteReg(FLUSH_RX, NOP);
        return 1;
    }
    return 0;
}

unsigned char NRF_TXPacket(unsigned char *txbuf) {
    unsigned char status;
    CE_0;
    delay_us(1);
    NRF_WriteBuff(WR_TX_PLOAD, txbuf, TX_PLOAD_WIDTH);
    delay_us(1);
    CE_1;
    while (IRQ() == SET);
    delay_us(1);
    status = NRF_ReadReg(STATUS);
    delay_us(1);
    NRF_WriteReg(NRF_WRITE_REG + STATUS, status);  //清除最大重发次数标志位
    delay_us(1);
    NRF_WriteReg(FLUSH_TX, NOP);
    if (status & MAX_RT) {
        NRF_WriteReg(FLUSH_TX, 0XFF);
        return MAX_RT;
    }
    if (status & TX_DS) {
        return TX_DS;  //成功
    }
	printf("%d\r\n",status);
    return 0xFF;
}
unsigned char NRF_Check(void) {
    unsigned char Check1[5] = { 0x01, 0x11, 0x61, 0x71, 0x02 };
    unsigned char Check2[5] = { 0 };
    unsigned char i;

    CSN_0;
    NRF_WriteBuff(NRF_WRITE_REG + RX_ADDR_P0, Check1, 5);  //写入发射方地址
    delay_us(2);
    NRF_ReadBuff(NRF_READ_REG + RX_ADDR_P0, Check2, 5);

    for (i = 0; i < 5; i++) {
        if (Check2[i] != Check1[i]) {
            CSN_1;
            return SET;
        }
    }
    CSN_1;
    return RESET;  //成功
}
