#ifndef __PAYLOAD__H
#define __PAYLOAD__H
#include "stm32f10x.h"                  // Device header

typedef struct Payload {
    u8 vehicleType;
    u8 vehicleSpeed;
} Payload;

#endif
