/*
 * Header file for low level routine for EEPROM used in storing operational parameters
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




void EEProm_Test(void);
void check_eeprom(void);

int16_t Write_Int_EEProm(uint16_t EEProm_Addr, int16_t value);
int16_t Read_Int_EEProm(uint16_t EEProm_Addr);
uint32_t Write_Long_EEProm(uint16_t EEProm_Addr, uint32_t value);
uint32_t Read_Long_EEProm(uint16_t EEProm_Addr);

void Store_SI570_Data(void);

