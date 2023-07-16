#include "sys.h"
#include "usart.h"
#if SYSTEM_SUPPORT_UCOS
#include "includes.h" //ucos ʹ��
#endif

u8 data1, data2;
u8 USART_RX_STACOMPLETE = 0;
#if 1
#pragma import(__use_no_semihosting)
// ��׼����Ҫ��֧�ֺ���
struct __FILE
{
	int handle;
};

FILE __stdout;
// ����_sys_exit()�Ա���ʹ�ð�����ģʽ
void _sys_exit(int x)
{
	x = x;
}
// �ض���fputc����
int fputc(int ch, FILE *f)
{
	while ((USART1->SR & 0X40) == 0)
		; // ѭ������,ֱ���������
	USART1->DR = (u8)ch;
	return ch;
}
#endif

#if EN_USART1_RX // ���ʹ���˽���
// ����1�жϷ������
// ע��,��ȡUSARTx->SR�ܱ���Ī������Ĵ���
u8 USART_RX_BUF[USART_REC_LEN]; // ���ջ���,���USART_REC_LEN���ֽ�.
// ����״̬
// bit15��	������ɱ�־
// bit14��	���յ�0x0d
// bit13~0��	���յ�����Ч�ֽ���Ŀ
u16 USART_RX_STA = 0; // ����״̬���

void uart_init(u32 bound)
{
	// GPIO�˿�����
	GPIO_InitTypeDef GPIO_InitStructure;
	USART_InitTypeDef USART_InitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;

	RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1 | RCC_APB2Periph_GPIOA, ENABLE); // ʹ��USART1��GPIOAʱ��
																				  // USART1_TX   PA.9
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9; // PA.9
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP; // �����������
	GPIO_Init(GPIOA, &GPIO_InitStructure);

	// USART1_RX	  PA.10
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING; // ��������
	GPIO_Init(GPIOA, &GPIO_InitStructure);

	// Usart1 NVIC ����

	NVIC_InitStructure.NVIC_IRQChannel = USART1_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 3; // ��ռ���ȼ�3
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 3;		  // �����ȼ�3
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;			  // IRQͨ��ʹ��
	NVIC_Init(&NVIC_InitStructure);							  // ����ָ���Ĳ�����ʼ��VIC�Ĵ���

	// USART ��ʼ������

	USART_InitStructure.USART_BaudRate = bound;										// һ������Ϊ9600;
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;						// �ֳ�Ϊ8λ���ݸ�ʽ
	USART_InitStructure.USART_StopBits = USART_StopBits_1;							// һ��ֹͣλ
	USART_InitStructure.USART_Parity = USART_Parity_No;								// ����żУ��λ
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None; // ��Ӳ������������
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;					// �շ�ģʽ

	USART_Init(USART1, &USART_InitStructure);	   // ��ʼ������
	USART_ITConfig(USART1, USART_IT_RXNE, ENABLE); // �����ж�
	USART_Cmd(USART1, ENABLE);					   // ʹ�ܴ���
}
void uart3_init(u32 bound)
{
	// GPIO�˿�����
	GPIO_InitTypeDef GPIO_InitStructure;
	USART_InitTypeDef USART_InitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;

	RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART3 | RCC_APB2Periph_GPIOB, ENABLE); // ʹ��USART1��GPIOAʱ��
																				  // USART1_TX   PB10
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP; // �����������
	GPIO_Init(GPIOB, &GPIO_InitStructure);

	// USART1_RX	  PB.11
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_11;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING; // ��������
	GPIO_Init(GPIOB, &GPIO_InitStructure);

	// Usart1 NVIC ����

	NVIC_InitStructure.NVIC_IRQChannel = USART3_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 3; // ��ռ���ȼ�3
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 3;		  // �����ȼ�3
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;			  // IRQͨ��ʹ��
	NVIC_Init(&NVIC_InitStructure);							  // ����ָ���Ĳ�����ʼ��VIC�Ĵ���

	// USART ��ʼ������

	USART_InitStructure.USART_BaudRate = bound;										// һ������Ϊ9600;
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;						// �ֳ�Ϊ8λ���ݸ�ʽ
	USART_InitStructure.USART_StopBits = USART_StopBits_1;							// һ��ֹͣλ
	USART_InitStructure.USART_Parity = USART_Parity_No;								// ����żУ��λ
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None; // ��Ӳ������������
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;					// �շ�ģʽ

	USART_Init(USART3, &USART_InitStructure);	   // ��ʼ������
	USART_ITConfig(USART3, USART_IT_RXNE, ENABLE); // �����ж�
	USART_Cmd(USART3, ENABLE);					   // ʹ�ܴ���
}

void Serila_SendData(u8 byte)
{
	USART_SendData(USART1, byte);
	while (USART_GetFlagStatus(USART1, USART_FLAG_TXE) == RESET)
		;
}
void Serila_SendData3(u8 byte)
{

	USART_SendData(USART3, byte);
	while (USART_GetFlagStatus(USART3, USART_FLAG_TXE) == RESET)
		;
}
void Seiral_SendMessage3(u8 *message, u8 len)
{
	u8 i;
	for (i = 0; i < len; i++)
	{
		Serila_SendData3(message[i]);
	}
}

void Seiral_SendMessage(u8 *message, u8 len)
{
	u8 i;
	for (i = 0; i < len; i++)
	{
		Serila_SendData(message[i]);
	}
}

void USART1_IRQHandler(void) // ����1�жϷ������
{
	static u8 RxState = 0;
	u8 Res;
	if (USART_GetITStatus(USART1, USART_IT_RXNE) == SET)
	{
		Res = USART_ReceiveData(USART1); // ��ȡ���յ�������
		if (RxState == 0)
		{
			if (Res == 0xFF)
			{
				USART_RX_STA = 0;
				RxState = 1;
			}
		}
		else if (RxState == 1)
		{
			if (Res == 0XFE)
			{
				RxState = 0;
				USART_RX_STACOMPLETE = 1;
			}
			else
			{
				USART_RX_BUF[USART_RX_STA] = Res;
				USART_RX_STA++;
			}
		}

		USART_ClearITPendingBit(USART1, USART_IT_RXNE); // ����жϱ�־λ
	}
}
#endif
