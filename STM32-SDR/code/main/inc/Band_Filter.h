/*
 * header file to handle setting the GPIO bits for the external band filter
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

#define			Test_GPIO		GPIOC
#define 		Test_0			GPIO_Pin_13
#define 		Test_1			GPIO_Pin_14
#define 		Test_2			GPIO_Pin_15

void GPIO_BandFilterInit(void);
void GPIO_SetFilter(uint8_t  val);
//GPIO_WriteBit(DMA_GPIO, Test_0, Bit_SET);
//GPIO_WriteBit(DMA_GPIO, DMA_IRQ, Bit_RESET);
