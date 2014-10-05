/**
 ******************************************************************************
 * @file    usbh_usr.c
 * @author  MCD Application Team
 * @version V2.1.0
 * @date    19-March-2012
 * @brief   This file includes the user application layer
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
#include "usbh_usr.h"
#include "usbh_hid_keybd.h"
#include "Keyboard_Input.h"
#include "KeyboardStatus.h"
#include "xprintf.h"

extern void Acquire(void);

// A macro to allow tracing of all USB events.
#define USB_DEBUG_MARKER() do { \
	debug (USB, "USB Event:   %s()   at line %d.\n", __func__, __LINE__); \
	} while (0)
		//USB_debugEvent(__LINE__);

USBH_Usr_cb_TypeDef USR_Callbacks = {
		USBH_USR_Init,
		USBH_USR_DeInit,
		USBH_USR_DeviceAttached,
		USBH_USR_ResetDevice,
        USBH_USR_DeviceDisconnected,
        USBH_USR_OverCurrentDetected,
        USBH_USR_DeviceSpeedDetected,
        USBH_USR_Device_DescAvailable,
        USBH_USR_DeviceAddressAssigned,
        USBH_USR_Configuration_DescAvailable,
        USBH_USR_Manufacturer_String,
        USBH_USR_Product_String,
        USBH_USR_SerialNum_String,
        USBH_USR_EnumerationDone,
        USBH_USR_UserInput,
        NULL,
        USBH_USR_DeviceNotSupported,
        USBH_USR_UnrecoveredError
};


// Called by other functions when interesting USB events happen.
// Used for debugging; passed line number of calling method to help debug.
void USB_debugEvent(int lineNumber)
{
	static int usbEventCounter = 0;
	usbEventCounter++;

	debug (USB, "USB_debugEvent #%4d from line %d.\n", usbEventCounter, lineNumber);
}

/**
 * @brief  USBH_USR_Init
 *         Displays the message on LCD for host lib initialization
 * @param  None
 * @retval None
 */
void USBH_USR_Init(void)
{
	USB_DEBUG_MARKER();
}

/**
 * @brief  USBH_USR_DeviceAttached
 *         Displays the message on LCD on device attached
 * @param  None
 * @retval None
 */
void USBH_USR_DeviceAttached(void)
{
	//USB_DEBUG_MARKER();
	KeyboardStatus_SetDeviceAttached();
}

/**
 * @brief  USBH_USR_UnrecoveredError
 * @param  None
 * @retval None
 */
void USBH_USR_UnrecoveredError(void)
{
	USB_DEBUG_MARKER();
}

/**
 * @brief  USBH_DisconnectEvent
 *         Device disconnect event
 * @param  None
 * @retval None
 */
void USBH_USR_DeviceDisconnected(void)
{
	//USB_DEBUG_MARKER();
	KeyboardStatus_SetDeviceDesconnected();
}

/**
 * @brief  USBH_USR_ResetUSBDevice
 *         Reset USB Device
 * @param  None
 * @retval None
 */
void USBH_USR_ResetDevice(void)
{
	/* Users can do their application actions here for the USB-Reset */
	USB_DEBUG_MARKER();
}

/**
 * @brief  USBH_USR_DeviceSpeedDetected
 *         Displays the message on LCD for device speed
 * @param  Devicespeed : Device Speed
 * @retval None
 */
void USBH_USR_DeviceSpeedDetected(uint8_t DeviceSpeed)
{
	USB_DEBUG_MARKER();
	//  if(DeviceSpeed == HPRT0_PRTSPD_HIGH_SPEED)
	//  {
	////   LCD_UsrLog((void *)MSG_DEV_HIGHSPEED);
	//  }
	//  else if(DeviceSpeed == HPRT0_PRTSPD_FULL_SPEED)
	//  {
	////LCD_UsrLog((void *)MSG_DEV_FULLSPEED);
	//  }
	//  else if(DeviceSpeed == HPRT0_PRTSPD_LOW_SPEED)
	//  {
	////    LCD_UsrLog((void *)MSG_DEV_LOWSPEED);
	//  }
	//  else
	//  {
	////    LCD_UsrLog((void *)MSG_DEV_ERROR);
	//  }
}

/**
 * @brief  USBH_USR_Device_DescAvailable
 *         Displays the message on LCD for device descriptor
 * @param  DeviceDesc : device descriptor
 * @retval None
 */
void USBH_USR_Device_DescAvailable(void *DeviceDesc)
{
	USB_DEBUG_MARKER();
	//  uint8_t temp[50];
	//  USBH_DevDesc_TypeDef *hs;
	//  hs = DeviceDesc;
	//
	//
	//  sprintf((char *)temp , "VID : %04Xh\n" , (uint32_t)(*hs).idVendor); /
	//  //LCD_UsrLog((void *)temp);
	//
	//
	//  sprintf((char *)temp , "PID : %04Xh\n" , (uint32_t)(*hs).idProduct);
	//  //LCD_UsrLog((void *)temp);
}

/**
 * @brief  USBH_USR_DeviceAddressAssigned
 *         USB device is successfully assigned the Address
 * @param  None
 * @retval None
 */
