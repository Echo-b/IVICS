#include "stm32f10x.h" // Device header
#include "Delay.h"
#include "L298N.h"
#include "usart.h"
#include "exti.h"
#include "Key.h"
uint8_t RxData;
uint16_t speed;
int main(void)
{
	L298N_Init();
	uart_init(115200);
	Key_Init();
	while (1)
	{

		if (Key_GetNum() == 1)
			L298N_BACK(15000);
		else
		{
			if (USART_RX_STACOMPLETE)
			{
				RxData = USART_RX_BUF[0];
				printf("%d", RxData);
				USART_RX_STACOMPLETE = 0;
			}
			switch (RxData)
			{
			case 0x01:
				speed = 0;
				break;
			case 0x02:
				speed = 5500;
				break;
			case 0x03:
				speed = 6500;
				break;
			case 0x04:
				speed = 7500;
				break;
			case 0x05:
				speed = 8500;
				break;
			default:
				break;
			}
			Delay_ms(50);
			L298N_RUN(speed);
		}
	}
}
