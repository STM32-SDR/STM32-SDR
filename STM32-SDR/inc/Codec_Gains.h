/*
 * Codec_Gains.h
 *
 *  Created on: Mar 28, 2013
 *      Author: CharleyK
 */

#ifndef CODEC_GAINS_H_
#define CODEC_GAINS_H_



#endif /* CODEC_GAINS_H_ */

	void Set_HP_Gain (int HP_gain );

	void Set_LO_Gain (int LO_gain );

	void Set_PGA_Gain (int PGA_gain );

	void Set_ADC_DVC (int ADC_gain );

	void Set_DAC_DVC (int DAC_gain );

	void Turn_On_Bias(void);

	void Turn_Off_Bias(void);

	void Disconnect_PGA(void);

	void Mute_HP(void);

	void Mute_LO(void);

	void Connect_IQ_Inputs (void) ;

	void Connect_Microphone_Input (void);
