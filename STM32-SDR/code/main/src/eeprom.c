/*
 * EEPROM low level access routines.
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
#include 	"stm32f4xx_i2c.h"
#include 	"Init_I2C.h"
#include 	"eeprom.h"

//Routine to write a integer value to the eeprom starting at eeprom address EEProm_Addr
int16_t Write_Int_EEProm(uint16_t EEProm_Addr, int16_t value)
{
	uint32_t result = 0;
	uint8_t i;
	uint8_t int_val_W[2];

	int_val_W[0] = (uint8_t) ((value & 0xFF00) >> 8);
	int_val_W[1] = (uint8_t) (value & 0x00FF);

	for (i = 0; i < 2; i++) {

		result = I2C_WriteEEProm(EEProm_Addr++, int_val_W[i]);
	}

	return result;
}

int16_t Read_Int_EEProm(uint16_t EEProm_Addr)
{
	int16_t result = 0;
	uint8_t i;
	uint8_t int_val_R[2];

	for (i = 0; i < 2; i++) {
		int_val_R[i] = (uint8_t) I2C_ReadEEProm(EEProm_Addr++);
	}

	result = (int16_t) int_val_R[0] << 8;
	result = (int16_t) int_val_R[1] | result;

	return result;
}

//Routine to write a long value to the eeprom starting at eeprom address EEProm_Addr
uint32_t Write_Long_EEProm(uint16_t EEProm_Addr, uint32_t value)
{
	uint32_t result = 0;
	uint8_t i;
	uint8_t long_val_W[4];

	long_val_W[0] = (uint8_t) (value >> 24 & 0x000000FF);
	long_val_W[1] = (uint8_t) (value >> 16 & 0x000000FF);
	long_val_W[2] = (uint8_t) (value >> 8 & 0x000000FF);
	long_val_W[3] = (uint8_t) (value & 0x000000FF);

	for (i = 0; i < 4; i++) {
		result = I2C_WriteEEProm(EEProm_Addr++, long_val_W[i]);
	}

	return result;
}

uint32_t Read_Long_EEProm(uint16_t EEProm_Addr)
{
	uint32_t result = 0;
	uint8_t i;
	uint8_t long_val_R[4];

	for (i = 0; i < 4; i++) {
		long_val_R[i] = (uint8_t) I2C_ReadEEProm(EEProm_Addr++);
	}

	result = (uint32_t) long_val_R[0] << 24;
	result = (uint32_t) long_val_R[1] << 16 | result;
	result = (uint32_t) long_val_R[2] << 8 | result;
	result = (uint32_t) long_val_R[3] | result;

	return result;
}
