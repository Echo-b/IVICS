#ifndef __HEAD_H
#define __HEAD_H
#include "stm32f10x.h"                  // Device header

typedef struct{
    u8 nodeId;
    u8 msgType;
} Head;

#endif