void USBH_USR_DeviceAddressAssigned(void)
{
	USB_DEBUG_MARKER();
}

/**
 * @brief  USBH_USR_Conf_Desc
 *         Displays the message on LCD for configuration descriptor
 * @param  ConfDesc : Configuration descriptor
 * @retval None
 */
void USBH_USR_Configuration_DescAvailable(USBH_CfgDesc_TypeDef * cfgDesc, USBH_InterfaceDesc_TypeDef *itfDesc,
        USBH_EpDesc_TypeDef *epDesc)
{
	USB_DEBUG_MARKER();
	//  USBH_InterfaceDesc_TypeDef *id;
	//
	//  id = itfDesc;
	//
	//  if((*id).bInterfaceClass  == 0x08)
	//  {
	////    LCD_UsrLog((void *)MSG_MSC_CLASS);
	//  }
	//  else if((*id).bInterfaceClass  == 0x03)
	//  {
	////    LCD_UsrLog((void *)MSG_HID_CLASS);
	//  }
}

/**
 * @brief  USBH_USR_Manufacturer_String
 *         Displays the message on LCD for Manufacturer String
 * @param  ManufacturerString : Manufacturer String of Device
 * @retval None
 */
void USBH_USR_Manufacturer_String(void *ManufacturerString)
{
	USB_DEBUG_MARKER();
}

/**
 * @brief  USBH_USR_Product_String
 *         Displays the message on LCD for Product String
 * @param  ProductString : Product String of Device
 * @retval None
 */
void USBH_USR_Product_String(void *ProductString)
{
	USB_DEBUG_MARKER();
}

/**
 * @brief  USBH_USR_SerialNum_String
 *         Displays the message on LCD for SerialNum_String
 * @param  SerialNumString : SerialNum_String of device
 * @retval None
 */
void USBH_USR_SerialNum_String(void *SerialNumString)
{
	USB_DEBUG_MARKER();
}

/**
 * @brief  EnumerationDone
 *         User response request is displayed to ask for
 *         application jump to class
 * @param  None
 * @retval None
 */
void USBH_USR_EnumerationDone(void)
{
	USB_DEBUG_MARKER();

}

/**
 * @brief  USBH_USR_DeviceNotSupported
 *         Device is not supported
 * @param  None
 * @retval None
 */
void USBH_USR_DeviceNotSupported(void)
{
	// ** This function seems to be called often! **
	//USB_DEBUG_MARKER();
}

/**
 * @brief  USBH_USR_UserInput
 *         User Action for application state entry
 * @param  None
 * @retval USBH_USR_Status : User response for key button
 */
USBH_USR_Status USBH_USR_UserInput(void)
{
	// ** This function seems to be called often! **
	//USB_DEBUG_MARKER();

	USBH_USR_Status usbh_usr_status;

	usbh_usr_status = USBH_USR_NO_RESP;

	//STM_EVAL_LEDOn(LED_Blue); // added by "STM32"

	// commented by "STM32"
	//  /*Key B3 is in polling mode to detect user action */
	//  if(STM_EVAL_PBGetState(Button_WAKEUP) == SET)
	//  {

	usbh_usr_status = USBH_USR_RESP_OK;

	//  }

	return usbh_usr_status;

}

/**
 * @brief  USBH_USR_OverCurrentDetected
 *         Device Overcurrent detection event
 * @param  None
 * @retval None
 */
void USBH_USR_OverCurrentDetected(void)
{
	USB_DEBUG_MARKER();
	//   LCD_ErrLog ("Overcurrent detected.\n");

}

/**
 * @brief  USR_KEYBRD_Init
 *         Init Keyboard window
 * @param  None
 * @retval None
 */
void USR_KEYBRD_Init(void)
{
	//USB_DEBUG_MARKER();
	KeyboardStatus_SetKeyboardWorking();

}

/**
 * @brief  USR_KEYBRD_ProcessData
 *         Process Keyboard data
 * @param  data : Keyboard data to be displayed
 * @retval None
 */
void USR_KEYBRD_ProcessData(uint8_t data1,uint8_t data2 )
{
	//USB_DEBUG_MARKER();

	debug(KEYBOARD, "USR_KEYBRD_ProcessData: data1 = %x, data2 = %x\n", data1, data2);

	// Skip control characters like enter, bell, escape..
	//if ((data1 >= ' ' && data2 < 112)||(data1 == 10)||(data1==8)) {
	//if (data1 >= ' ' && data2 < 112) {
	// Could eliminate data2 and use only data1--data1 can be used to identify Function keys.

	// Check the Function Keys F1 thru F10
	if (data1 >=0x80 && data1<=0x8B) {
		//check for modified ASCII codes for F1-F12
		data1 = data1 - 0x80;
		kybd_dispFunctionKey(data1);
		//Note: 0x90 to 0x99 denote shifted Function keys 1 to 10.
	}
	else if (data1 == 0x8C) { Acquire();}
	else {
		//data1 is ASCII data
		kybd_char_switch(data1);
	}
	xputc(data1);
}

/**
* @brief USBH_USR_DeInit
* Deint User state and associated variables
* @param None
* @retval None
*/
void USBH_USR_DeInit(void)
{
        USB_DEBUG_MARKER();
}

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
