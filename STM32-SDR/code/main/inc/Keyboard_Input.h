/*
 * Keyboard_Input.h
 *
 *  Created on: Jun 15, 2013
 *      Author: CharleyK
 */


void kybd_addCharacter(char c);
void kybd_dispFunctionKey(uint8_t d);
void set_kybd_mode(int data);
void kybd_char_switch(char data);
void kybd_clear_call(void);
void kybd_clear_name(void);

void kybd_edit_my_call(void);
void kybd_clear_my_call(void);
void kybd_clear_my_name(void);

uint8_t call_cnt;
uint8_t my_call_cnt;

#define KBD_BUFF_LEN 40
extern volatile char kybd_string[KBD_BUFF_LEN];

void String2Buffer(char *str);
void Text2Buffer(char *str, uint8_t count);
