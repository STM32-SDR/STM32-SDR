/**
 ******************************************************************************
 * @file    usbh_usr_lcd.c
 * @author  MCD Application Team
 * @version V2.1.0
 * @date    19-March-2012
 * @brief   This file includes the some user routines for LCD
 ******************************************************************************
 * @attention
 *
 * <h2><center>&copy; COPYRIGHT 2012 STMicroelectronics</center></h2>
 *
 * Licensed under MCD-ST Liberty SW License Agreement V2, (the "License");
 * You may not use this file except in compliance with the License.
 * You may obtain a copy of the License at:
 *
 *        http://www.st.com/software_license_agreement_liberty_v2
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 ******************************************************************************
 */
/* Includes ------------------------------------------------------------------*/
#include "usbh_usr_lcd.h"
#include "usb_conf.h"

#include <stdint.h>

int16_t x_loc = 0, y_loc = 0;
int16_t prev_x = 0, prev_y = 0;

/**
 * @brief  HID_MOUSE_UpdatePosition
 *         The function is to handle mouse scroll to upadte the mouse position
 *         on display window
 * @param   x : USB HID Mouse X co-ordinate
 * @param   y :  USB HID Mouse Y co-ordinate
 * @retval None
 */
void HID_MOUSE_UpdatePosition(int8_t x, int8_t y) {
	/* Set the color for button press status */
	//LCD_SetTextColor(LCD_COLOR_GREEN);
	//LCD_SetBackColor(LCD_COLOR_BLACK);
	if ((x != 0) || (y != 0)) {

		x_loc += x / 2;
		y_loc += y / 2;

		if (y_loc > MOUSE_WINDOW_HEIGHT - 12) {
			y_loc = MOUSE_WINDOW_HEIGHT - 12;
		}
		if (x_loc > MOUSE_WINDOW_WIDTH - 10) {
			x_loc = MOUSE_WINDOW_WIDTH - 10;
		}
		if (y_loc < 2) {
			y_loc = 2;
		}
		if (x_loc < 2) {
			x_loc = 2;
		}

		if ((prev_x != 0) && (prev_y != 0)) {
			/*LCD_DisplayChar(MOUSE_WINDOW_X + prev_y ,\
        MOUSE_WINDOW_Y - prev_x, ' ');*/
		} else if ((prev_x == 0) && (prev_y == 0)) {

			/* LCD_DisplayChar(MOUSE_WINDOW_X + 1,
			 MOUSE_WINDOW_Y - 1,
			 ' ');    */
		}

		/*LCD_DisplayChar  (MOUSE_WINDOW_X + y_loc ,\
      MOUSE_WINDOW_Y - x_loc, 'x');*/

		prev_x = x_loc;
		prev_y = y_loc;
	}
}

/**
 * @brief  HID_MOUSE_ButtonPressed
 *         The function is to handle mouse button press
 * @param  button_idx : mouse button pressed
 * @retval None
 */

void HID_MOUSE_ButtonPressed(uint8_t button_idx) {

	/* Set the color for button press status */
	//LCD_SetTextColor(LCD_COLOR_GREEN);
	//LCD_SetBackColor(LCD_COLOR_GREEN);
	/* Change the color of button pressed to indicate button press*/
	switch (button_idx) {
	/* Left Button Pressed */
	case 0:
		STM_EVAL_LEDToggle(LED_Green); // added by "STM32"
//    LCD_DrawFullRect(HID_MOUSE_BUTTON_XCHORD, HID_MOUSE_BUTTON1_YCHORD, HID_MOUSE_BUTTON_WIDTH, HID_MOUSE_BUTTON_HEIGHT );
		break;

		/* Right Button Pressed */
	case 1:
		STM_EVAL_LEDToggle(LED_Red); // added by "STM32"
//    LCD_DrawFullRect(HID_MOUSE_BUTTON_XCHORD,HID_MOUSE_BUTTON2_YCHORD, HID_MOUSE_BUTTON_WIDTH,HID_MOUSE_BUTTON_HEIGHT);
		break;

		/* Middle button Pressed */
	case 2:
		STM_EVAL_LEDToggle(LED_Orange); // added by "STM32"
//    LCD_DrawFullRect(HID_MOUSE_BUTTON_XCHORD,HID_MOUSE_BUTTON3_YCHORD, HID_MOUSE_BUTTON_WIDTH,HID_MOUSE_BUTTON_HEIGHT);
		break;
	}
}

/**
 * @brief  HID_Mouse_ButtonReleased
 *         The function is to handle mouse button release
 * @param  button_idx : mouse button released
 * @retval None
 */

void HID_MOUSE_ButtonReleased(uint8_t button_idx) {
	/* Set the color for release status */
	//LCD_SetTextColor(LCD_COLOR_WHITE);
	//LCD_SetBackColor(LCD_COLOR_WHITE);
	/* Change the color of button released to default button color*/
	switch (button_idx) {

	/* Left Button Released */
	case 0:
		/* LCD_DrawFullRect(HID_MOUSE_BUTTON_XCHORD, HID_MOUSE_BUTTON1_YCHORD, HID_MOUSE_BUTTON_WIDTH, HID_MOUSE_BUTTON_HEIGHT);*/
		break;

		/* Right Button Released */
	case 1:
		/*LCD_DrawFullRect(HID_MOUSE_BUTTON_XCHORD, HID_MOUSE_BUTTON2_YCHORD, HID_MOUSE_BUTTON_WIDTH, HID_MOUSE_BUTTON_HEIGHT);*/
		break;

		/* Middle Button Released */
	case 2:
		/*LCD_DrawFullRect (HID_MOUSE_BUTTON_XCHORD, HID_MOUSE_BUTTON3_YCHORD, HID_MOUSE_BUTTON_WIDTH, HID_MOUSE_BUTTON_HEIGHT);*/
		break;
	}
}

/**
 * @}
 */

/**
 * @}
 */

/**
 * @}
 */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
