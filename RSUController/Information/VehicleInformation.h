#ifndef __VEHICLEINFORMATION_H
#define __VEHICLEINFORMATION_H

#include "stm32f10x.h"  // Device header
#define CAR 1
#define TRUCK 2

typedef struct {
    u8 id;
    u8 type;
    u8 speed;
    u8 dispatched;
}VehicleInfomation;

extern VehicleInfomation VehicleInfos[256];

void initVehicleInfos(void);
void updateVehicleInfos(u8 *message);

#endif
