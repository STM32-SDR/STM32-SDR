/*
 * ModeSelect.h
 *
 *  Created on: Apr 7, 2013
 *      Author: CharleyK
 */

#include <stdint.h>

#define MODE_SSB   0
#define MODE_CW    1
#define MODE_PSK   2

extern uint16_t Mode;

void Init_Mode(void);
