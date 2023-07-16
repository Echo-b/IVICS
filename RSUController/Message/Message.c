#include "Message.h"
#include "CorssingInformation.h"

void getStausMessageToVechicle(u8 *message, Head *head, CrossingInfomation *payload)
{
	message[0] = head->nodeId;
	message[1] = head->msgType;
	message[2] = payload->staus;
	message[3] = payload->address;
	// message[4] = VehicleInfos[0].speed;
}

/**
 * @brief Get the Message To Server object
 * 路侧单元主控板向联网模块发送的道口交通信息，消息格式如下
 *
 * +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 * + 0xff + 道口开放状态 + 警示灯信号 + 防护栏状态 + 道口内车的数量+
 * +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 * 车辆id + 车辆种类 + 车辆速度 + （若干车）+ 0xfe                +
 * +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 *
 * @param message
 * @param payload
 * @param vehicleInfo
 * @return u8
 */
u8 getMessageToServer(u8 *message, CrossingInfomation *payload, VehicleInfomation *vehicleInfo)
{
	u16 i = 0;
	u16 count = 0;
	u16 temp = 2;
	message[0] = 0xff;
	message[1] = payload->staus;
	message[2] = payload->ledStaus;
	message[3] = payload->barrierStatus;
	message[4] = vehicleInfo[0].speed;
	for (i = 1; i < 256; i++)
	{
		if (vehicleInfo[i].dispatched)
		{
			temp = count * 3 + 5; // 跳过协议头，通信节点id等信息
			count++;
			message[temp] = vehicleInfo[i].id;
			message[temp + 1] = vehicleInfo[i].type;
			message[temp + 2] = vehicleInfo[i].speed;
		}
	}
	message[temp + 3] = 0xfe;
	return temp + 4;

	// message[3] = payload->distance;
	// message[4] = VehicleInfos[0].speed;
}

void analysisVehicleMessage(u8 *message, VehicleInfomation *payload)
{
	payload->id = message[2];
	payload->type = message[3];
	payload->speed = message[4];
}

void analysisControlMessageForOne(u8 *message, CrossingInfomation *payload)
{
	payload->staus = message[2];
	if (payload->staus == OPEN)
	{
		payload->ledStaus = 0;
		payload->barrierStatus = 0;
	}
	else if (payload->staus == CLOSE_TEAIN)
	{
		payload->ledStaus = 1;
		payload->barrierStatus = 1;
	}
}

/**
 * @brief 道口故障维修，道口关闭
 *
 * @param message
 * @param payload
 */
void analysisControlMessageForTwo(u8 *message, CrossingInfomation *payload)
{
	payload->staus = message[2];
	if (payload->staus == CLOSE_MALFUNCTION)
	{
		payload->ledStaus = 1;
		payload->barrierStatus = 1;
	}
}

void analysisControlMessageForThree(u8 *message, CrossingInfomation *payload)
{
	if (message[2] != 0)
	{
		payload->staus = message[2];
	}
	if (payload->staus == CLOSE_OBSTACLE)
	{
		payload->ledStaus = 1;
		payload->barrierStatus = 1;
	}
	else if (payload->staus == CLOSE_MALFUNCTION)
	{
		payload->barrierStatus = !payload->barrierStatus;
		payload->ledStaus = 1;
	}
	payload->obstacle = message[4];
}

void analysisMessageHead(u8 *message, Head *head)
{
	head->nodeId = message[0];
	head->msgType = message[1];
}
