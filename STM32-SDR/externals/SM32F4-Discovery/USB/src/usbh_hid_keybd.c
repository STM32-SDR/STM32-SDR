/**
 ******************************************************************************
 * @file    usbh_hid_keybd.c
 * @author  MCD Application Team
 * @version V2.1.0
 * @date    19-March-2012
 * @brief   This file is the application layer for USB Host HID Keyboard handling
 *          QWERTY and AZERTY Keyboard are supported as per the selection in
 *          usbh_hid_keybd.h
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
#include "usbh_hid_keybd.h"
#include "usbh_def.h"
#include <stdio.h>
#include "STM32-SDR-Subroutines.h"
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
 * @brief    This file includes HID Layer Handlers for USB Host HID class.
 * @{
 */

/** @defgroup USBH_HID_KEYBD_Private_TypesDefinitions
 * @{
 */
/**
 * @}
 */

/** @defgroup USBH_HID_KEYBD_Private_Defines
 * @{
 */
/**
 * @}
 */

/** @defgroup USBH_HID_KEYBD_Private_Macros
 * @{
 */
/**
 * @}
 */

/** @defgroup USBH_HID_KEYBD_Private_FunctionPrototypes
 * @{
 */
static void KEYBRD_Init(void);
static void KEYBRD_Decode(uint8_t *data);

/**
 * @}
 */

#ifdef USB_OTG_HS_INTERNAL_DMA_ENABLED
#if defined   (__CC_ARM) /*!< ARM Compiler */
__align(4)
#elif defined ( __ICCARM__ ) /*!< IAR Compiler */
#pragma data_alignment=4
#elif defined (__GNUC__) /*!< GNU Compiler */
#pragma pack(4)
#elif defined  (__TASKING__) /*!< TASKING Compiler */
__align(4)
#endif /* __CC_ARM */
#endif

/** @defgroup USBH_HID_KEYBD_Private_Variables
 * @{
 */
HID_cb_TypeDef HID_KEYBRD_cb = { KEYBRD_Init, KEYBRD_Decode };

/*
 *******************************************************************************
 *                                             LOCAL CONSTANTS
 *******************************************************************************
 */

//keyboard function pointers


void Dummy (void);

typedef void (*pfunc0_t)(void);

void Dummy(void){

}


#define  Ctl_A (pfunc0_t)CtlAFunc
#define  Ctl_B (pfunc0_t)CtlBFunc
#define  Ctl_C (pfunc0_t)CtlCFunc
#define  Ctl_D (pfunc0_t)CtlDFunc
#define  Ctl_E (pfunc0_t)CtlEFunc
#define  Ctl_F (pfunc0_t)CtlFFunc

#define  Alt_A (pfunc0_t)AltAFunc
#define  Alt_B (pfunc0_t)AltBFunc
#define  Alt_C (pfunc0_t)AltCFunc
#define  Alt_D (pfunc0_t)AltDFunc
#define  Alt_E (pfunc0_t)AltEFunc
#define  Alt_F (pfunc0_t)AltFFunc


#define  Fkey (pfunc0_t)FKeyFunc
#define  Fkey10 (pfunc0_t)FKey10Func
#define  S_Fkey (pfunc0_t)SFKeyFunc
#define  C_Fkey (pfunc0_t)CFKeyFunc
#define  A_Fkey (pfunc0_t)AFKeyFunc
#define  A_Fkey10 (pfunc0_t)AFKey10Func

void (*Ctl_A_Pointer)(void) = Dummy;
void (*Ctl_B_Pointer)(void) = Dummy;
void (*Ctl_C_Pointer)(void) = Dummy;
void (*Ctl_D_Pointer)(void) = Dummy;
void (*Ctl_E_Pointer)(void) = Dummy;
void (*Ctl_F_Pointer)(void) = Dummy;

void (*Alt_A_Pointer)(void) = Dummy;
void (*Alt_B_Pointer)(void) = Dummy;
void (*Alt_C_Pointer)(void) = Dummy;
void (*Alt_D_Pointer)(void) = Dummy;
void (*Alt_E_Pointer)(void) = Dummy;
void (*Alt_F_Pointer)(void) = Dummy;

