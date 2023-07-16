#include "stm32f10x.h"
#include "stdio.h"
#include "math.h"
#include "stdlib.h"
#include "string.h"
#include "usart.h"
#include "delay.h"
#include "sys.h"
#include "vl53l0.h"
#include "iwdg.h"
#include "timer.h"
#include "stm32_i2c.h" // Device header
#include "Delay.h"
#include "L298N.h"
#include "Servo.h"
#include "LED.h"
#include "Key.h"

void SendData();
void SendKeyData();
void SendTrainData();
void SendCamraData();
u8 a[5];
u8 camra[7];
u8 flag = 1;
u8 twinkle_flag = 0; // 闪烁标志位
u8 run[3] = {0xff, 0x00, 0xfe};
u8 flag_langan = 1; // 1-升起 0-落下
int x = 0;
int y = 0;
void Timer4_Init(void)
{

	a[0] = 0xff;
	a[3] = 1;
	a[4] = 0xfe;

	camra[0] = 0xff;
	camra[1] = 0x03;
	camra[2] = 0x0d;
	camra[6] = 0xfe;
	/*初始化时基单元结构体*/
	TIM_TimeBaseInitTypeDef TIM_TimeBaseInitStructer;
	/*初始化NVIC结构体*/
	NVIC_InitTypeDef NVIC_InitStructer;

	/*开启TIM3的时钟*/
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM4, ENABLE);

	/*定时器TIM3初始化*/
	TIM_DeInit(TIM4);
	TIM_TimeBaseInitStructer.TIM_Period = 4999;				   // 定时周期为1000
	TIM_TimeBaseInitStructer.TIM_Prescaler = 7199;			   // 分频系数72
	TIM_TimeBaseInitStructer.TIM_ClockDivision = TIM_CKD_DIV1; // 不分频
	TIM_TimeBaseInitStructer.TIM_CounterMode = TIM_CounterMode_Up;
	TIM_TimeBaseInitStructer.TIM_RepetitionCounter = 0; // 高级定时器才使用
	TIM_TimeBaseInit(TIM4, &TIM_TimeBaseInitStructer);

	/*开启中断，更新时触发中断*/
	TIM_ITConfig(TIM4, TIM_IT_Update, ENABLE);

	/*定时器中断初始化*/
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);

	NVIC_InitStructer.NVIC_IRQChannelPreemptionPriority = 0;
	NVIC_InitStructer.NVIC_IRQChannelSubPriority = 0;
	NVIC_InitStructer.NVIC_IRQChannel = TIM4_IRQn;
	NVIC_InitStructer.NVIC_IRQChannelCmd = ENABLE;

	NVIC_Init(&NVIC_InitStructer);
	TIM_Cmd(TIM4, ENABLE); // 开启定时器使能
}
void TIM4_IRQHandler(void)
{
	if (TIM_GetITStatus(TIM4, TIM_IT_Update) != RESET)
	{
		TIM_ClearITPendingBit(TIM4, TIM_IT_Update); // 清除中断标志
		flag = 1;
	}
}

int main(void)
{

	L298N_Init();
	Servo_Init();
	Key_Init();
	Servo_SetAngle(0);
	flag_langan = 1; // 升起
	Led_Init();
	delay_init();
	NVIC_Configuration();
	uart_init(115200);
	uart3_init(115200);
	vl53l0x_init();
	Timer4_Init();

	run[1] = 0x04;

	while (1)
	{

		switch (Key_GetNum())
		{
		case 1:
		{
			do
			{

				Led_R(); // red
				Servo_SetAngle(90);
				flag_langan = 0; // 落下
				run[1] = 0x01;
				SendKeyData();
				SendTrainData();
			} while (Key_GetNum() != 2);

			if ((distancebuff[0]) > 100 || (distancebuff[1] > 100) || (distancebuff[2] < 100))
			{
				Led_G(); // red
				Servo_SetAngle(0);
			}
			run[1] = 0x04;
			a[3] = 1;

			break;
		}
		default:
			break;
		}

		vl53l0x_getdistance();
		SendCamraData(); // 发送图像识别数据
		if (flag)
		{
			SendData();

			flag = 0;
			// 检查是否需要红闪
			if (twinkle_flag)
			{
				Led_twinkle();
			}
		}

		if ((distancebuff[0] < 100))

		{
			Led_R(); // red
			Servo_SetAngle(90);
			flag_langan = 0;
			run[1] = 0x03;
		}

		else if (distancebuff[1] < 100)
		{

			twinkle_flag = 1;
		}

		else if (distancebuff[2] < 100)
		{
			twinkle_flag = 0;
			Led_G();
			Servo_SetAngle(0);
			flag_langan = 1;
			run[1] = 0x04;
		}
	}
}

