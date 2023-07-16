#ifndef __VEHICLEINFORMATION_H
#define __VEHICLEINFORMATION_H

#include "stm32f10x.h"  // Device header
#define CAR 1
#define TRUCK 2

typedef struct {
    u8 id;
    u8 type;
    u8 speed;
}VehicleInfomation;

#endif
