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


// Screen global variables (defined in screen_all.c) for switching screens
// ## Add new screens here:
extern GL_Page_TypeDef g_screenMain;
extern GL_Page_TypeDef g_screenCalibrate;
extern GL_Page_TypeDef g_screenCalibrationTest;
extern GL_Page_TypeDef g_screenMode;
extern GL_Page_TypeDef g_screenOptions;


// Screen creation routines (in screen_XYZ.c)
// ## Add creation routines for all screens here
void ScreenMain_Create(void);
void ScreenCalibrate_Create(void);
void ScreenCalibrationTest_Create(void);
void ScreenMode_Create(void);
void ScreenOptions_Create(void);


// Screen access functions:
void Screen_CreateAllScreens(void);
void Screen_ShowScreen(GL_Page_TypeDef *pNewScreen);


#endif
