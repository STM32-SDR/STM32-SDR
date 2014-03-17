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
#include "xprintf.h"

#define TRUE 1
#define FALSE 0

// A macro to allow tracing of all USB events.
#define USB_KEYBRD_MARKER() do { \
	debug (USB, "USB KEYBRDEvent:   %s()   at line %d.\n", __func__, __LINE__); \
	} while (0)
		//USB_debugEvent(__LINE__);
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


extern void Acquire (void);

void Dummy (void);

typedef void (*pfunc0_t)(void);

void Dummy(void){

}

//keyboard function pointers
#define  Ctl_A Dummy
#define  Ctl_B Dummy
#define  Ctl_C Dummy
#define  Ctl_D Dummy
#define  Ctl_E Dummy
#define  Ctl_F Dummy
#define  Ctl_G Dummy
#define  Ctl_H Dummy
#define  Ctl_I Dummy
#define  Ctl_J Dummy
#define  Ctl_K Dummy
#define  Ctl_L Dummy
#define  Ctl_M Dummy
#define  Ctl_N Dummy
#define  Ctl_O Dummy
#define  Ctl_P Dummy
#define  Ctl_Q Dummy
#define  Ctl_R Dummy
#define  Ctl_S Dummy
#define  Ctl_T Dummy
#define  Ctl_U Dummy
#define  Ctl_V Dummy
#define  Ctl_W Dummy
#define  Ctl_X Dummy
#define  Ctl_Y Dummy
#define  Ctl_Z Dummy

#define  Alt_A Dummy
#define  Alt_B Dummy
#define  Alt_C Dummy
#define  Alt_D Dummy
#define  Alt_E Dummy
#define  Alt_F Dummy
#define  Alt_G Dummy
#define  Alt_H Dummy
#define  Alt_I Dummy
#define  Alt_J Dummy
#define  Alt_K Dummy
#define  Alt_L Dummy
#define  Alt_M Dummy
#define  Alt_N Dummy
#define  Alt_O Dummy
#define  Alt_P Dummy
#define  Alt_Q Dummy
#define  Alt_R Dummy
#define  Alt_S Dummy
#define  Alt_T Dummy
#define  Alt_U Dummy
#define  Alt_V Dummy
#define  Alt_W Dummy
#define  Alt_X Dummy
#define  Alt_Y Dummy
#define  Alt_Z Dummy

//Note--Fn and Shift-Fn are handled with special ASCII characters

#define  C_F1 Dummy
#define  C_F2 Dummy
#define  C_F3 Dummy
#define  C_F4 Dummy
#define  C_F5 Dummy
#define  C_F6 Dummy
#define  C_F7 Dummy
#define  C_F8 Dummy
#define  C_F9 Dummy
#define  C_F10 Dummy
#define	 C_END Acquire

#define  A_F1 Sel_Filt1
#define  A_F2 Sel_Filt2
#define  A_F3 Sel_Filt3
#define  A_F4 Sel_Filt4
#define  A_F5 Sel_Filt5
#define  A_F6 Dummy
#define  A_F7 Dummy
#define  A_F8 Dummy
#define  A_F9 Dummy
#define  A_F10 No_Filt
#define  A_END Acquire


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
/*	4D	END	*/	{	0x8C,	0x8C },
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
/*	59	1END*/	{	'1',	0x8C },
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
/*	0A	gG	*/	{	Ctl_G, 		Alt_G, 		NULL },
/*	0B	hH	*/	{	Ctl_H, 		Alt_H, 		NULL },
/*	0C	iI	*/	{	Ctl_I, 		Alt_I, 		NULL },
/*	0D	jJ	*/	{	Ctl_J, 		Alt_J, 		NULL },
/*	0E	kK	*/	{	Ctl_K, 		Alt_K, 		NULL },
/*	0F	lL	*/	{	Ctl_L, 		Alt_L, 		NULL },

/*	Hex				Ctrl		Alt			Ctl+Alt	*/
/*	10	mM	*/	{	Ctl_M, 		Alt_M, 		NULL },
/*	11	nN	*/	{	Ctl_N, 		Alt_N, 		NULL },
/*	12	oO	*/	{	Ctl_O, 		Alt_O, 		NULL },
/*	13	pP	*/	{	Ctl_P, 		Alt_P, 		NULL },
/*	14	qQ	*/	{	Ctl_Q, 		Alt_Q, 		NULL },
/*	15	rR	*/	{	Ctl_R, 		Alt_R, 		NULL },
/*	16	sS	*/	{	Ctl_S, 		Alt_S, 		NULL },
/*	17	tT	*/	{	Ctl_T, 		Alt_T, 		NULL },
/*	18	uU	*/	{	Ctl_U, 		Alt_U, 		NULL },
/*	19	vV	*/	{	Ctl_V, 		Alt_V, 		NULL },
/*	1A	wW	*/	{	Ctl_W, 		Alt_W, 		NULL },
/*	1B	xX	*/	{	Ctl_X, 		Alt_X, 		NULL },
/*	1C	yY	*/	{	Ctl_Y, 		Alt_Y, 		NULL },
/*	1D	zZ	*/	{	Ctl_Z, 		Alt_Z, 		NULL },
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
/*	3A	F1	*/	{	C_F1,		A_F1,		NULL },
/*	3B	F2	*/	{	C_F2,		A_F2, 		NULL },
/*	3C	F3	*/	{	C_F3,		A_F3,		NULL },
/*	3D	F4	*/	{	C_F4,		A_F4,		NULL },
/*	3E	F5	*/	{	C_F5,		A_F5,		NULL },
/*	3F	F6	*/	{	C_F6,		A_F6,		NULL },

/*	Hex				Ctrl		Alt	 		Ctl+Alt	*/
/*	40	F7	*/	{	C_F7,		A_F7,		NULL },
/*	41	F8	*/	{	C_F8,		A_F8, 		NULL },
/*	42	F9	*/	{	C_F9,		A_F9, 		NULL },
/*	43	F10	*/	{	C_F10,		A_F10, 		NULL },
/*	44	F11	*/	{	NULL,		NULL,		NULL },
/*	45	F12	*/	{	NULL,		NULL, 		NULL },
/*	46	PS	*/	{	NULL,		NULL, 		NULL },
/*	47	SL	*/	{	NULL,		NULL, 		NULL },
/*	48	PA	*/	{	NULL,		NULL, 		NULL },
/*	49	INS	*/	{	NULL,		NULL, 		NULL },
/*	4A	HM	*/	{	NULL,		NULL, 		NULL },
/*	4B	PU	*/	{	NULL,		NULL, 		NULL },
/*	4C	DEL	*/	{	NULL,		NULL, 		NULL },
/*	4D	END	*/	{	C_END,		A_END, 		NULL },
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

	//USB_KEYBRD_MARKER();

	//static uint8_t shift;
	static uint8_t keys[KBR_MAX_NBR_PRESSED];
	static uint8_t keys_new[KBR_MAX_NBR_PRESSED];
	static uint8_t keys_last[KBR_MAX_NBR_PRESSED];
	static uint8_t key_newest;
	static uint8_t nbr_keys;
	static uint8_t nbr_keys_new;
	static uint8_t nbr_keys_last;

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

		xputc(key_newest);

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

