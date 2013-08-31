/*
 * Header file for I2S Init code
 *
 * STM32-SDR: A software defined HAM radio embedded system.
 * Copyright (C) 2013, STM32-SDR Group
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
 */


#include	"arm_math.h"

void Delay(uint32_t nCount);

#define I2C_SPEED                        400000
#define I2C_SHORT_TIMEOUT             ((uint32_t)0x1000)
#define I2C_LONG_TIMEOUT             ((uint32_t)(300 * 0x1000))

#define	EEPROM_Write_Address	0b10100000
#define	EEPROM_Read_Address		0b10100001

void I2C_GPIO_Init(void);
void I2C_Cntrl_Init(void);

uint32_t I2C_WriteRegister(uint8_t DeviceAddr, uint8_t RegisterAddr, uint8_t RegisterValue);

uint32_t I2C_WriteRegister_N(uint8_t DeviceAddr, uint8_t RegisterAddr, uint8_t write[], uint8_t N);

uint32_t I2C_ReadSlave(uint8_t Address, uint8_t Register);

uint32_t I2C_WriteEEProm(uint16_t RegisterAddr, uint8_t RegisterValue);

uint32_t I2C_ReadEEProm(uint16_t RegisterAddr);

