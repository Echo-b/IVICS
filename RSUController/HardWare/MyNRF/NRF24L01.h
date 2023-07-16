#ifndef __NRF24L01_H
#define __NRF24L01_H

#define TX_ADR_WIDTH   5  				// 5×Ö½Ú¿í¶ÈµÄ·¢ËÍ/½ÓÊÕµØÖ·
#define TX_PLOAD_WIDTH 32  				// Êý¾ÝÍ¨µÀÓÐÐ§Êý¾Ý¿í¶È

//********************************************************************************************************************//
// SPI(SI24R1) 命令
#define NRF_READ_REG        0x00  // 读寄存器命令
#define NRF_WRITE_REG       0x20  // 写寄存器命令

#define RD_RX_PLOAD     0x61  // 从FIFO中读收到的数据，1-32字节，读出后FIFO数据被删除，适用于接收模式
#define WR_TX_PLOAD     0xA0  // 写发射负载数据，大小为1-32字节，适用于发射模式

#define FLUSH_TX        0xE1  // 清空TX FIFO，适用于发射模式
#define FLUSH_RX        0xE2  // 清空RX FIFO，适用于接收模式。如果需要回ACK，则不能在回ACK操作完成前进行清空FIFO，
                                  //否则视为通信失败
#define REUSE_TX_PL     0xE3  // 适用于发送方，重新使用之前地数据发送，但清空TX FIFO或对FIFO写入新的数据后不能使用该命令
#define NOP             0xFF  //无操作 ，可用于返回STATUS值

//********************************************************************************************************************//
// SPI(SI24R1) registers(addresses)
#define CONFIG          0x00  // 配置寄存器
#define EN_AA           0x01  // 使能自动确认
#define EN_RXADDR       0x02  // 使能接收数据管道地址
#define SETUP_AW        0x03  // 地址宽度配置
#define SETUP_RETR      0x04  // 自动重发配置
#define RF_CH           0x05  // 射频信道 
#define RF_SETUP        0x06  // 射频配置
#define STATUS          0x07  // 状态寄存器(SPI操作开始，状态寄存器值通过MISO串行输出)
#define OBSERVE_TX      0x08  // 发射结果统计
#define RSSI            0x09  // 接收信号强度检测

#define RX_ADDR_P0      0x0A  //数据管道0的接收地址，最大宽度为5bytes (LSByte最先写入，通过SETUP_AW配置地址宽度)
#define RX_ADDR_P1      0x0B  // 'RX address pipe1' register address
#define RX_ADDR_P2      0x0C  // 'RX address pipe2' register address
#define RX_ADDR_P3      0x0D  // 'RX address pipe3' register address
#define RX_ADDR_P4      0x0E  // 'RX address pipe4' register address
#define RX_ADDR_P5      0x0F  // 'RX address pipe5' register address

#define TX_ADDR         0x10  // 发射方的发射地址(LSByte最先写入)，如果发射放需要收ACK确认信号，
                              // 则需要配置RX_ADDR_P0的值等于TX_ADDR，并使能ARQ。

#define RX_PW_P0        0x11  // 接收数据管道0数据字节数
#define RX_PW_P1        0x12  // 'RX payload width, pipe1' register address
#define RX_PW_P2        0x13  // 'RX payload width, pipe2' register address
#define RX_PW_P3        0x14  // 'RX payload width, pipe3' register address
#define RX_PW_P4        0x15  // 'RX payload width, pipe4' register address
#define RX_PW_P5        0x16  // 'RX payload width, pipe5' register address

#define FIFO_STATUS     0x17  // FIFO状态

//********************************************************************************************************************//
// 状态寄存器
#define RX_DR						0x40  //RX FIFO有值标志位，写’1’清除
#define TX_DS						0x20  //发射端发射完成中断位,写‘1’清楚
#define MAX_RT					    0x10  //达到最大重发次数中断位，写’1’清除

/***********************************************************************************************************************/
void NRF_Init(void);
unsigned char NRF_WriteReg(unsigned char reg,unsigned char value);
unsigned char NRF_WriteBuff(unsigned char reg,unsigned char* buf ,unsigned char Length);
unsigned char NRF_ReadReg(unsigned char reg);
unsigned char NRF_ReadBuff(unsigned char reg,unsigned char* buf,unsigned char Length);

void NRF_RXMode(void);
void NRF_TXMode(void);
unsigned char NRF_RXPacket(unsigned char * rxbuf);
unsigned char NRF_TXPacket(unsigned char * txbuf);

unsigned char NRF_Check(void);

#endif
