#ifndef  __VL53L0X_H
#define  __VL53L0X_H


#include "vl53l0x_api.h"
#include "vl53l0x_platform.h"
#include "vl53l0x_i2c_platform.h"


extern VL53L0X_Error vl53l0x_status;
extern VL53L0X_RangingMeasurementData_t vl53l0x_data;

extern u16 distancebuff[4];

extern VL53L0X_Dev_t vl53l0x_dev0;
extern VL53L0X_Dev_t vl53l0x_dev1;
extern VL53L0X_Dev_t vl53l0x_dev2;
extern VL53L0X_Dev_t vl53l0x_dev3;

uint16_t vl53l0x_start_single_test(VL53L0X_Dev_t *pdev, 
                            VL53L0X_RangingMeasurementData_t *pdata);
							
VL53L0X_Error vl53l0x_init(void);

VL53L0X_Error vl53l0x_Addr_set(VL53L0X_Dev_t *dev,uint8_t newaddr);
VL53L0X_Error vl53l0x_initX( VL53L0X_Dev_t *pMyDevice ,u8 vl53l0_x);

void vl53l0x_getdistance();

#endif
