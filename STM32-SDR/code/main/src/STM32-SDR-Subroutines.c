//general usage subroutines--not specific to a screen

#include "STM32-SDR-Subroutines.h"
#include "ChangeOver.h"
#include "widgets.h"
#include "arm_math.h"
#include <string.h>
#include <stdio.h>

#define		POST_FILT_SIZE 125
int i;

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

extern void ClearTextDisplay(void);

extern _Bool Encoders_IsOptionsEncoderPressed(void);

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


void Acquire( void )
{
	extern int count;
	extern int char_count;
	extern unsigned char FFT_Filter[];
	int TempIndex;
	extern double NCO_Frequency;
	long i, S1, S2, W;
	double delta;

	/* this is where I  add a correction to the NCO frequency
		based on the nearby spectral peaks */
	TempIndex = (int)(NCO_Frequency/15.625 +0.5);
	S1 = 0;
	S2 = 0;
	delta = 0.;
	//for (i=-2; i<3; i++){
	for (i=-4; i<5; i++){
		W = (long)FFT_Filter[TempIndex  + i];
		S1 += W*i;
		S2 += W;
	}
	if (S2 == 0){
		delta = 0.0;
	}
	else {
		delta = (double) S1/((double)S2);
	}
	if ((delta < -5.)||(delta > 5.)) delta = 0.0;
	NCO_Frequency += (double)((float)delta *15.625);
	SetRXFrequency (NCO_Frequency );
	count = 0;
	char_count = 0;
}

void Sel_Filt1 ( void )
{
	Flow = 300;
	Fhigh = 3300;
	sprintf(sFilt,"F1");
	for ( i=0; i < POST_FILT_SIZE; i++)
	PFC[i] = post_FILT_Coeff[i][0];
}

void Sel_Filt2 ( void )
{
	Flow = 300;
	Fhigh = 2700;
	sprintf(sFilt,"F2");
	for ( i=0; i < POST_FILT_SIZE; i++)
	PFC[i] = post_FILT_Coeff[i][1];
}

void Sel_Filt3 ( void )
{
	Flow = 300;
	Fhigh = 1300;
	sprintf(sFilt,"F3");
	for ( i=0; i < POST_FILT_SIZE; i++)
	PFC[i] = post_FILT_Coeff[i][2];
}

void Sel_Filt4 ( void )
{
	Flow = 400;
	Fhigh = 700;
	sprintf(sFilt,"F4");
	for ( i=0; i < POST_FILT_SIZE; i++)
	PFC[i] = post_FILT_Coeff[i][3];
}

void Sel_Filt5 ( void )
{
	Flow = 500;
	Fhigh = 600;
	sprintf(sFilt,"F5");
	for ( i=0; i < POST_FILT_SIZE; i++)
	PFC[i] = post_FILT_Coeff[i][4];
}

void No_Filt ( void )
{
	Flow = 125;
	Fhigh = 3875;
	FilterNumber = Filt0;
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
