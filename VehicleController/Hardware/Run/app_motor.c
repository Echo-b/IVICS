              


#include "app_motor.h"
#include "bsp_motor.h"
#include "sys.h"


void LeftMotorPWM(u16 Speed) {
    TIM_SetCompare2(TIM4, Speed);
}

void RightMotorPWM(u16 Speed) {
    TIM_SetCompare1(TIM4, Speed);
}

void LeftMotor_Go() {
    GPIO_SetBits(Motor_Port, Left_MotoA_Pin);
    GPIO_ResetBits(Motor_Port, Left_MotoB_Pin);
}
void LeftMotor_Back() {
    GPIO_ResetBits(Motor_Port, Left_MotoA_Pin);
    GPIO_SetBits(Motor_Port, Left_MotoB_Pin);
}
void LeftMotor_Stop() {
    GPIO_ResetBits(Motor_Port, Left_MotoA_Pin);
    GPIO_ResetBits(Motor_Port, Left_MotoB_Pin);
}

void RightMotor_Go() {
    GPIO_SetBits(Motor_Port, Right_MotoA_Pin);
    GPIO_ResetBits(Motor_Port, Right_MotoB_Pin);
}
void RightMotor_Back() {
    GPIO_ResetBits(Motor_Port, Right_MotoA_Pin);
    GPIO_SetBits(Motor_Port, Right_MotoB_Pin);
}
void RightMotor_Stop() {
    GPIO_ResetBits(Motor_Port, Right_MotoA_Pin);
    GPIO_ResetBits(Motor_Port, Right_MotoB_Pin);
}

/**
* Function       Car_Run
* @author        liusen
* @date          2017.07.17    
* @brief         小车前进
* @param[in]     Speed  （0~4800） 速度范围
* @param[out]    void
* @retval        void
* @par History   无
*/

void Car_Run(int Speed) {
    LeftMotor_Go();
    RightMotor_Go();
    LeftMotorPWM(Speed);
    RightMotorPWM(Speed);
}

/**
* Function       Car_Back
* @author        liusen
* @date          2017.07.17    
* @brief         小车后退
* @param[in]     Speed  （0~4800） 速度范围
* @param[out]    void
* @retval        void
* @par History   无
*/

void Car_Back(int Speed) {
    LeftMotor_Back();
    RightMotor_Back();
    LeftMotorPWM(Speed);
    RightMotorPWM(Speed);
}

/**
* Function       Car_Left
* @author        liusen
* @date          2017.07.17    
* @brief         小车左转
* @param[in]     Speed  （0~4800） 速度范围
* @param[out]    void
* @retval        void
* @par History   无
*/

void Car_Left(int Speed) {
    LeftMotor_Stop();
    RightMotor_Go();
    LeftMotorPWM(0);
    RightMotorPWM(Speed);
}

/**
* Function       Car_Right
* @author        liusen
* @date          2017.07.17    
* @brief         小车右转
* @param[in]     Speed  （0~4800） 速度范围
* @param[out]    void
* @retval        void
* @par History   无
*/

void Car_Right(int Speed) {
    LeftMotor_Go();
    RightMotor_Stop();
    LeftMotorPWM(Speed);
    RightMotorPWM(0);
}

/**
* Function       Car_Stop
* @author        liusen
* @date          2017.07.17    
* @brief         小车刹车
* @param[in]     void
* @param[out]    void
* @retval        void
* @par History   无
*/

void Car_Stop(void) {
    LeftMotor_Stop();
    RightMotor_Stop();
    LeftMotorPWM(0);
    RightMotorPWM(0);
}

/**
* Function       Car_SpinLeft
* @author        liusen
* @date          2017.07.17    
* @brief         小车左旋
* @param[in]     LeftSpeed：左电机速度  RightSpeed：右电机速度 取值范围：（0~4800）
* @param[out]    void
* @retval        void
* @par History   无
*/

void Car_SpinLeft(int LeftSpeed, int RightSpeed) {
    LeftMotor_Back();
    RightMotor_Go();
    LeftMotorPWM(LeftSpeed);
    RightMotorPWM(RightSpeed);
}

/**
* Function       Car_SpinRight
* @author        liusen
* @date          2017.07.17    
* @brief         小车右旋
* @param[in]     LeftSpeed：左电机速度  RightSpeed：右电机速度 取值范围：（0~4800）
* @param[out]    void
* @retval        void
* @par History   无
*/

void Car_SpinRight(int LeftSpeed, int RightSpeed) {
    LeftMotor_Go();
    RightMotor_Back();
    LeftMotorPWM(LeftSpeed);
    RightMotorPWM(RightSpeed);
}
