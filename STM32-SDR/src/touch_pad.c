#include <assert.h>
#include "TSHal.h"


#include "touch_pad.h"
#include "TFT_Display.h"
#include "PSKDet.h"
#include "Encoder_1.h"
#include "Encoder_2.h"
#include "eeprom.h"
#include "DMA_IRQ_Handler.h"
#include "ChangeOver.h"
#include "ModeSelect.h"
#include "TSDriver_ADS7843.h"
#include "User_Button.h"
#include "options.h"
#include "FrequencyManager.h"



/* ****************************************************************
 *   ISR
 * ****************************************************************/
static void DelayUS(vu32 cnt)
{
	uint32_t i;
	for (i = 0; i < cnt; i++) {
		uint8_t us = 12;
		while (us--) {
			;
		}
	}
}

// TODO: Move to better loctaion.
void EXTI9_5_IRQHandler(void)
{
	//Handle Touch Screen Interrupts
	if (EXTI_GetITStatus(EXTI_Line6) != RESET) {
		TSDriver_HandleTouchInterrupt();
		EXTI_ClearITPendingBit(EXTI_Line6 );
	}

	//Handle Encoder #2 PB interrupt
	if (EXTI_GetITStatus(EXTI_Line7) != RESET) {
		// LCD code is not thread safe, so trigger a delayed event to display the "Store IQ" message.
		DelayEvent_TriggerEvent(DelayEvent_DisplayStoreIQ);

		DelayUS(10);
		Options_WriteToEEPROM();
		DelayUS(10);
		EXTI_ClearITPendingBit(EXTI_Line7 );
	}

	//Handle Encoder #1 PB interrupt
	if (EXTI_GetITStatus(EXTI_Line8) != RESET) {
		// LCD code is not thread safe, so trigger a delayed event to display the "Store Freq" message.
		DelayEvent_TriggerEvent(DelayEvent_DisplayStoreFreq);

		DelayUS(10);
		FrequencyManager_WriteBandsToEeprom();
		DelayUS(10);
		EXTI_ClearITPendingBit(EXTI_Line8 );
	}
}
