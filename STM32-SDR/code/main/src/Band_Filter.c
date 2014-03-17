/*
 * Code to handle setting the GPIO bits for the external band filter
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

#include	"stm32f4xx_gpio.h"
#include	"stm32f4xx_rcc.h"
//#include	"DMA_Test_Pins.h"


void GPIO_BandFilterInit(void)
{
	//Structure Assignment
	GPIO_InitTypeDef GPIO_InitStructure;

	/* Enable GPIOC clock */
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOC, ENABLE);
	/* Enable SYSCFG clock */
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_SYSCFG, ENABLE);

	//Define the GPIO Structure
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_13 | GPIO_Pin_14 | GPIO_Pin_15 ;

	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_OD;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;

	//Init the GPIO Test Pins
	GPIO_Init(GPIOC, &GPIO_InitStructure);
}
void GPIO_SetFilter(uint8_t  val){
	if (val % 2)
		GPIO_WriteBit(GPIOC, GPIO_Pin_13, Bit_SET);
	else
		GPIO_WriteBit(GPIOC, GPIO_Pin_13, Bit_RESET);
	val /= 2;
	if (val % 2)
		GPIO_WriteBit(GPIOC, GPIO_Pin_14, Bit_SET);
	else
		GPIO_WriteBit(GPIOC, GPIO_Pin_14, Bit_RESET);
	val /= 2;
	if (val % 2)
		GPIO_WriteBit(GPIOC, GPIO_Pin_15, Bit_SET);
	else
		GPIO_WriteBit(GPIOC, GPIO_Pin_15, Bit_RESET);
}
