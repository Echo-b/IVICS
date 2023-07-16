#include "24l01.h"
#include "CrossingInfomation.h"
#include "Head.h"
#include "Message.h"
#include "VehicleInfomation.h"
#include "app_motor.h"
#include "app_ultrasonic.h"
#include "bsp.h"
#include "delay.h"
#include "stm32f10x.h" // Device header
#include "string.h"
#include "usart.h"

u8 text[32];
u8 cnt = 0;
u8 state = 1; // ����״̬
u8 flag = 1;
u16 speed = 700;
VehicleInfomation vehicleInfo;
CrossingInfomation crossingInfo;
u8 message[32];
Head head;
u8 nodeId = 11;
int hasObstacle = 0;

void nrf()
{
    // ����ģʽ
    if (state)
    {
        // ������﷢��ʱ��
        if (flag)
        {
            // ��װ��Ϣ
            head.nodeId = nodeId;
            head.msgType = VEHICLEINFOMESSAGE;
            vehicleInfo.speed = speed / 100;
            vehicleInfo.id = 2;
            vehicleInfo.type = CAR;
            getVehicleMessage(message, &head, &vehicleInfo);
            // printf("Tx VehicleInfo Ok\r\n");
            // ������Ϣ
            if (NRF24L01_TxPacket(message) == TX_OK)
            {
                // �޸�״̬
                printf("Vehicle Tx:nodeId%d,id%d,type%d,speed%d\r\n", nodeId, vehicleInfo.id, vehicleInfo.type, vehicleInfo.speed);
                state = 0;
                NRF24L01_RX_Mode();
                // ����ʱ��
                flag = 0;
                printf("Ready Rx\r\n");
            }
        }
    }
    else
    {
        if (NRF24L01_RxPacket(message) == 0)
        {
            // ������Ϣ
            analysisMessageHead(message, &head);
            if (nodeId == head.nodeId && head.msgType == CORSSINGINFIOMESSAGE)
            {
                analysisStausMessage(message, &crossingInfo);
                printf("Vehicle Rx:nodeid%d,staus%d\r\n", head.nodeId, crossingInfo.staus);
            }

            // ��Ϊ����ģʽ
            state = 1;
            NRF24L01_TX_Mode();
            printf("Ready Tx\r\n");
        }
    }
}

void TIM3_IRQHandler(void)
{
    if (TIM_GetITStatus(TIM3, TIM_IT_Update) != RESET)
    {
        TIM_ClearITPendingBit(TIM3, TIM_IT_Update); // ����жϱ�־
        flag = 1;
    }
}

void Timer3_Init(void)
{
    /*��ʼ��ʱ����Ԫ�ṹ��*/
    TIM_TimeBaseInitTypeDef TIM_TimeBaseInitStructer;
    /*��ʼ��NVIC�ṹ��*/
    NVIC_InitTypeDef NVIC_InitStructer;

    /*����TIM3��ʱ��*/
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3, ENABLE);

    /*��ʱ��TIM3��ʼ��*/
    TIM_DeInit(TIM3);
    TIM_TimeBaseInitStructer.TIM_Period = 9999;                // ��ʱ����Ϊ1000
    TIM_TimeBaseInitStructer.TIM_Prescaler = 7199 * 2;         // ��Ƶϵ��72
    TIM_TimeBaseInitStructer.TIM_ClockDivision = TIM_CKD_DIV1; // ����Ƶ
    TIM_TimeBaseInitStructer.TIM_CounterMode = TIM_CounterMode_Up;
    TIM_TimeBaseInitStructer.TIM_RepetitionCounter = 0; // �߼���ʱ����ʹ��
    TIM_TimeBaseInit(TIM3, &TIM_TimeBaseInitStructer);

    /*�����жϣ�����ʱ�����ж�*/
    TIM_ITConfig(TIM3, TIM_IT_Update, ENABLE);

    /*��ʱ���жϳ�ʼ��*/
    // NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);

    NVIC_InitStructer.NVIC_IRQChannelPreemptionPriority = 0;
    NVIC_InitStructer.NVIC_IRQChannelSubPriority = 0;
    NVIC_InitStructer.NVIC_IRQChannel = TIM3_IRQn;
    NVIC_InitStructer.NVIC_IRQChannelCmd = ENABLE;

    NVIC_Init(&NVIC_InitStructer);
    TIM_Cmd(TIM3, ENABLE); // ������ʱ��ʹ��
}

int main(void)
{

    NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2); // �����ж����ȼ�����Ϊ��2��2λ��ռ���ȼ���2λ��Ӧ���ȼ�

    delay_init();
    uart_init(115200);
    NRF24L01_Init();
    Timer3_Init();
    bsp_init();
    bsp_Ultrasonic_Timer2_Init();
    crossingInfo.staus = CLOSE_INIT;

    if (NRF24L01_Check() == 0)
    {
        printf("Init Yes!\r\n");
    }

    NRF24L01_TX_Mode();
    printf("Ready Tx\r\n");

    while (1)
    {
        nrf();
        hasObstacle = app_ultrasonic_mode();
        if (hasObstacle)
        {
            switch (crossingInfo.staus)
            {
            case OPEN:
            {
                speed = 700;
                Car_Run(speed);

                break;
            }
            case CLOSE_TEAIN:
            {
                speed = 500;
                Car_Run(speed);

                break;
            }
            case CLOSE_MALFUNCTION:
            {
                Car_Stop();
                speed = 0;
                break;
            }
            case CLOSE_OBSTACLE:
            {
                Car_Stop();
                speed = 0;
                break;
            }
            case CLOSE_INIT:
            {
                Car_Stop();
                speed = 0;
                break;
            }
            case CLOSE_TYPE:
            {
                Car_Stop();
                speed = 0;
                break;
            }
            default:
                break;
            }
        }
        else
        {
            Car_Stop();
            speed = 0;
        }

        delay_ms(50);
    }
}
