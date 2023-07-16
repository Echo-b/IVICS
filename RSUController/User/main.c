#include "24l01.h"
#include "CorssingInformation.h"
#include "Head.h"
#include "Message.h"
#include "VehicleInformation.h"
#include "delay.h"
#include "key.h"
#include "lcd.h"
#include "led.h"
#include "sys.h"
#include "usart.h"

u8 state = 0; // 接受状态
u8 message[32];
Head messageHead;
VehicleInfomation vehicleInfo_temp;
CrossingInfomation crossInfo_temp;
u8 currentNode;
u8 currentAddress[5];
u8 flag = 0;

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
    TIM_TimeBaseInitStructer.TIM_Prescaler = 7200;             // 分频系数72
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

void TIM3_IRQHandler(void)
{
    if (TIM_GetITStatus(TIM3, TIM_IT_Update) != RESET)
    {
        TIM_ClearITPendingBit(TIM3, TIM_IT_Update); // 清除中断标志
        flag = 1;
    }
}

void showUpdateInfo(void)
{
    u16 i;
    u8 count = 0;

    // 向液晶显示屏上写入字符串和对应的值
    LCD_Clear(WHITE);
    LCD_ShowString(10, 10, 50, 16, 16, "Status:");
    LCD_ShowNum(60, 10, crossInfo_temp.staus, 2, 16);
    LCD_ShowString(110, 10, 70, 16, 16, "LedStatus:");
    LCD_ShowNum(180, 10, crossInfo_temp.ledStaus, 3, 16);
    LCD_ShowString(110, 30, 70, 16, 16, "Obstacle:");
    LCD_ShowNum(180, 30, crossInfo_temp.obstacle, 3, 16);
    LCD_ShowString(110, 50, 70, 16, 16, "barrierStatus:");
    LCD_ShowNum(180, 50, crossInfo_temp.barrierStatus, 3, 16);
    LCD_ShowString(10, 70, 110, 16, 16, "VehicleNumber:");
    LCD_ShowNum(120, 70, VehicleInfos[0].speed, 3, 16);
    LCD_ShowString(10, 90, 70, 16, 16, "ID");
    LCD_ShowString(80, 90, 70, 16, 16, "Type");
    LCD_ShowString(150, 90, 70, 16, 16, "Speed");

    for (i = 1; i < 256; i++)
    {
        if (VehicleInfos[i].dispatched)
        {
            u16 temp = count * 20;
            count++;
            LCD_ShowNum(10, 110 + temp, VehicleInfos[i].id, 3, 16);
            LCD_ShowNum(80, 110 + temp, VehicleInfos[i].type, 3, 16);
            LCD_ShowNum(150, 110 + temp, VehicleInfos[i].speed, 3, 16);
        }
    }
}

void checkSerial()
{
    if (USART_RX_STACOMPLETE)
    {
        analysisMessageHead(USART_RX_BUF, &messageHead);
        if (messageHead.msgType == CONTROLMESSAGE)
        {
            // 设置道口的状态
            switch (messageHead.nodeId)
            {
            case MESSAGE_TRAIN:
            {
                analysisControlMessageForOne(USART_RX_BUF, &crossInfo_temp);
                break;
            }
            case MESSAGE_CONSOLE:
            {
                analysisControlMessageForTwo(USART_RX_BUF, &crossInfo_temp);
                break;
            }
            case MESSAGE_CAMERE:
            {
                analysisControlMessageForThree(USART_RX_BUF, &crossInfo_temp);
                break;
            }
            default:
                break;
            }
        }
        showUpdateInfo();
        USART_RX_STACOMPLETE = 0;
    }
}

void nrf()
{
    // 发送模式
    if (state)
    {
        // 路口状态信息
        messageHead.msgType = CORSSINGINFIOMESSAGE;
        // 发送目标的节点号
        messageHead.nodeId = currentNode;
        // 组装信息

        // 不允许该类型车辆通行
        if (vehicleInfo_temp.type == TRUCK)
        {
            CrossingInfomation typeClose;
            typeClose.staus = CLOSE_TYPE;
            getStausMessageToVechicle(message, &messageHead, &typeClose);
        }
        // 其他消息
        else
        {
            getStausMessageToVechicle(message, &messageHead, &crossInfo_temp);
        }

        if (NRF24L01_TxPacket(message) == TX_OK)
        {
            // 打印发送的消息
            printf("RSU Tx:nodeid%d,staus%d\r\n", messageHead.nodeId, crossInfo_temp.staus);
            // 切换为接收模式
            state = 0;
            NRF24L01_RX_Mode();
            printf("Ready Rx\r\n");
        }
    }
    // 接受模式
    else
    {
        if (NRF24L01_RxPacket(message) == 0)
        {
            // 分析消息首部
            analysisMessageHead(message, &messageHead);
            // 如果是车辆信息
            // 记录下当前车辆的节点号
            currentNode = messageHead.nodeId;
            analysisVehicleMessage(message, &vehicleInfo_temp);
            // 更新储存的车辆信息
            updateVehicleInfos(message);
            // showUpdateInfo();

            printf("RSU Rx:nodeId%d,id%d,type%d,speed%d\r\n", messageHead.nodeId, vehicleInfo_temp.id, vehicleInfo_temp.type, vehicleInfo_temp.speed);

            // 切换发送模式
            state = 1;
            NRF24L01_TX_Mode();
            printf("Ready Tx\r\n");
        }
    }
}

int main(void)
{

    // 显示屏颜色
    POINT_COLOR = RED;

    // 初始化
    KEY_Init();
    delay_init();
    NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);
    uart_init(115200);
    uart2_init(9600);
    LCD_Init();
    crossInfo_temp.staus = OPEN;
    NRF24L01_Init();

    initVehicleInfos();

    // 检测24l01是否存在
    //    if (NRF24L01_Check() == 0) {
    //        LCD_Clear(WHITE);
    //        LCD_ShowString(30, 50, 200, 16, 16, "nRF24L01 Init Yes!");
    //    }

    NRF24L01_RX_Mode();
    showUpdateInfo();
    printf("Ready Rx\r\n");
    Timer3_Init();
    while (1)
    {
        checkSerial();
        nrf();
        showUpdateInfo();
        if (flag == 1)
        {
            u8 message_temp[32];
            u8 len = getMessageToServer(message_temp, &crossInfo_temp, VehicleInfos);
            sendDataToSerial2(message_temp, len);
            flag = 0;
        }
        delay_ms(100);
    }
}
