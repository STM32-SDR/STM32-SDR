/*
 * Encoder_2.h
 *
 *  Created on: Mar 23, 2013
 *      Author: CharleyK
 */

#ifndef ENCODER_2_H_
#define ENCODER_2_H_

#endif /* ENCODER_2_H_ */
#include	"arm_math.h"

void init_encoder2(void);
void Encoder2_GPIO_Config(void);
void process_encoder2(void);
void check_SS2(void);
void display_SS2(void);
void Set_Initial_IQ_Data(void);
void Set_Minimums(void);
void Set_Maximums(void);
void Set_IQ_Units(void);
void Store_Defaults(void);

int16_t read_SS2;
int16_t IQData[16];
int16_t DataMin[16];
int16_t DataMax[16];
int16_t IQ_Unit[16];

int16_t EEProm_Value2;

