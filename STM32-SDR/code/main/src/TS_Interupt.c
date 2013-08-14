/*
 TS_Interrupt.c/
 *
 *  Created on: Sep 9, 2012
 *
 *      Author: John
 */
//===============================================================================

#include	"stm32f4xx_exti.h"
#include "TSDriver_ADS7843.h"


/* ****************************************************************
 *   ISR
 * ****************************************************************/
void EXTI9_5_IRQHandler(void)
{
	//Handle Touch Screen Interrupts
	if (EXTI_GetITStatus(EXTI_Line6) != RESET) {
		TSDriver_HandleTouchInterrupt();
		EXTI_ClearITPendingBit(EXTI_Line6 );
	}

}
