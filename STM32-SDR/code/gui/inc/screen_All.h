/*
 * Header File for Screen level graphics routines to support UI
 *
 * STM32-SDR: A software defined HAM radio embedded system.
 * Copyright (C) 2013, STM32-SDR Group
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
 */


/*
 * Entry point for all UI screen access
 *
 * Example usage in main():
 *  Screen_CreateAllScreens();
 *  Screen_ShowScreen(&g_screenMain);
 */

/*
 * Directions to add new screens:
 * ******************************
 * - Generally, look for the ## mark to find where changes are needed.
 *
 * Assume you want to create screen MyThing
 * 1. In screen_All.c, declare a global variable for your new screen:
 *    GL_Page_TypeDef g_screenMyThing;
 * 2. In screen_All.h, add the extern declaration for your screen:
 *    extern GL_Page_TypeDef g_screenMyThing;
 * 3. Create file screen_MyThing.c; copy code found in an existing screen file for a guide.
 *    - In screen_MyThing.h, change the variable s_pThisScreen to be:
 *      static GL_Page_TypeDef *s_pThisScreen = &g_screenMyThing;
 *    - This is just so you can always use the s_pThisScreen in your screen_XYZ.c file's
 *      without having to change it to always referring to the correct screen if the code
 *      is copy-and-pasted.
 * 4. In screen_MyThing.c, add a function to create the screen's widgets:
 *    void ScreenMyThing_Create(void) {...}
 *    - Add any functionality you need for your screen to this file, such as button callbacks,
 *      custom widget functions, etc...
 * 5. In screen_All.h, add prototype for ScreenMyThing_Create():
 *    void ScreenMyThing_Create(void);
 *    - Note: there is no need for a screen_MyThing.h file by doing this.
 * 6. In screen_All.c, change Screen_CreateAllScreens() to also call ScreenMyThing_Create().
 * 7. Change some other screen to show your new screen at some point by executing:
 *    Screen_ShowScreen(g_screenMyThing);
 */

#ifndef SCREEN_ALL_H
#define SCREEN_ALL_H

#include "graphicObjectTypes.h"
#include "graphicObject.h"
#include "LcdHal.h"
#include "Init_I2C.h"

// screen modes
#define MAIN 0
#define OPTIONS 1
#define ADVANCED 2
#define FILTER 3
#define FUNCTION 4
#define FREQUENCY 5
#define FREQEDIT 6

// Screen global variables (defined in screen_all.c) for switching screens
// ## Add new screens here:
extern GL_Page_TypeDef g_screenMainPSK;
extern GL_Page_TypeDef g_screenMainCW;
extern GL_Page_TypeDef g_screenMainSSB;
extern GL_Page_TypeDef g_screenCalibrate;
extern GL_Page_TypeDef g_screenCalibrationTest;
extern GL_Page_TypeDef g_screenMode;
extern GL_Page_TypeDef g_screenOptions;
extern GL_Page_TypeDef g_screenAdvanced;
extern GL_Page_TypeDef g_screenFilter;
extern GL_Page_TypeDef g_screenFrequencies;
extern GL_Page_TypeDef g_screenEditText;
extern GL_Page_TypeDef g_screenEditProgText;
extern GL_Page_TypeDef g_screenEditTagText;

// Screen creation routines (in screen_XYZ.c)
// ## Add creation routines for all screens here
void ScreenMainPSK_Create(void);
void ScreenMainCW_Create(void);
void ScreenMainSSB_Create(void);
void ScreenCalibrate_Create(void);
void ScreenCalibrationTest_Create(void);
void ScreenMode_Create(void);
void ScreenOptions_Create(void);
void ScreenAdvanced_Create(void);
void ScreenFilter_Create(void);
void ScreenRefreshFilter(int);
void ScreenFrequencies_Create(void);
void ScreenEditText_Create(void);
void ScreenEditTagText_Create(void);
void ScreenEditProgText_Create(void);
void Screen_TextDone(void);
void Screen_TagDone(void);
void Screen_ChangeButtonTextLabel(int i);
void Screen_ChangeButtonTagLabel(int i);
void Screen_ChangeButtonProgLabel(int i);
void Screen_ChangeButtonFreqLabel(int i);
//static void Screen_TextButtonClick(GL_PageControls_TypeDef* pThis);

// Screen access functions:
void Screen_CreateAllScreens(void);
void Screen_ShowScreen(GL_Page_TypeDef *pNewScreen);
void Screen_ShowMainScreen(void);
void Screen_ButtonAnimate(GL_PageControls_TypeDef* pThis);
void Screen_Update (void);

// void Screen_done(GL_PageControls_TypeDef* pThis);
void Screen_Done(void);
void Screen_FreqDone(void);

int Screen_GetFilterCodeID(void);
int Screen_GetFilterFreqID(void);
int Screen_GetScreenMode(void);
void Screen_SetScreenMode(int value);
void Screen_filter_Click(GL_PageControls_TypeDef* pThis);

void Screen_PSK_SetTune(void);

#endif
