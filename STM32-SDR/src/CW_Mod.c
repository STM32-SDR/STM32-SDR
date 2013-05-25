/*
 * CW_Mod.c
 *
 *  Created on: Apr 8, 2013
 *      Author: CharleyK
 */

#include	"stm32f4xx_rcc.h"
#include 	"stm32f4xx_gpio.h"

void Init_CW_GPIO (void) {
	GPIO_InitTypeDef   GPIO_InitStructure;

	/* Enable GPIOC clock */
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOC, ENABLE);
	/* Enable SYSCFG clock */
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_SYSCFG, ENABLE);

	/* Configure PC9 pin as input floating */
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9;
	GPIO_Init(GPIOC, &GPIO_InitStructure);
	}


