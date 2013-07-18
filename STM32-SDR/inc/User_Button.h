/*
 * User_Button.h
 *
 *  Created on: Sep 9, 2012
 *      Author: John
 */

#ifndef USER_BUTTON_H_
#define USER_BUTTON_H_
#include	"arm_math.h"

void User_Button_Config(void);
void BT_Flag_Config(void);
_Bool IsBTConnected(void);

/*
 * Interface to queue general events (small ones) for after ISR processing
 *
 * For example, if an LCD write command is current done in an ISR, it's moved here
 * because the LCD code is not threadsafe for access inside the ISR.
 */
typedef enum {
	// Values must be unique single bits (1, 2, 4, 8, ...)
	DelayEvent_DisplayStoreIQ = 0x01,
	DelayEvent_DisplayStoreFreq = 0x02,
} DelayEvent_Event;

// Handle (and clear) the triggered events.
void DelayEvent_ProcessDelayedEvents(void);
void DelayEvent_TriggerEvent(DelayEvent_Event event);


#endif /* USER_BUTTON_H_ */
