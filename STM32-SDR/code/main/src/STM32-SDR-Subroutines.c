//general usage subroutines--not specific to a screen

#include "STM32-SDR-Subroutines.h"
#include "ChangeOver.h"
#include "widgets.h"
#include "arm_math.h"
#include "screen_All.h"
#include "ModeSelect.h"
#include <string.h>
#include <stdio.h>
#include "xprintf.h"
#include "options.h"

#define		POST_FILT_SIZE 125
int i;

#define SPECTRUM 0
#define WATERFALL 1

char sFilt[3];
unsigned int Flow;
unsigned int Fhigh;

typedef enum {
	Filt0 = 0,
	Filt1,
	Filt2,
	Filt3,
	Filt4,
	Filt5,
	NumFilts //automatically set count
}FiltNum;

FiltNum FilterNumber;

extern const q15_t post_FILT_Coeff[125][5];
q15_t PFC[125];

extern 	int WF_Flag;

extern _Bool Encoders_IsOptionsEncoderPressed(void);

extern void ClearXmitBuffer (void);
extern void ClearTextDisplay(void);
extern void FrequencyManager_SetCurrentFrequency(int value);
extern void FrequencyManager_SetSelectedBand(int value);

void Sel_Next_Filt (int);
void ChangeNextMode (void);
void ChangeNextAGC (void);

void ToggleRxTx (void) {
	if (RxTx_InTxMode()){
		RxTx_SetReceive();
//		if (Mode_GetCurrentMode() == MODE_PSK)
//				ClearTextDisplay();
	}
	else {
		RxTx_SetTransmit();
	}
}

void ToggleWaterfall (void) {
	WF_Flag = !WF_Flag;
	if (WF_Flag) Init_Waterfall();
}

void ChangeMode(int userMode){
	debug (KEYBOARD, "ChangeMode userMode = %d\n", userMode);
	Mode_SetCurrentMode(userMode);
	Screen_ShowMainScreen();
}

void SetFrequency(char *string){
	debug (KEYBOARD, "SetFrequency: string = %s\n", string);
	int value = 0;
	int length = strlen(string);
	for (i = 0; i < length; i++){
		// convert ascii to numeric
		if (string[i] >= 0x30 && string[i] <= 0x39){
			value = value * 10 + (int) string[i] - 0x30;
			debug (KEYBOARD, "SetFrequency: i = %d, string[i] = %c, value = %d\n", i, string[i], value);
		}
	}
	value *= 1000; //kHz to Hz
	debug (KEYBOARD, "SetFrequency: Changing to value = %d\n", value);
	FrequencyManager_SetSelectedBand(15); //Blank the frequency band display
	FrequencyManager_SetCurrentFrequency(value);
}

void PlayMacro (int n) {
	debug (KEYBOARD, "PlayMacro: n = %d", n);
	if (n == FilterNumber){ // toggle filters on/off
		debug (KEYBOARD, "PlayMacro: toggle filter off");
		n=0;
	}
	switch (n) {
		case 0:
			No_Filt();
			break;
		case 1:
			Sel_Filt1();
			break;
		case 2:
			Sel_Filt2();
			break;
		case 3:
			Sel_Filt3();
			break;
		case 4:
			Sel_Filt4();
			break;
		case 5:
			Sel_Filt5();
			break;
		case 6:
			Sel_Next_Filt(5);
			break;
		case 7:
			break;
		case 8:
			RxTx_SetReceive();
			break;
		case 9:
			RxTx_SetTransmit();
			break;
		case 10:
			ToggleRxTx();
			break;
		case 11:
			WF_Flag = SPECTRUM;
			Init_Waterfall();
			break;
		case 12:
			WF_Flag = WATERFALL;
			Init_Waterfall();
			break;
		case 13:
			ToggleWaterfall();
			break;
		case 14:
			ChangeMode(USERMODE_USB); // USB
			break;
		case 15:
			ChangeMode(USERMODE_LSB); // LSB
			break;
		case 16:
			ChangeMode(USERMODE_CW); // CW
			break;
		case 17:
			ChangeMode(USERMODE_CWR); // CWR
			break;
		case 18:
			ChangeMode(USERMODE_DIGU); // PSK-U
			break;
		case 19:
			ChangeMode(USERMODE_DIGL); // PSK-L
			break;
		case 20:
			ClearXmitBuffer(); // Clear
			ClearTextDisplay();
			break;
		case 22:
			ChangeNextMode(); // Cycle through modes
			break;
		case 23:
			Options_SetValue(OPTION_AGC_Mode, 0);
			break;
		case 24:
			Options_SetValue(OPTION_AGC_Mode, 1);
			break;
		case 25:
			Options_SetValue(OPTION_AGC_Mode, 2);
			break;
		case 26:
			Options_SetValue(OPTION_AGC_Mode, 3);
			break;
		case 27:
			ChangeNextAGC();
			break;
		case 28:
			Sel_Next_Filt(2);
			break;
		case 29:
			Sel_Next_Filt(3);
			break;
		case 30:
			Sel_Next_Filt(4);
			break;
		default:
			break;
	}
}

