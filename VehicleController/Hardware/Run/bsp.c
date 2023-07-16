
#include "bsp.h"
#include "bsp_ultrasonic.h"

void bsp_init(void)
{
    delay_init();

    Ultrasonic_GPIO_Init();

    MOTOR_GPIO_Init();                 /*���GPIO��ʼ��*/
    Motor_PWM_Init(7200, 11, 7200, 1); /*����Ƶ��PWMƵ�� 72000000/7200=10khz	  */
}
