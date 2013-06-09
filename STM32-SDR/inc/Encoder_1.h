/*
 * Encoder_1.h
 *
 *  Created on: Feb 25, 2013
 *      Author: CharleyK
 */

#ifndef ENCODER_1_H_
#define ENCODER_1_H_

#endif /* ENCODER_1_H_ */

void redrawFrequencyOnScreen(void);
void init_encoder1(void);
void Encoder1_GPIO_Config(void);
void process_encoder1(void);
void check_SS1(void);
void process_SS1(void);
void Set_Initial_SI570_Data(void);
void Decrease_Step(void);
void Increase_Step(void);

int16_t read_SS1;
uint8_t F_Mult;

uint32_t Freq_Disp;
uint32_t Freq_Set[16];
uint32_t EEProm_Value1;
