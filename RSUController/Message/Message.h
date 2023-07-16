#ifndef __MESSAGE_H
#define __MESSAGE_H

#include "CorssingInformation.h"
#include "Head.h"
#include "VehicleInformation.h"
#include "stm32f10x.h" // Device header

#define VEHICLEINFOMESSAGE 11
#define CORSSINGINFIOMESSAGE 12
#define CONTROLMESSAGE 13

void getStausMessageToVechicle(u8 *message, Head *head, CrossingInfomation *payload);
void analysisVehicleMessage(u8 *message, VehicleInfomation *payload);
void analysisControlMessageForOne(u8 *message, CrossingInfomation *payload);
void analysisControlMessageForTwo(u8 *message, CrossingInfomation *payload);
void analysisControlMessageForThree(u8 *message, CrossingInfomation *payload);
void analysisMessageHead(u8 *message, Head *head);
u8 getMessageToServer(u8 *message, CrossingInfomation *payload, VehicleInfomation *vehicleInfo);

#endif
