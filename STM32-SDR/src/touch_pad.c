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

static int16_t TouchCount = 0;


/*
 * Handle touch events using the initial hard-coded coordinate system.
 */
void Old_HandleTouchEvent(void)
{
	if (!TS_HasNewTouchEvent()) {
		return;
	}

	// Get the coordinates:
	uint16_t X_Point, Y_Point;
	TS_GetTouchEventCoords(&X_Point, &Y_Point);

	// Convert to the "old" coordinates of:
	// X Origin: On Left
	// Y Origin: On bottom
	X_Point = X_Point + 0;
	Y_Point = Y_Point + 0;
//	Y_Point = LCD_HEIGHT - Y_Point - 1;



	//Update PSK NCO Frequency
	if ((Y_Point > 176) && (X_Point > 39) && (X_Point < 280)) {
		NCO_Frequency = (double) ((float) ((X_Point - 40) + 8) * 15.625);
		Plot_Integer((int) NCO_Frequency, 150, 150);
		SetRXFrequency(NCO_Frequency);
	}

	//Update Frequency Step
	if ((X_Point > 234) && (Y_Point < 20)) {
		if (X_Point < 277)
			TouchCount++;
		if (X_Point > 277)
			TouchCount--;

		if (TouchCount == 2) {  //Increase Step
			Increase_Step();
			TouchCount = 0;
		} //end Increase step

		if (TouchCount == -2) { //Decrease Step
			Decrease_Step();
			TouchCount = 0;
		} // end Decrease Step

		// Redraw the frequency to update colors.
		redrawFrequencyOnScreen();
	}  //End of Frequency Step

	if ((X_Point > 320) && (Y_Point < 48)) {  //Store Default Freq & IQ
		Store_Defaults();  //Both Encoder 1 & 2 Defaults Stored, see Encoder_2.c
	}  //End of Store Defaults

	if ((X_Point > 320) && (Y_Point > 48) && (Y_Point < 96)) {  //USB Selection
		rgain = -0.5;
		LCD_StringLine(0, 220, "LSB");
	}

	if ((X_Point > 320) && (Y_Point > 96) && (Y_Point < 144)) {  //LSB Selection
		rgain = 0.5;
		LCD_StringLine(0, 220, "USB");
	}

	if ((X_Point > 320) && (Y_Point > 192)) {  //TX Selection

		switch (Mode) {
		case MODE_SSB:
			Xmit_SSB_Sequence();
			break;
		case MODE_CW:
			Xmit_CW_Sequence();
			break;
		case MODE_PSK:
			Xmit_PSK_Sequence();
			break;
		}  // End of switch

		LCD_StringLine(296, 220, "TX");
	}

	// RX Selection
	if ((X_Point > 320) && (Y_Point > 144) && (Y_Point < 192)) {
		Receive_Sequence();
		//Tx_Flag=0;
		LCD_StringLine(296, 220, "RX");
	}

	// SSB Mode Selection
	if ((X_Point > 88) && (X_Point < 128) && (Y_Point < 20) && (Tx_Flag == 0)) {
		Mode = MODE_SSB;
		Set_Mode_Display();
	}

	// CW Mode Selection
	if ((X_Point > 144) && (X_Point < 176) && (Y_Point < 20) && (Tx_Flag == 0)) {
		Mode = MODE_CW;
		Set_Mode_Display();
	}

	// PSK Mode Selection
	if ((X_Point > 188) && (X_Point < 228) && (Y_Point < 20) && (Tx_Flag == 0)) {
		Mode = MODE_PSK;
		Set_Mode_Display();
	}

}

/* ****************************************************************
 *   ISR
 * ****************************************************************/
static void DelayUS(vu32 cnt)
{
	uint16_t i;
	for (i = 0; i < cnt; i++) {
		uint8_t us = 12;
		while (us--) {
			;
		}
	}
}

void EXTI9_5_IRQHandler(void)
{
	//Handle Touch Screen Interrupts
	if (EXTI_GetITStatus(EXTI_Line6) != RESET) {
		TSDriver_HandleTouchInterrupt();
		EXTI_ClearITPendingBit(EXTI_Line6 );
	}

	//Handle Encoder #2 PB interrupt
	if (EXTI_GetITStatus(EXTI_Line7) != RESET) {
		LCD_StringLine(0, 40, "Store   IQ");
		DelayUS(10);
		Store_IQ_Data();
		DelayUS(10);
		EXTI_ClearITPendingBit(EXTI_Line7 );
	}

	//Handle Encoder #1 PB interrupt
	if (EXTI_GetITStatus(EXTI_Line8) != RESET) {
		LCD_StringLine(234, 40, "Store Freq");
		DelayUS(10);
		Store_SI570_Data();
		DelayUS(10);
		EXTI_ClearITPendingBit(EXTI_Line8 );
	}
}
