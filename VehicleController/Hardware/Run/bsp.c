
#include "bsp.h"
#include "bsp_ultrasonic.h"

void bsp_init(void)
{
    delay_init();

    Ultrasonic_GPIO_Init();

    MOTOR_GPIO_Init();                 /*电机GPIO初始化*/
    Motor_PWM_Init(7200, 11, 7200, 1); /*不分频。PWM频率 72000000/7200=10khz	  */
}
