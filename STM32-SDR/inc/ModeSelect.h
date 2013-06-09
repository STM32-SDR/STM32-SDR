/*
 * ModeSelect.h
 *
 *  Created on: Apr 7, 2013
 *      Author: CharleyK
 */

#define MODE_SSB   0
#define MODE_CW    1
#define MODE_PSK   2

extern uint16_t Mode;

void Init_Mode(void);
void Set_Mode_Display(void);
