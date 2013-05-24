/*
 * Init_I2c.h
 *
 *  Created on: Feb 7, 2013
 *      Author: CharleyK
 */

#include	"arm_math.h"


 //void Delay( __IO uint32_t nCount);

 void Delay( uint32_t nCount);


#define I2C_SPEED                        400000
#define I2C_SHORT_TIMEOUT             ((uint32_t)0x1000)
#define I2C_LONG_TIMEOUT             ((uint32_t)(300 * 0x1000))

#define	EEPROM_Write_Address	0b10100000
#define	EEPROM_Read_Address		0b10100001

 void I2C_GPIO_Init(void);
 void I2C_Cntrl_Init(void);

uint32_t I2C_WriteRegister(uint8_t DeviceAddr, uint8_t RegisterAddr, uint8_t RegisterValue);

uint32_t I2C_WriteRegister_N(uint8_t DeviceAddr, uint8_t RegisterAddr, uint8_t write[],uint8_t N );

uint32_t I2C_ReadSlave(uint8_t Address, uint8_t Register);

uint32_t I2C_WriteEEProm( uint16_t RegisterAddr, uint8_t RegisterValue);

uint32_t I2C_ReadEEProm( uint16_t RegisterAddr);