void (*FKey_Pointer)(void) = Dummy;
void (*SFKey_Pointer)(void) = Dummy;
void (*CFKey_Pointer)(void) = Dummy;
void (*AFKey_Pointer)(void) = Dummy;
void (*AFKey10_Pointer)(void) = ToggleRxTx;

//Initialize pointers
void CtlAFunc (void) { (*Ctl_A_Pointer) (); }
void CtlBFunc (void) { (*Ctl_B_Pointer) (); }
void CtlCFunc (void) { (*Ctl_C_Pointer) (); }
void CtlDFunc (void) { (*Ctl_D_Pointer) (); }
void CtlEFunc (void) { (*Ctl_E_Pointer) (); }
void CtlFFunc (void) { (*Ctl_F_Pointer) (); }

void AltAFunc (void) { (*Alt_A_Pointer) (); }
void AltBFunc (void) { (*Alt_B_Pointer) (); }
void AltCFunc (void) { (*Alt_C_Pointer) (); }
void AltDFunc (void) { (*Alt_D_Pointer) (); }
void AltEFunc (void) { (*Alt_E_Pointer) (); }
void AltFFunc (void) { (*Alt_F_Pointer) (); }

void FKeyFunc (void) { (*FKey_Pointer) (); }
void SFKeyFunc (void) { (*SFKey_Pointer) (); }
void CFKeyFunc (void) { (*CFKey_Pointer) (); }
void AFKeyFunc (void) { (*AFKey_Pointer) (); }
void AFKey10Func (void) { (*AFKey10_Pointer) (); }