void ChangeNextMode (void) {
	debug (KEYBOARD, "ChangeNextMode:\n");
	UserModeType mode = Mode_GetCurrentUserMode();
	switch (mode){
	case USERMODE_USB:
		ChangeMode(USERMODE_LSB);
		break;
	case USERMODE_LSB:
		ChangeMode(USERMODE_CW);
		break;
	case USERMODE_CW:
		ChangeMode(USERMODE_DIGU);
		break;
	case USERMODE_CWR:
		ChangeMode(USERMODE_DIGU);
		break;
	case USERMODE_DIGU:
		ChangeMode(USERMODE_DIGL);
		break;
	case USERMODE_DIGL:
		ChangeMode(USERMODE_USB);
		break;
	default:
		break;
	}
}

void ChangeNextAGC (void) {
	debug (KEYBOARD, "ChangeNextAGC:\n");
	int agcValue;
	agcValue = Options_GetValue(OPTION_AGC_Mode);
	if (agcValue < 3)
		agcValue++;
	else
		agcValue = 0;
	Options_SetValue(OPTION_AGC_Mode, agcValue);
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


void Acquire( void )
{
	extern int count;
	extern int char_count;
	extern unsigned char FFT_Filter[];
	extern int NCO_Point;
	extern double NCO_Frequency;
	extern double NCO_1;
	long i, S1, S2, W;
	double delta;

	/* this is where I  add a correction to the NCO frequency
		based on the nearby spectral peaks */
	S1 = 0;
	S2 = 0;
	delta = 0.;
	//for (i=-2; i<3; i++){
	for (i=-4; i<5; i++){
		W = (long)FFT_Filter[NCO_Point + i];
		S1 += W*i;
		S2 += W;
	}
	if (S2 != 0) delta = (double) S1/((double)S2);
	NCO_Frequency +=  (double)((float)delta * 15.625);
	NCO_1 = NCO_Frequency;
	SetRXFrequency (NCO_Frequency );
	count = 0;
	char_count = 0;
}

void Sel_Filt1 ( void )
{
	debug (KEYBOARD, "Sel_Filt1:\n");
	Flow = 300;
	Fhigh = 3300;
	sprintf(sFilt,"F1");
	for ( i=0; i < POST_FILT_SIZE; i++)
	PFC[i] = post_FILT_Coeff[i][0];
	FilterNumber = Filt1;
}

void Sel_Filt2 ( void )
{
	debug (KEYBOARD, "Sel_Filt2:\n");
	Flow = 300;
	Fhigh = 2700;
	sprintf(sFilt,"F2");
	for ( i=0; i < POST_FILT_SIZE; i++)
	PFC[i] = post_FILT_Coeff[i][1];
	FilterNumber = Filt2;
}

void Sel_Filt3 ( void )
{
	debug (KEYBOARD, "Sel_Filt3:\n");
	Flow = 300;
	Fhigh = 1300;
	sprintf(sFilt,"F3");
	for ( i=0; i < POST_FILT_SIZE; i++)
	PFC[i] = post_FILT_Coeff[i][2];
	FilterNumber = Filt3;
}

void Sel_Filt4 ( void )
{
	debug (KEYBOARD, "Sel_Filt4:\n");
	Flow = 400;
	Fhigh = 700;
	sprintf(sFilt,"F4");
	for ( i=0; i < POST_FILT_SIZE; i++)
	PFC[i] = post_FILT_Coeff[i][3];
	FilterNumber = Filt4;
}

void Sel_Filt5 ( void )
{
	debug (KEYBOARD, "Sel_Filt5:\n");
	Flow = 500;
	Fhigh = 600;
	sprintf(sFilt,"F5");
	for ( i=0; i < POST_FILT_SIZE; i++)
	PFC[i] = post_FILT_Coeff[i][4];
	FilterNumber = Filt5;
}

void No_Filt ( void )
{
	debug (KEYBOARD, "No_Filt:\n");
	Flow = 125;
	Fhigh = 3875;
	FilterNumber = Filt0;
}

void Sel_Next_Filt (int depth)
{
	debug (KEYBOARD, "Sel_Next_Filt: depth = %d\n", depth);
	switch (FilterNumber){
	case Filt0:
		Sel_Filt1();
		break;
	case Filt1:
		if (depth > 1)
			Sel_Filt2();
		else
			No_Filt();
		break;
	case Filt2:
		if (depth > 2)
			Sel_Filt3();
		else
			No_Filt();
		break;
	case Filt3:
		if (depth > 3)
			Sel_Filt4();
		else
			No_Filt();
		break;
	case Filt4:
		if (depth > 4)
			Sel_Filt5();
		else
			No_Filt();
		break;
	case Filt5:
		No_Filt();
		break;
	default:
		break;
	}
}

void process_button ( void ){
	static unsigned char Press_cnt;
	static unsigned char ButtonPress;
	static unsigned char button_one_shot;
	ButtonPress = Encoders_IsOptionsEncoderPressed();
	button_one_shot |= ButtonPress;
	if (button_one_shot) Press_cnt++;
	if (!ButtonPress && Press_cnt){
		if(Press_cnt >=2){
			FilterNumber++;
			FilterNumber %= NumFilts;
			button_one_shot = 0;
			Press_cnt = 0;
			ProcessFilters (FilterNumber);
		}
	}
}

void ProcessFilters ( unsigned char filternumber){
	debug (KEYBOARD, "ProcessFilters:\n");
	switch (filternumber){
	case 0:
		No_Filt();
		break;
	case 1:
		Sel_Filt1();
		break;
	case 2:
		Sel_Filt2();
		break;
	case 3:
		Sel_Filt3();
		break;
	case 4:
		Sel_Filt4();
		break;
	case 5:
		Sel_Filt5();
		break;
	}
}
