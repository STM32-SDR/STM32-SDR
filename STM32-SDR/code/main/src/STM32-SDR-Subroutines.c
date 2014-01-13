//general usage subroutines--not specific to a screen

#include "STM32-SDR-Subroutines.h"
#include "ChangeOver.h"
#include "widgets.h"
#include "arm_math.h"

#define		POST_FILT_SIZE 125
int i;
int NoFILT = 1;

extern const q15_t post_FILT_Coeff[125][5];
q15_t PFC[125];

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
	NoFILT = 0;
	for ( i=0; i < POST_FILT_SIZE; i++)
	PFC[i] = post_FILT_Coeff[i][0];
}

void Sel_Filt2 ( void )
{
	NoFILT = 0;
	for ( i=0; i < POST_FILT_SIZE; i++)
	PFC[i] = post_FILT_Coeff[i][1];
}

void Sel_Filt3 ( void )
{
	NoFILT = 0;
	for ( i=0; i < POST_FILT_SIZE; i++)
	PFC[i] = post_FILT_Coeff[i][2];
}

void Sel_Filt4 ( void )
{
	NoFILT = 0;
	for ( i=0; i < POST_FILT_SIZE; i++)
	PFC[i] = post_FILT_Coeff[i][3];
}

void Sel_Filt5 ( void )
{
	NoFILT = 0;
	for ( i=0; i < POST_FILT_SIZE; i++)
	PFC[i] = post_FILT_Coeff[i][4];
}

void No_Filt ( void )
{
	NoFILT = 1;
}
