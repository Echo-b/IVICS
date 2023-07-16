#ifndef __CROSSINFORMATION_H
#define __CROSSINFORMATION_H

#include "stm32f10x.h"  // Device header

#define OPEN 1
#define CLOSE_TEAIN 2
#define CLOSE_MALFUNCTION 3
#define CLOSE_OBSTACLE 4
#define CLOSE_INIT 5
#define CLOSE_TYPE 6

typedef struct {
    u8 staus;
	u8 address;
} CrossingInfomation;

#endif