/**
 * @brief 下面简要描述发送消息所封装的各个消息帧格式
 * 上位机向路侧单元发送的信息为控制信息，消息格式如下
 *
 * + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + +
 * + 0xff + 通信节点id + 消息类型 + 道口状态 + 防护栏状态（可选）+ 障碍物数量 + 0xfe  +
 * + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + +
 *
 *
 * 由车辆发送至路侧单元的信息主要为车辆运行状态信息，
 * 包含了该车辆的运行信息，其基本格式如下
 *
 * + + + + + + + + + + + + + + + + + + + +
 * + 协议头 + 通信节点id + 消息类型+       +
 * + + + + + + + + + + + + + + + + + + + +
 * + 协议负载 + 车辆id + 车辆种类 + 车辆速度+
 * + + + + + + + + + + + + + + + + + + + +
 *
 */

void SendData()
{

	a[1] = 1;
	a[2] = 13; // 0x0c 道口交通信息

	if (distancebuff[0] < 100)
	{
		a[3] = 2; // 列车到达
	}
	else if (distancebuff[2] < 100)
	{
		a[3] = 1; // 列车离开
	}

	Seiral_SendMessage(run, 3);
	Seiral_SendMessage3(a, 5);
}

void SendKeyData()
{
	a[1] = 2;
	a[2] = 0x0d; // 控制信息
	a[3] = 0x03;

	Seiral_SendMessage3(a, 5);
}
void SendTrainData()
{

	Seiral_SendMessage(run, 3);
}

void SendCamraData()
{

	u8 rxdata[2];
	if (USART_RX_STACOMPLETE)
	{
		rxdata[0] = USART_RX_BUF[0];
		rxdata[1] = USART_RX_BUF[1];
		USART_RX_STACOMPLETE = 0;
	}
	if (((rxdata[0] == 0x01) && (flag_langan == 0)) || ((rxdata[0] == 0x02) && (flag_langan == 1))) // 栏杆异常
	{
		camra[3] = 0x03;
		camra[4] = 0x01;
		camra[5] = 0x00;
		Seiral_SendMessage3(camra, 6);
		run[1] = 0x01;
		Led_R(); // red
		y = 1;
	}
	else if (rxdata[1] == 0x01) // 障碍物
	{
		camra[3] = 0x04;
		camra[4] = 0x00;
		camra[5] = 0x01;

		Led_R(); // red
		Servo_SetAngle(90);
		flag_langan = 0;
		run[1] = 0x01;
		Seiral_SendMessage3(camra, 6);
		x = 1;
	}

	if (x)
	{
		if (rxdata[1] == 0x00)
		{
			if ((distancebuff[0]) > 100 || (distancebuff[1] > 100) || (distancebuff[2] < 100))
			{
				Led_G(); //
				Servo_SetAngle(0);
				flag_langan = 1;
				run[1] = 0x04;
			}
			x = 0;
		}
	}

	if (y)
	{
		if (((rxdata[0] == 0x01) && (flag_langan == 1)) || ((rxdata[0] == 0x02) && (flag_langan == 0)))
		{
			if ((distancebuff[0]) > 100 || (distancebuff[1] > 100) || (distancebuff[2] < 100))
			{
				Led_G(); //

				run[1] = 0x04;
			}
			y = 0;
		}
	}
}
