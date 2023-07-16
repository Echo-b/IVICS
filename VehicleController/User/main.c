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
u8 state = 1; // 接受状态
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
    // 发送模式
    if (state)
    {
        // 如果到达发送时间
        if (flag)
        {
            // 组装消息
            head.nodeId = nodeId;
            head.msgType = VEHICLEINFOMESSAGE;
            vehicleInfo.speed = speed / 100;
            vehicleInfo.id = 2;
            vehicleInfo.type = CAR;
            getVehicleMessage(message, &head, &vehicleInfo);
            // printf("Tx VehicleInfo Ok\r\n");
            // 发送信息
            if (NRF24L01_TxPacket(message) == TX_OK)
            {
                // 修改状态
                printf("Vehicle Tx:nodeId%d,id%d,type%d,speed%d\r\n", nodeId, vehicleInfo.id, vehicleInfo.type, vehicleInfo.speed);
                state = 0;
                NRF24L01_RX_Mode();
                // 重置时间
                flag = 0;
                printf("Ready Rx\r\n");
            }
        }
    }
    else
    {
        if (NRF24L01_RxPacket(message) == 0)
        {
            // 解析信息
            analysisMessageHead(message, &head);
            if (nodeId == head.nodeId && head.msgType == CORSSINGINFIOMESSAGE)
            {
                analysisStausMessage(message, &crossingInfo);
                printf("Vehicle Rx:nodeid%d,staus%d\r\n", head.nodeId, crossingInfo.staus);
            }

            // 变为发送模式
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
        TIM_ClearITPendingBit(TIM3, TIM_IT_Update); // 清除中断标志
        flag = 1;
    }
}

void Timer3_Init(void)
{
    /*初始化时基单元结构体*/
    TIM_TimeBaseInitTypeDef TIM_TimeBaseInitStructer;
    /*初始化NVIC结构体*/
    NVIC_InitTypeDef NVIC_InitStructer;

    /*开启TIM3的时钟*/
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3, ENABLE);

    /*定时器TIM3初始化*/
    TIM_DeInit(TIM3);
    TIM_TimeBaseInitStructer.TIM_Period = 9999;                // 定时周期为1000
    TIM_TimeBaseInitStructer.TIM_Prescaler = 7199 * 2;         // 分频系数72
    TIM_TimeBaseInitStructer.TIM_ClockDivision = TIM_CKD_DIV1; // 不分频
    TIM_TimeBaseInitStructer.TIM_CounterMode = TIM_CounterMode_Up;
    TIM_TimeBaseInitStructer.TIM_RepetitionCounter = 0; // 高级定时器才使用
    TIM_TimeBaseInit(TIM3, &TIM_TimeBaseInitStructer);

    /*开启中断，更新时触发中断*/
    TIM_ITConfig(TIM3, TIM_IT_Update, ENABLE);

    /*定时器中断初始化*/
    // NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);

    NVIC_InitStructer.NVIC_IRQChannelPreemptionPriority = 0;
    NVIC_InitStructer.NVIC_IRQChannelSubPriority = 0;
    NVIC_InitStructer.NVIC_IRQChannel = TIM3_IRQn;
    NVIC_InitStructer.NVIC_IRQChannelCmd = ENABLE;

    NVIC_Init(&NVIC_InitStructer);
    TIM_Cmd(TIM3, ENABLE); // 开启定时器使能
}

int main(void)
{

    NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2); // 设置中断优先级分组为组2：2位抢占优先级，2位响应优先级

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
