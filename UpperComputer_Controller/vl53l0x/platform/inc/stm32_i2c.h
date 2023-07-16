#ifndef _VL53L0X_I2C_H
#define _VL53L0X_I2C_H

#include "stm32f10x.h"
#include "stm32f10x_i2c.h"


//四个VL53L0挂载在同一个IIC总线下，所以使用四个片选信号--2019/10/30
//!!!!!!!注意：重新使能设备后，设备iic的地址会恢复为默认值0x52--2019/10/30
//VL53L0 0
#define I2C_SCL_GPIO       	GPIOA
#define	I2C_PIN_SCL       	GPIO_Pin_0
#define I2C_SCL_HIGH()      GPIO_SetBits(I2C_SCL_GPIO,I2C_PIN_SCL) 
#define I2C_SCL_LOW()  	    GPIO_ResetBits(I2C_SCL_GPIO,I2C_PIN_SCL)

#define I2C_SDA_GPIO       	GPIOA
#define	I2C_PIN_SDA       	GPIO_Pin_1
#define I2C_SDA_HIGH()      GPIO_SetBits(I2C_SDA_GPIO,I2C_PIN_SDA) 
#define I2C_SDA_LOW()  	    GPIO_ResetBits(I2C_SDA_GPIO,I2C_PIN_SDA)
#define I2C_SDA_STATE       GPIO_ReadInputDataBit(I2C_SDA_GPIO,I2C_PIN_SDA)

//片选使能--2019/10/30
#define I2C_X_GPIO       	GPIOA
#define	I2C_PIN_X0       	GPIO_Pin_2
#define I2C_X0_HIGH()       GPIO_SetBits(I2C_X_GPIO,I2C_PIN_X0) 
#define I2C_X0_LOW()  	    GPIO_ResetBits(I2C_X_GPIO,I2C_PIN_X0)

#define	I2C_PIN_X1       	GPIO_Pin_3
#define I2C_X1_HIGH()       GPIO_SetBits(I2C_X_GPIO,I2C_PIN_X1) 
#define I2C_X1_LOW()  	    GPIO_ResetBits(I2C_X_GPIO,I2C_PIN_X1)

#define	I2C_PIN_X2       	GPIO_Pin_4
#define I2C_X2_HIGH()       GPIO_SetBits(I2C_X_GPIO,I2C_PIN_X2) 
#define I2C_X2_LOW()  	    GPIO_ResetBits(I2C_X_GPIO,I2C_PIN_X2)

#define	I2C_PIN_X3       	GPIO_Pin_5
#define I2C_X3_HIGH()       GPIO_SetBits(I2C_X_GPIO,I2C_PIN_X3) 
#define I2C_X3_LOW()  	    GPIO_ResetBits(I2C_X_GPIO,I2C_PIN_X3)

#define	I2C_PIN_X4       	GPIO_Pin_6
#define I2C_X4_HIGH()       GPIO_SetBits(I2C_X_GPIO,I2C_PIN_X4) 
#define I2C_X4_LOW()  	    GPIO_ResetBits(I2C_X_GPIO,I2C_PIN_X4)


void i2c_init(void);
uint8_t i2c_write(uint8_t addr, uint8_t reg, uint32_t len, uint8_t * data);
uint8_t i2c_read(uint8_t addr, uint8_t reg, uint32_t len, uint8_t *buf);


#endif