static const uint8_t	HID_KEYBOARD_DECODER1 [ 0x64 ][ 2 ] 	=	{

/*	Hex	Key			Norm	Shift */
/*	00		*/	{	'\0',	'\0' },
/*	01		*/	{	'\0',	'\0' },
/*	02		*/	{	'\0',	'\0' },
/*	03		*/	{	'\0',	'\0' },
/*	04	aA	*/	{	'a',	'A'  },
/*	05	bB	*/	{	'b',	'B'  },
/*	06	cC	*/	{	'c',	'C'  },
/*	07	dD	*/	{	'd',	'D'  },
/*	08	eE	*/	{	'e',	'E'  },
/*	09	fF	*/	{	'f',	'F'  },
/*	0A	gG	*/	{	'g',	'G'  },
/*	0B	hH	*/	{	'h',	'H'  },
/*	0C	iI	*/	{	'i',	'I'  },
/*	0D	jJ	*/	{	'j',	'J'  },
/*	0E	kK	*/	{	'k',	'K'  },
/*	0F	lL	*/	{	'l',	'L'  },

/*	Hex				Norm	Shift */
/*	10	mM	*/	{	'm',	'M'  },
/*	11	nN	*/	{	'n',	'N'  },
/*	12	oO	*/	{	'o',	'O'  },
/*	13	pP	*/	{	'p',	'P'  },
/*	14	qQ	*/	{	'q',	'Q'  },
/*	15	rR	*/	{	'r',	'R'  },
/*	16	sS	*/	{	's',	'S'  },
/*	17	tT	*/	{	't',	'T'  },
/*	18	uU	*/	{	'u',	'U'  },
/*	19	vV	*/	{	'v',	'V'  },
/*	1A	wW	*/	{	'w',	'W'  },
/*	1B	xX	*/	{	'x',	'X'  },
/*	1C	yY	*/	{	'y',	'Y'  },
/*	1D	zZ	*/	{	'z',	'Z'  },
/*	1E	1!	*/	{	'1',	'!'  },
/*	1F	2@	*/	{	'2',	'@'  },

/*	Hex				Norm	Shift */
/*	20	3#	*/	{	'3',	'#'  },
/*	21	4$	*/	{	'4',	'$'  },
/*	22	5%	*/	{	'5',	'%'  },
/*	23	6^	*/	{	'6',	'^'  },
/*	24	7&	*/	{	'7',	'&'  },
/*	25	8*	*/	{	'8',	'*'  },
/*	26	9(	*/	{	'9',	'('  },
/*	27	0)	*/	{	'0',	')'  },
/*	28	ENT	*/	{	0x0D,	0x0D },
/*	29	ESC	*/	{	0x1B,	0x1B },
/*	2A	DEL	*/	{	0x08,	0x08 },
/*	2B	TAB	*/	{	0x09,	0x09 },
/*	2C	SPC	*/	{	' ',	' '  },
/*	2D	-_	*/	{	'-',	'_'  },
/*	2E	=+	*/	{	'=',	'+'  },
/*	2F	[{	*/	{	'[',	'{'  },

/*	Hex				Norm	Shift */
/*	30	]}	*/	{	']',	'}'  },
/*	31	\|	*/	{	'\\',	'|'  },
/*	32		*/	{	'\0',	'\0' },
/*	33	;:	*/	{	';',	':'  },
/*	34	'"	*/	{	'\'',	'"'  },
/*	35		*/	{	'\0',	'\0' },
/*	36	,<	*/	{	',',	'<'  },
/*	37	.>	*/	{	'.',	'>'  },
/*	38	/?	*/	{	'/',	'?'  },
/*	39	CAP	*/	{	'\0',	'\0' },
/*	3A	F1	*/	{	0x80,	0x90 },
/*	3B	F2	*/	{	0x81,	0x91 },
/*	3C	F3	*/	{	0x82,	0x92 },
/*	3D	F4	*/	{	0x83,	0x93 },
/*	3E	F5	*/	{	0x84,	0x94 },
/*	3F	F6	*/	{	0x85,	0x95 },

/*	Hex				Norm	Shift */
/*	40	F7	*/	{	0x86,	0x96 },
/*	41	F8	*/	{	0x87,	0x97 },
/*	42	F9	*/	{	0x88,	0x98 },
/*	43	F10	*/	{	0x89,	0x99 },
/*	44	F11	*/	{	0x8A,	0x9A },
/*	45	F12	*/	{	0x8B,	0x9B },
/*	46	PS	*/	{	'\0',	'\0' },
/*	47	SL	*/	{	'\0',	'\0' },
/*	48	PA	*/	{	'\0',	'\0' },
/*	49	INS	*/	{	'\0',	'\0' },
/*	4A	HM	*/	{	'\0',	'\0' },
/*	4B	PU	*/	{	'\0',	'\0' },
/*	4C	DEL	*/	{	0x08,	0x08 },
/*	4D	END	*/	{	'\0',	'\0' },
/*	4E	PD	*/	{	'\0',	'\0' },
/*	4F	RGT	*/	{	0xA0,	0xB0 },

/*	Hex				Norm	Shift */
/*	50	LFT	*/	{	0xA1,	0xB1 },
/*	51	DWN	*/	{	0xA2,	0xB2 },
/*	52	UP	*/	{	0xA3,	0xB3 },
/*	53	NUM	*/	{	'\0',	'\0' },
/*	54	/	*/	{	'/',	'/'  },
/*	55	*	*/	{	'*',	'*'  },
/*	56	-	*/	{	'-',	'-'  },
/*	57	+	*/	{	'+',	'+'  },
/*	58	ENT	*/	{	0x0D,	0x0D },
/*	59	1END*/	{	'1',	'\0' },
/*	5A	2DWN*/	{	'2',	'\0' },
/*	5B	3PD	*/	{	'3',	'\0' },
/*	5C	4LFT*/	{	'4',	'\0' },
/*	5D	5	*/	{	'5',	'\0' },
/*	5E	6RGT*/	{	'6',	'\0' },
/*	5F	7HOM*/	{	'7',	'\0' },

/*	Hex				Norm	Shift */
/*	60	8UP	*/	{	'8',	'\0' },
/*	61	9PU	*/	{	'9',	'\0' },
/*	62	0INS*/	{	'0',	'\0' },
/*	63	.DEL*/	{	'.',	0x08 }
};

