#ifndef __MESSAGE_H
#define __MESSAGE_H

#include "CrossingInfomation.h"
#include "Head.h"
#include "VehicleInfomation.h"
#include "stm32f10x.h"  // Device header

#define VEHICLEINFOMESSAGE 11
#define CORSSINGINFIOMESSAGE 12
#define CONTROLMESSAGE 13

//组装车辆信息
void getVehicleMessage(u8 *message, Head *head, VehicleInfomation *payload);
//分析接收到的状态信息
void analysisStausMessage(u8 *message,CrossingInfomation  *payload);
//分析消息头
void analysisMessageHead(u8 *message, Head *head);


#endif
