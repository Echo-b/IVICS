/**
 * @par Copyright (C): 2010-2019, Shenzhen Yahboom Tech
 * @file         app_ultrasonic.c
 * @author       liusen
 * @version      V1.0
 * @date         2015.01.03
 * @brief        ����������ģʽԴ�ļ�
 * @details
 * @par History  ������˵��
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
 * @brief         ����������
 * @param[in]     void
 * @param[out]    void
 * @retval        void
 * @par History   ��
 */
int app_ultrasonic_mode(void)
{
    float Len = 0;

    Len = bsp_getUltrasonicDistance();

    // printf("CSB:%f", Len);

    if (Len < 80) // ��ֵΪ�����ϰ���ľ��룬���԰�ʵ���������
    {
        // Car_Stop();  //ͣ��
        // Len = bsp_getUltrasonicDistance();
        //        while (Len < 80)  //�ٴ��ж��Ƿ����ϰ��������ת������󣬼����ж�
        //        {

        //            //Car_SpinRight(2600, 2600);
        //            delay_ms(300);
        //            Len = bsp_getUltrasonicDistance();
        //        }
        return 0;
    }
    else
    {
        // Car_Run(1000);  //���ϰ��ֱ��
        return 1;
    }
}
