/*
 * Keyboard_Input.h
 *
 *  Created on: Jun 15, 2013
 *      Author: CharleyK
 */


void kybd_addCharacter(char c);
void kybd_dispFunctionKey(uint8_t d);


#define KBD_BUFF_LEN 40
extern volatile char kybd_string[KBD_BUFF_LEN];