static pfunc0_t HID_KEYBOARD_DECODER2 [ 0x64 ][ 3 ]	=	{

/*	Hex	Key			Ctrl		Alt	  		Ctl+Alt	*/
/*	00		*/	{	NULL,		NULL,  		NULL },
/*	01		*/	{	NULL,		NULL,  		NULL },
/*	02		*/	{	NULL,		NULL,  		NULL },
/*	03		*/	{	NULL,		NULL,  		NULL },
/*	04	aA	*/	{	Ctl_A,		Alt_A,		NULL },
/*	05	bB	*/	{	Ctl_B, 		Alt_B,  	NULL },
/*	06	cC	*/	{	Ctl_C, 		Alt_C, 		NULL },
/*	07	dD	*/	{	Ctl_D, 		Alt_D, 		NULL },
/*	08	eE	*/	{	Ctl_E, 		Alt_E, 		NULL },
/*	09	fF	*/	{	Ctl_F, 		Alt_F, 		NULL },
/*	0A	gG	*/	{	NULL, 		NULL, 		NULL },
/*	0B	hH	*/	{	NULL, 		NULL, 		NULL },
/*	0C	iI	*/	{	NULL, 		NULL, 		NULL },
/*	0D	jJ	*/	{	NULL, 		NULL, 		NULL },
/*	0E	kK	*/	{	NULL, 		NULL, 		NULL },
/*	0F	lL	*/	{	NULL, 		NULL, 		NULL },

/*	Hex				Ctrl		Alt			Ctl+Alt	*/
/*	10	mM	*/	{	NULL, 		NULL, 		NULL },
/*	11	nN	*/	{	NULL, 		NULL, 		NULL },
/*	12	oO	*/	{	NULL, 		NULL, 		NULL },
/*	13	pP	*/	{	NULL, 		NULL, 		NULL },
/*	14	qQ	*/	{	NULL, 		NULL, 		NULL },
/*	15	rR	*/	{	NULL, 		NULL, 		NULL },
/*	16	sS	*/	{	NULL, 		NULL, 		NULL },
/*	17	tT	*/	{	NULL, 		NULL, 		NULL },
/*	18	uU	*/	{	NULL, 		NULL, 		NULL },
/*	19	vV	*/	{	NULL, 		NULL, 		NULL },
/*	1A	wW	*/	{	NULL, 		NULL, 		NULL },
/*	1B	xX	*/	{	NULL, 		NULL, 		NULL },
/*	1C	yY	*/	{	NULL, 		NULL, 		NULL },
/*	1D	zZ	*/	{	NULL, 		NULL, 		NULL },
/*	1E	1!	*/	{	NULL,  		NULL, 		NULL },
/*	1F	2@	*/	{	NULL,		NULL, 		NULL },

/*	Hex				Ctrl		Alt	 		Ctl+Alt*/
/*	20	3#	*/	{	NULL,		NULL, 		NULL },
/*	21	4$	*/	{	NULL,		NULL, 		NULL },
/*	22	5%	*/	{	NULL,		NULL, 		NULL },
/*	23	6^	*/	{	NULL,		NULL, 		NULL },
/*	24	7&	*/	{	NULL,		NULL, 		NULL },
/*	25	8*	*/	{	NULL,		NULL, 		NULL },
/*	26	9(	*/	{	NULL,		NULL, 		NULL },
/*	27	0)	*/	{	NULL,		NULL, 		NULL },
/*	28	ENT	*/	{	NULL,		NULL, 		NULL },
/*	29	ESC	*/	{	NULL,		NULL, 		NULL },
/*	2A	DEL	*/	{	NULL,		NULL, 		NULL },
/*	2B	TAB	*/	{	NULL,		NULL, 		NULL },
/*	2C	SPC	*/	{	NULL,		NULL, 		NULL },
/*	2D	-_	*/	{	NULL,		NULL, 		NULL },
/*	2E	=+	*/	{	NULL,		NULL, 		NULL },
/*	2F	[{	*/	{	NULL,		NULL, 		NULL },

/*	Hex				Ctrl		Alt	 		Ctl+Alt	*/
/*	30	]}	*/	{	NULL,		NULL, 		NULL },
/*	31	\|	*/	{	NULL,		NULL, 		NULL },
/*	32		*/	{	NULL,		NULL, 		NULL },
/*	33	;:	*/	{	NULL,		NULL, 		NULL },
/*	34	'"	*/	{	NULL,		NULL, 		NULL },
/*	35		*/	{	NULL,		NULL, 		NULL },
/*	36	,<	*/	{	NULL,		NULL, 		NULL },
/*	37	.>	*/	{	NULL,		NULL,		NULL },
/*	38	/?	*/	{	NULL,		NULL, 		NULL },
/*	39	CAP	*/	{	NULL,		NULL, 		NULL },
/*	3A	F1	*/	{	C_Fkey,		A_Fkey,		NULL },
/*	3B	F2	*/	{	C_Fkey,		A_Fkey, 	NULL },
/*	3C	F3	*/	{	C_Fkey,		A_Fkey,		NULL },
/*	3D	F4	*/	{	C_Fkey,		A_Fkey,		NULL },
/*	3E	F5	*/	{	C_Fkey,		A_Fkey,		NULL },
/*	3F	F6	*/	{	C_Fkey,		A_Fkey,		NULL },

/*	Hex				Ctrl		Alt	 		Ctl+Alt	*/
/*	40	F7	*/	{	C_Fkey,		A_Fkey,		NULL },
/*	41	F8	*/	{	C_Fkey,		A_Fkey, 	NULL },
/*	42	F9	*/	{	C_Fkey,		A_Fkey, 	NULL },
/*	43	F10	*/	{	C_Fkey,		A_Fkey10, 	NULL },
/*	44	F11	*/	{	NULL,		NULL,		NULL },
/*	45	F12	*/	{	NULL,		NULL, 		NULL },
/*	46	PS	*/	{	NULL,		NULL, 		NULL },
/*	47	SL	*/	{	NULL,		NULL, 		NULL },
/*	48	PA	*/	{	NULL,		NULL, 		NULL },
/*	49	INS	*/	{	NULL,		NULL, 		NULL },
/*	4A	HM	*/	{	NULL,		NULL, 		NULL },
/*	4B	PU	*/	{	NULL,		NULL, 		NULL },
/*	4C	DEL	*/	{	NULL,		NULL, 		NULL },
/*	4D	END	*/	{	NULL,		NULL, 		NULL },
/*	4E	PD	*/	{	NULL,		NULL, 		NULL },
/*	4F	RGT	*/	{	NULL,		NULL, 		NULL },

/*	Hex				Ctrl		Alt	 		Ctl+Alt*/
/*	50	LFT	*/	{	NULL,		NULL, 		NULL },
/*	51	DWN	*/	{	NULL,		NULL, 		NULL },
/*	52	UP	*/	{	NULL,		NULL, 		NULL },
/*	53	NUM	*/	{	NULL,		NULL, 		NULL },
/*	54	/	*/	{	NULL,		NULL, 		NULL },
/*	55	*	*/	{	NULL,		NULL, 		NULL },
/*	56	-	*/	{	NULL,		NULL, 		NULL },
/*	57	+	*/	{	NULL,		NULL, 		NULL },
/*	58	ENT	*/	{	NULL,		NULL, 		NULL },
/*	59	1END*/	{	NULL,		NULL, 		NULL },
/*	5A	2DWN*/	{	NULL,		NULL, 		NULL },
/*	5B	3PD	*/	{	NULL,		NULL, 		NULL },
/*	5C	4LFT*/	{	NULL,		NULL, 		NULL },
/*	5D	5	*/	{	NULL,		NULL, 		NULL },
/*	5E	6RGT*/	{	NULL,		NULL, 		NULL },
/*	5F	7HOM*/	{	NULL,		NULL, 		NULL },

/*	Hex				Ctrl		Alt			Ctl+Alt*/
/*	60	8UP	*/	{	NULL,		NULL,		NULL },
/*	61	9PU	*/	{	NULL,		NULL,		NULL },
/*	62	0INS*/	{	NULL,		NULL,		NULL },
/*	63	.DEL*/	{	NULL,		NULL,		NULL }
};




