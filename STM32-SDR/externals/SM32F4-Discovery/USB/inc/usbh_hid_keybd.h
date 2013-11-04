/**
 ******************************************************************************
 * @file    usbh_hid_keybd.h
 * @author  MCD Application Team
 * @version V2.1.0
 * @date    19-March-2012
 * @brief   This file contains all the prototypes for the usbh_hid_keybd.c
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

/* Define to prevent recursive -----------------------------------------------*/
#ifndef __USBH_HID_KEYBD_H
#define __USBH_HID_KEYBD_H

/* Includes ------------------------------------------------------------------*/
#include "usb_conf.h"
#include "usbh_hid_core.h"

/** @addtogroup USBH_LIB
 * @{
 */

/** @addtogroup USBH_CLASS
 * @{
 */

/** @addtogroup USBH_HID_CLASS
 * @{
 */

/** @defgroup USBH_HID_KEYBD
 * @brief This file is the Header file for USBH_HID_KEYBD.c
 * @{
 */

/** @defgroup USBH_HID_KEYBD_Exported_Types
 * @{
 */

/**
 * @}
 */

/** @defgroup USBH_HID_KEYBD_Exported_Defines
 * @{
 */
#define QWERTY_KEYBOARD
//#define AZERTY_KEYBOARD

#define  KBD_LEFT_CTRL                                  0x01
#define  KBD_LEFT_SHIFT                                 0x02
#define  KBD_LEFT_ALT                                   0x04
#define  KBD_LEFT_GUI                                   0x08
#define  KBD_RIGHT_CTRL                                 0x10
#define  KBD_RIGHT_SHIFT                                0x20
#define  KBD_RIGHT_ALT                                  0x40
#define  KBD_RIGHT_GUI                                  0x80

#define  KBR_MAX_NBR_PRESSED                            6

/**
 * @}
 */

/** @defgroup USBH_HID_KEYBD_Exported_Macros
 * @{
 */
/**
 * @}
 */

/** @defgroup USBH_HID_KEYBD_Exported_Variables
 * @{
 */

extern HID_cb_TypeDef HID_KEYBRD_cb;
/**
 * @}
 */

/** @defgroup USBH_HID_KEYBD_Exported_FunctionsPrototype
 * @{
 */
void USR_KEYBRD_Init(void);
void USR_KEYBRD_ProcessData(uint8_t pbuf, uint8_t d);

void CtlAFunc (void) ;
void CtlBFunc (void) ;
void CtlCFunc (void) ;
void CtlDFunc (void) ;
void CtlEFunc (void) ;
void CtlFFunc (void) ;

void AltAFunc (void) ;
void AltBFunc (void) ;
void AltCFunc (void) ;
void AltDFunc (void) ;
void AltEFunc (void) ;
void AltFFunc (void) ;

void FKeyFunc (void) ;
void SFKeyFunc (void) ;
void CFKeyFunc (void) ;
void AFKeyFunc (void) ;
void AFKey10Func (void) ;

void (*Ctl_A_Pointer)(void);
void (*Ctl_B_Pointer)(void);
void (*Ctl_C_Pointer)(void);
void (*Ctl_D_Pointer)(void);
void (*Ctl_E_Pointer)(void);
void (*Ctl_F_Pointer)(void);

void (*Alt_A_Pointer)(void);
void (*Alt_B_Pointer)(void);
void (*Alt_C_Pointer)(void);
void (*Alt_D_Pointer)(void);
void (*Alt_E_Pointer)(void);
void (*Alt_F_Pointer)(void);

void (*FKey_Pointer)(void);
void (*SFKey_Pointer)(void);
void (*CFKey_Pointer)(void);
void (*AFKey_Pointer)(void);
void (*AFKey10_Pointer)(void);

/**
 * @}
 */

#endif /* __USBH_HID_KEYBD_H */

/**
 * @}
 */

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

