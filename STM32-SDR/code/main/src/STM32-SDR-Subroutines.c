//general usage subroutines--not specific to a screen

#include "STM32-SDR-Subroutines.h"
#include "ChangeOver.h"
#include "widgets.h"

extern 	int WF_Flag;

extern void ClearTextDisplay(void);

void ToggleRxTx (void) {
	if (RxTx_InTxMode()){
		RxTx_SetReceive();
		ClearTextDisplay();
	}
	else {
		RxTx_SetTransmit();
	}
}

void ToggleWaterfall (void) {
	WF_Flag = !WF_Flag;
	if (WF_Flag) Init_Waterfall();
}

void PlayMacro (int n) {

	switch (n) {
		case 0:
			break;
		case 1:
			break;
		case 2:
			break;
		case 3:
			break;
		case 4:
			break;
		case 5:
			break;
		case 6:
			break;
		case 7:
			break;
		case 8:
			break;
		case 9:
			break;
		case 10:
			ToggleRxTx();
			break;
		default:
			break;
	}
}

void RecordMacro (int n) {

}

void ClearMacro (int n) {

}

void RecordMyName (void) {

}

void RecordMyCall (void) {

}

void RecordTheirName (void) {

}

void RecordTheirCall (void) {

}

void TuneUpBig (void) {

}

void TuneUpSmall (void) {

}

void TuneDownBig (void) {

}

void TuneDownSmall (void) {

}
