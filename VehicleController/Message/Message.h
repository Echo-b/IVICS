#ifndef __MESSAGE_H
#define __MESSAGE_H

#include "CrossingInfomation.h"
#include "Head.h"
#include "VehicleInfomation.h"
#include "stm32f10x.h"  // Device header

#define VEHICLEINFOMESSAGE 11
#define CORSSINGINFIOMESSAGE 12
#define CONTROLMESSAGE 13

//��װ������Ϣ
void getVehicleMessage(u8 *message, Head *head, VehicleInfomation *payload);
//�������յ���״̬��Ϣ
void analysisStausMessage(u8 *message,CrossingInfomation  *payload);
//������Ϣͷ
void analysisMessageHead(u8 *message, Head *head);


#endif