/**
 * @}
 */

/** @defgroup USBH_HID_KEYBD_Private_Functions
 * @{
 */

/**
 * @brief  KEYBRD_Init.
 *         Initialize the keyboard function.
 * @param  None
 * @retval None
 */
static void KEYBRD_Init(void)
{
	/* Call User Init*/
	USR_KEYBRD_Init();
}

/**
 * @brief  KEYBRD_ProcessData.
 *         The function is to decode the pressed keys.
 * @param  pbuf : Pointer to the HID IN report data buffer
 * @retval None
 */

static void KEYBRD_Decode(uint8_t *pbuf)
{
	//static uint8_t shift;
	static uint8_t keys[KBR_MAX_NBR_PRESSED];
	static uint8_t keys_new[KBR_MAX_NBR_PRESSED];
	static uint8_t keys_last[KBR_MAX_NBR_PRESSED];
	static uint8_t key_newest;
	static uint8_t nbr_keys;
	static uint8_t nbr_keys_new;
	static uint8_t nbr_keys_last;

	static uint8_t temp;
	static uint8_t Decoder_Col;

	void (* tempf)(void);

	uint8_t ix;
	uint8_t jx;
	uint8_t error;
	uint8_t output;
	//uint8_t functionKey;

	nbr_keys = 0;
	nbr_keys_new = 0;
	nbr_keys_last = 0;
	key_newest = 0x00;



	error = FALSE;

	/* Check for the value of pressed key */
	for (ix = 2; ix < 2 + KBR_MAX_NBR_PRESSED; ix++) {
		if ((pbuf[ix] == 0x01) || (pbuf[ix] == 0x02) || (pbuf[ix] == 0x03)) {
			error = TRUE;
		}
	}

	if (error == TRUE) {
		return;
	}

	nbr_keys = 0;
	nbr_keys_new = 0;
	for (ix = 2; ix < 2 + KBR_MAX_NBR_PRESSED; ix++) {
		if (pbuf[ix] != 0) {
			keys[nbr_keys] = pbuf[ix];
			nbr_keys++;
			for (jx = 0; jx < nbr_keys_last; jx++) {
				if (pbuf[ix] == keys_last[jx]) {
					break;
				}
			}

			if (jx == nbr_keys_last) {
				keys_new[nbr_keys_new] = pbuf[ix];
				nbr_keys_new++;
			}
		}
	}

	if (nbr_keys_new == 1) {
		key_newest = keys_new[0];



		switch (pbuf[0]) {
			case 0x0:	//normal
				Decoder_Col = 0;
				break;
			case 0x01:	//Ctl
			case 0x10:
				Decoder_Col = 2;
				break;
			case 0x02:	//Shifted
			case 0x20:
				Decoder_Col = 1;
				break;
			case 0x04:	//Alt
			case 0x40:
				Decoder_Col = 3;
				break;
			case 0x05:	//Ctl+Alt
			case 0x50:
			case 0x14:
			case 0x41:
				Decoder_Col = 4;
				break;

		}

		if (Decoder_Col < 2){
			output = HID_KEYBOARD_DECODER1[key_newest][Decoder_Col];
			USR_KEYBRD_ProcessData(output, key_newest);
		}
		else
		{
			tempf = HID_KEYBOARD_DECODER2[key_newest][Decoder_Col-2];
			tempf();
		}

		//functionKey = HID_KEYBRD_Codes[key_newest];

		/* call userSR_KEYB process handle */

	}
	else {
		key_newest = 0x00;
	}

	nbr_keys_last = nbr_keys;
	for (ix = 0; ix < KBR_MAX_NBR_PRESSED; ix++) {
		keys_last[ix] = keys[ix];
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

/**
 * @}
 */

/**
 * @}
 */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/

