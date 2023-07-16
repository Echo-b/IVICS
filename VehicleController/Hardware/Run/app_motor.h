

#ifndef __APP_MOTOR_H__
#define __APP_MOTOR_H__


#include "bsp_motor.h"



void Car_Run(int Speed);
void Car_Back(int Speed);
void Car_Left(int Speed);
void Car_Right(int Speed);
void Car_Stop(void);
void Car_SpinStop(void);
void Car_SpinLeft(int LeftSpeed, int RightSpeed);
void Car_SpinRight(int LeftSpeed, int RightSpeed);


#endif


