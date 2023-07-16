/**
 * @par Copyright (C): 2010-2019, Shenzhen Yahboom Tech
 * @file         app_ultrasonic.c
 * @author       liusen
 * @version      V1.0
 * @date         2015.01.03
 * @brief        超声波避障模式源文件
 * @details
 * @par History  见如下说明
 *
 * version:	liusen_20170717
 */

#include "app_ultrasonic.h"
#include "app_motor.h"
#include "bsp_ultrasonic.h"
#include "delay.h"
#include "sys.h"
#include "usart.h"

/**
 * Function       app_ultrasonic_mode
 * @author        liusen
 * @date          2017.07.20
 * @brief         超声波避障
 * @param[in]     void
 * @param[out]    void
 * @retval        void
 * @par History   无
 */
int app_ultrasonic_mode(void)
{
    float Len = 0;

    Len = bsp_getUltrasonicDistance();

    // printf("CSB:%f", Len);

    if (Len < 80) // 数值为碰到障碍物的距离，可以按实际情况设置
    {
        // Car_Stop();  //停车
        // Len = bsp_getUltrasonicDistance();
        //        while (Len < 80)  //再次判断是否有障碍物，若有则转动方向后，继续判断
        //        {

        //            //Car_SpinRight(2600, 2600);
        //            delay_ms(300);
        //            Len = bsp_getUltrasonicDistance();
        //        }
        return 0;
    }
    else
    {
        // Car_Run(1000);  //无障碍物，直行
        return 1;
    }
}
