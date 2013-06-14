/**
  ******************************************************************************
  * @file    touchscreen.c
  * @author  MCD Application Team
  * @version V2.0.0
  * @date    11-July-2011
  * @brief   Touchscreen Driver 
  ******************************************************************************
  * @attention
  *
  * THE PRESENT FIRMWARE WHICH IS FOR GUIDANCE ONLY AIMS AT PROVIDING CUSTOMERS
  * WITH CODING INFORMATION REGARDING THEIR PRODUCTS IN ORDER FOR THEM TO SAVE
  * TIME. AS A RESULT, STMICROELECTRONICS SHALL NOT BE HELD LIABLE FOR ANY
  * DIRECT, INDIRECT OR CONSEQUENTIAL DAMAGES WITH RESPECT TO ANY CLAIMS ARISING
  * FROM THE CONTENT OF SUCH FIRMWARE AND/OR THE USE MADE BY CUSTOMERS OF THE
  * CODING INFORMATION CONTAINED HEREIN IN CONNECTION WITH THEIR PRODUCTS.
  *
  * <h2><center>&copy; COPYRIGHT 2011 STMicroelectronics</center></h2>
  ******************************************************************************
  */

/* Includes ------------------------------------------------------------------*/
#include "touchscreen.h"
#include "graphicObject.h"
#include "TscHal.h"
#include "LcdHal.h"

/** @addtogroup Embedded_GUI_Library
  * @{
  */

/** @defgroup touchscreen 
  * @brief touchscreen main functions
  * @{
  */ 
/* External variables --------------------------------------------------------*/
/* Private typedef -----------------------------------------------------------*/
/* Private defines -----------------------------------------------------------*/
#define RESCALE_FACTOR 1000000
#define CalibrationDone_Offset  ((uint8_t)0 * sizeof(uint32_t))
#define A2_Offset               ((uint8_t)1 * sizeof(uint32_t))
#define B2_Offset               ((uint8_t)2 * sizeof(uint32_t))
#define C2_Offset               ((uint8_t)3 * sizeof(uint32_t))
#define D2_Offset               ((uint8_t)4 * sizeof(uint32_t))
#define E2_Offset               ((uint8_t)5 * sizeof(uint32_t))
#define F2_Offset               ((uint8_t)6 * sizeof(uint32_t)) 
/* Private macros ------------------------------------------------------------*/
#define TS_ReadCalibrationVaraible(offset)  (*(__IO uint32_t*)(CalibrationAddr + offset))
/* Private variables ---------------------------------------------------------*/
int32_t A2 = 0, B2 = 0, C2 = 0, D2 = 0, E2 = 0, F2 = 0;

/* Pointer to Penultimate Flash Memory Address */
uint32_t EndAddr = 0;
uint32_t CalibrationAddr = 0;
uint32_t TSC_Value_X;
uint32_t TSC_Value_Y;
  
/* Flash Memory Variables */
uint32_t EraseCounter = 0x00;
uint32_t FlashFree_Address = 0;
__IO uint32_t NbrOfPage = 0x00;
__IO TSC_FLASH_Status TSC_FlashStatus;
__IO TSC_Flash_TestStatus TSC_MemoryProgramStatus;
/* Private function prototypes -----------------------------------------------*/
static void TS_SaveCalibrationVariables(void);
/* Private functions ---------------------------------------------------------*/


/**
  * @brief  Calibrate TouchScreen coordinates by LCD touch in 5 points
  * @param  None
  * @retval None
  */
void TS_Calibration(void)
{
  uint32_t coordinate_X1a = 0, coordinate_X2a = 0, coordinate_X3a = 0, coordinate_X4a = 0, coordinate_X5a = 0;
  uint32_t coordinate_Y1a = 0, coordinate_Y2a = 0, coordinate_Y3a = 0, coordinate_Y4a = 0, coordinate_Y5a = 0;
  uint32_t coordinate_X1b = 0, coordinate_X2b = 0, coordinate_X3b = 0, coordinate_X4b = 0, coordinate_X5b = 0;
  uint32_t coordinate_Y1b = 0, coordinate_Y2b = 0, coordinate_Y3b = 0, coordinate_Y4b = 0, coordinate_Y5b = 0;
  uint32_t coordinate_X1 = 0, coordinate_X2 = 0, coordinate_X3 = 0, coordinate_X4 = 0, coordinate_X5 = 0;
  uint32_t coordinate_Y1 = 0, coordinate_Y2 = 0, coordinate_Y3 = 0, coordinate_Y4 = 0, coordinate_Y5 = 0;
  uint16_t Xd1 = (LCD_Width / 2), Xd2 = 1 * (LCD_Width / 5), Xd3 = 4 * (LCD_Width / 5), Xd4 = 4 * (LCD_Width / 5), Xd5 = 1 * (LCD_Width / 5);
  uint16_t Yd1 = (LCD_Height / 2), Yd2 = 1 * (LCD_Height / 5), Yd3 = 1 * (LCD_Height / 5), Yd4 = 4 * (LCD_Height / 5), Yd5 = 4 * (LCD_Height / 5);
  double A = 0.0, B = 0.0, C = 0.0, D = 0.0, E = 0.0, F = 0.0;
  double d = 0.0, dx1 = 0.0, dx2 = 0.0, dx3 = 0.0, dy1 = 0.0, dy2 = 0.0, dy3 = 0.0;
  uint32_t X2_1 = 0, X2_2 = 0, X2_3 = 0, X2_4 = 0, X2_5 = 0;
  uint32_t Y2_1 = 0, Y2_2 = 0, Y2_3 = 0, Y2_4 = 0, Y2_5 = 0;
  uint32_t XxY_1 = 0, XxY_2 = 0, XxY_3 = 0, XxY_4 = 0, XxY_5 = 0;
  uint32_t XxXd_1 = 0, XxXd_2 = 0, XxXd_3 = 0, XxXd_4 = 0, XxXd_5 = 0;
  uint32_t YxXd_1 = 0, YxXd_2 = 0, YxXd_3 = 0, YxXd_4 = 0, YxXd_5 = 0;
  uint32_t XxYd_1 = 0, XxYd_2 = 0, XxYd_3 = 0, XxYd_4 = 0, XxYd_5 = 0;
  uint32_t YxYd_1 = 0, YxYd_2 = 0, YxYd_3 = 0, YxYd_4 = 0, YxYd_5 = 0;
  uint32_t alfa = 0, beta = 0, chi = 0, Kx = 0, Ky = 0, Lx = 0, Ly = 0;
  uint16_t epsilon = 0, fi = 0, Mx = 0, My = 0;

  GL_SetBackColor(GL_White);
  GL_SetTextColor(GL_Black);
  GL_Clear(White);
  GL_DisplayAdjStringLine(3 * (LCD_Height / 7), LCD_Width - 25, "Run Calibration.", GL_FALSE);
  GL_Delay(40);
  GL_DisplayAdjStringLine(3 * (LCD_Height / 7), LCD_Width - 25, "Run Calibration..", GL_FALSE);
  GL_Delay(40);
  GL_DisplayAdjStringLine(3 * (LCD_Height / 7), LCD_Width - 25, "Run Calibration...", GL_FALSE);
  GL_Delay(30);
  touch_done = 0;

  GL_Clear(White);
  GL_Cross( (LCD_Height / 2), (LCD_Width / 2) ); /* Absolute Central Point */
  while ( touch_done == 0)
  {
    TSC_Read();
  }
  coordinate_X1a = TSC_Value_X;
  coordinate_Y1a = TSC_Value_Y;

  GL_Delay(90); /* This is to catch only one touch event */
  TSC_Init();
  touch_done = 0;

  GL_Clear(White);
  GL_Cross(1*(LCD_Height / 5), 1*(LCD_Width / 5)); /* Nord-East Corner point */
  while ( touch_done == 0)
  {
    TSC_Read();
  }
  coordinate_X2a = TSC_Value_X;
  coordinate_Y2a = TSC_Value_Y;

  GL_Delay(90); /* This is to catch only one touch event */
  TSC_Init();
  touch_done = 0;

  GL_Clear(White);
  GL_Cross(1*(LCD_Height / 5), 4*(LCD_Width / 5)); /* Nord-West Corner */
  while ( touch_done == 0)
  {
    TSC_Read();
  }
  coordinate_X3a = TSC_Value_X;
  coordinate_Y3a = TSC_Value_Y;

  GL_Delay(90); /* This is to catch only one touch event */
  TSC_Init();
  touch_done = 0;

  GL_Clear(White);
  GL_Cross(4*(LCD_Height / 5), 4*(LCD_Width / 5)); /* Sud-West Corner */
  while ( touch_done == 0)
  {
    TSC_Read();
  }
  coordinate_X4a = TSC_Value_X;
  coordinate_Y4a = TSC_Value_Y;

  GL_Delay(90); /* This is to catch only one touch event */
  TSC_Init();
  touch_done = 0;

  GL_Clear(White);
  GL_Cross(4*(LCD_Height / 5), 1*(LCD_Width / 5)); /* Sud-East Corner point */
  while ( touch_done == 0)
  {
    TSC_Read();
  }
  coordinate_X5a = TSC_Value_X;
  coordinate_Y5a = TSC_Value_Y;

  GL_Delay(90); /* This is to catch only one touch event */
  TSC_Init();
  touch_done = 0;

  GL_Clear(White);
  GL_Cross( (LCD_Height / 2), (LCD_Width / 2) ); /* Absolute Central Point */
  while ( touch_done == 0)
  {
    TSC_Read();
  }
  coordinate_X1b = TSC_Value_X;
  coordinate_Y1b = TSC_Value_Y;

  GL_Delay(90); /* This is to catch only one touch event */
  TSC_Init();
  touch_done = 0;

  GL_Clear(White);
  GL_Cross(1*(LCD_Height / 5), 1*(LCD_Width / 5)); /* Nord-East Corner point */
  while ( touch_done == 0)
  {
    TSC_Read();
  }
  coordinate_X2b = TSC_Value_X;
  coordinate_Y2b = TSC_Value_Y;

  GL_Delay(90); /* This is to catch only one touch event */
  TSC_Init();
  touch_done = 0;

  GL_Clear(White);
  GL_Cross(1*(LCD_Height / 5), 4*(LCD_Width / 5)); /* Nord-West Corner */
  while ( touch_done == 0)
  {
    TSC_Read();
  }
  coordinate_X3b = TSC_Value_X;
  coordinate_Y3b = TSC_Value_Y;

  GL_Delay(90); /* This is to catch only one touch event */
  TSC_Init();
  touch_done = 0;

  GL_Clear(White);
  GL_Cross(4*(LCD_Height / 5), 4*(LCD_Width / 5)); /* Sud-West Corner */
  while ( touch_done == 0)
  {
    TSC_Read();
  }
  coordinate_X4b = TSC_Value_X;
  coordinate_Y4b = TSC_Value_Y;

  GL_Delay(90); /* This is to catch only one touch event */
  TSC_Init();
  touch_done = 0;

  GL_Clear(White);
  GL_Cross(4*(LCD_Height / 5), 1*(LCD_Width / 5)); /* Sud-East Corner point */
  while ( touch_done == 0)
  {
    TSC_Read();
  }
  coordinate_X5b = TSC_Value_X;
  coordinate_Y5b = TSC_Value_Y;

  GL_Delay(90); /* This is to catch only one touch event */
  TSC_Init();
  touch_done = 0;

  /* Average between X and Y coupled Touchscreen values */
  coordinate_X1 = (coordinate_X1a + coordinate_X1b) / 2;
  coordinate_X2 = (coordinate_X2a + coordinate_X2b) / 2;
  coordinate_X3 = (coordinate_X3a + coordinate_X3b) / 2;
  coordinate_X4 = (coordinate_X4a + coordinate_X4b) / 2;
  coordinate_X5 = (coordinate_X5a + coordinate_X5b) / 2;

  coordinate_Y1 = (coordinate_Y1a + coordinate_Y1b) / 2;
  coordinate_Y2 = (coordinate_Y2a + coordinate_Y2b) / 2;
  coordinate_Y3 = (coordinate_Y3a + coordinate_Y3b) / 2;
  coordinate_Y4 = (coordinate_Y4a + coordinate_Y4b) / 2;
  coordinate_Y5 = (coordinate_Y5a + coordinate_Y5b) / 2;

  X2_1 = (coordinate_X1 * coordinate_X1);
  X2_2 = (coordinate_X2 * coordinate_X2);
  X2_3 = (coordinate_X3 * coordinate_X3);
  X2_4 = (coordinate_X4 * coordinate_X4);
  X2_5 = (coordinate_X5 * coordinate_X5);

  Y2_1 = (coordinate_Y1 * coordinate_Y1);
  Y2_2 = (coordinate_Y2 * coordinate_Y2);
  Y2_3 = (coordinate_Y3 * coordinate_Y3);
  Y2_4 = (coordinate_Y4 * coordinate_Y4);
  Y2_5 = (coordinate_Y5 * coordinate_Y5);

  XxY_1 = (coordinate_X1 * coordinate_Y1);
  XxY_2 = (coordinate_X2 * coordinate_Y2);
  XxY_3 = (coordinate_X3 * coordinate_Y3);
  XxY_4 = (coordinate_X4 * coordinate_Y4);
  XxY_5 = (coordinate_X5 * coordinate_Y5);

  XxXd_1 = ( coordinate_X1 * Xd1 );
  XxXd_2 = ( coordinate_X2 * Xd2 );
  XxXd_3 = ( coordinate_X3 * Xd3 );
  XxXd_4 = ( coordinate_X4 * Xd4 );
  XxXd_5 = ( coordinate_X5 * Xd5 );

  YxXd_1 = ( coordinate_Y1 * Xd1 );
  YxXd_2 = ( coordinate_Y2 * Xd2 );
  YxXd_3 = ( coordinate_Y3 * Xd3 );
  YxXd_4 = ( coordinate_Y4 * Xd4 );
  YxXd_5 = ( coordinate_Y5 * Xd5 );

  XxYd_1 = ( coordinate_X1 * Yd1 );
  XxYd_2 = ( coordinate_X2 * Yd2 );
  XxYd_3 = ( coordinate_X3 * Yd3 );
  XxYd_4 = ( coordinate_X4 * Yd4 );
  XxYd_5 = ( coordinate_X5 * Yd5 );

  YxYd_1 = ( coordinate_Y1 * Yd1 );
  YxYd_2 = ( coordinate_Y2 * Yd2 );
  YxYd_3 = ( coordinate_Y3 * Yd3 );
  YxYd_4 = ( coordinate_Y4 * Yd4 );
  YxYd_5 = ( coordinate_Y5 * Yd5 );

  alfa = X2_1 + X2_2 + X2_3 + X2_4 + X2_5;
  beta = Y2_1 + Y2_2 + Y2_3 + Y2_4 + Y2_5;
  chi = XxY_1 + XxY_2 + XxY_3 + XxY_4 + XxY_5;
  epsilon = coordinate_X1 + coordinate_X2 + coordinate_X3 + coordinate_X4 + coordinate_X5;
  fi = coordinate_Y1 + coordinate_Y2 + coordinate_Y3 + coordinate_Y4 + coordinate_Y5;
  Kx = XxXd_1 + XxXd_2 + XxXd_3 + XxXd_4 + XxXd_5;
  Ky = XxYd_1 + XxYd_2 + XxYd_3 + XxYd_4 + XxYd_5;
  Lx = YxXd_1 + YxXd_2 + YxXd_3 + YxXd_4 + YxXd_5;
  Ly = YxYd_1 + YxYd_2 + YxYd_3 + YxYd_4 + YxYd_5;
  Mx = Xd1 + Xd2 + Xd3 + Xd4 + Xd5;
  My = Yd1 + Yd2 + Yd3 + Yd4 + Yd5;

  d = 5 * ( ((double)alfa * beta) - ((double)chi * chi) ) + 2 * ((double)chi * epsilon * fi) - ((double)alfa * fi * fi ) - ( (double)beta * epsilon * epsilon );
  dx1 = 5 * ( ((double)Kx * beta) - ((double)Lx * chi) ) + ((double)fi * ( ((double)Lx * epsilon) - ((double)Kx * fi) )) + ((double)Mx * ( ((double)chi * fi) - ((double)beta * epsilon) ));
  dx2 = 5 * ( ((double)Lx * alfa) - ((double)Kx * chi) ) + ((double)epsilon * ( ((double)Kx * fi) - ((double)Lx * epsilon) )) + ((double)Mx * ( ((double)chi * epsilon) - ((double)alfa * fi) ));
  dx3 = ((double)Kx * ( ((double)chi * fi) - ((double)beta * epsilon) )) + ((double)Lx * ( ((double)chi * epsilon) - ((double)alfa * fi) )) + ((double)Mx * ( ((double)alfa * beta) - ((double)chi * chi) ));
  dy1 = 5 * ( ((double)Ky * beta) - ((double)Ly * chi) ) + ((double)fi * ( ((double)Ly * epsilon) - ((double)Ky * fi) )) + ((double)My * ( ((double)chi * fi) - ((double)beta * epsilon) ));
  dy2 = 5 * ( ((double)Ly * alfa) - ((double)Ky * chi) ) + ((double)epsilon * ( ((double)Ky * fi) - ((double)Ly * epsilon) )) + ((double)My * ( ((double)chi * epsilon) - ((double)alfa * fi) ));
  dy3 = ((double)Ky * ( ((double)chi * fi) - ((double)beta * epsilon) )) + ((double)Ly * ( ((double)chi * epsilon) - ((double)alfa * fi) )) + ((double)My * ( ((double)alfa * beta) - ((double)chi * chi) ));

  A = dx1 / d;
  B = dx2 / d;
  C = dx3 / d;
  D = dy1 / d;
  E = dy2 / d;
  F = dy3 / d;

  /* To avoid computation with "double" variables A, B, C, D, E, F, we use the s32 variables
     A2, B2, C2, D2, E2, F2, multiplied for a Scale Factor equal to 100000 to retain the precision*/
  A2 = (int32_t)(A * RESCALE_FACTOR);
  B2 = (int32_t)(B * RESCALE_FACTOR);
  C2 = (int32_t)(C * RESCALE_FACTOR);
  D2 = (int32_t)(D * RESCALE_FACTOR);
  E2 = (int32_t)(E * RESCALE_FACTOR);
  F2 = (int32_t)(F * RESCALE_FACTOR);

  GL_Clear(White);
  GL_DisplayAdjStringLine(3 * (LCD_Height / 7), 10 * (LCD_Width / 11), "Calibration done!", GL_FALSE);

  GL_Delay(25); /* Now show HOME Menu */
  GL_Clear(White);
  calibration_done = 1;

  TS_SaveCalibrationVariables();
}

/**
  * @brief  Save TouchScreen 5 points calibration coordinates
  * @param  None
  * @retval None
  */
static void TS_SaveCalibrationVariables(void)
{
  int32_t Data[7];
    /********************* FLASH PROGRAMMING FOR SAVING "calibration_done" variable **********************/
  TSC_FlashStatus = TSC_FLASH_COMPLETE;
  TSC_MemoryProgramStatus = PASSED;
  FlashFree_Address = CalibrationAddr;
  
  Data[0] = calibration_done;
  Data[1] = A2;
  Data[2] = B2;
  Data[3] = C2;
  Data[4] = D2;
  Data[5] = E2;
  Data[6] = F2;
  
  TSC_FlashStatus = TSC_WriteDataToNVM(FlashFree_Address, Data, sizeof(Data));

  /* Reading Calibration Flag from the Flash Memory */
  calibration_done = (*(__IO uint32_t*) FlashFree_Address) & 0x000000FF;
}

/**
  * @brief  Check if the calibration has been done, and if not, start it.
  *         Initialize the variables A2, B2, C2, D2, E2, F2 needed to
  *         calculate the LCD coordinate from the Touchscreen ones.
  * @param  None
  * @retval None
  */
void TS_CheckCalibration(void)
{

  /* Clear All pending flags */
#if defined(USE_STM3210C_EVAL) || defined(USE_STM32100E_EVAL)
  TSC_FLASH_ClearFlag( TSC_FLASH_FLAG_BSY | TSC_FLASH_FLAG_EOP | TSC_FLASH_FLAG_PGERR );
#endif

  /* Calculate the address of the Penultimate Flash Memory Page, where the calibration parameters will be saved. */
  /*  CalibrationAddr = (uint32_t)(EndAddr-0x404); */
  
  if ((TS_ReadCalibrationVaraible(CalibrationDone_Offset) & 0x000000FF) != 1)
  {
    TS_Calibration();
  }
  else
  {
    calibration_done = 1;
  }
  
  /* Reading Calibration Parameters from the Flash Memory */
  A2 =  ( TS_ReadCalibrationVaraible(A2_Offset) );
  B2 =  ( TS_ReadCalibrationVaraible(B2_Offset) );
  C2 =  ( TS_ReadCalibrationVaraible(C2_Offset) );
  D2 =  ( TS_ReadCalibrationVaraible(D2_Offset) );
  E2 =  ( TS_ReadCalibrationVaraible(E2_Offset) );
  F2 =  ( TS_ReadCalibrationVaraible(F2_Offset) );  
}

/**
  * @brief  Returns the Display y-axis Coordinate corresponding to Touch y-axis coordinate.
  *         X and Y are inverted because the display is meant in Landscape
  * @param  x_touch: X coordinate of the Touch Panel
  * @param  y_touch: Y coordinate of the Touch Panel
  * @retval uint16_t - Xd: Coordinate X of the LCD Panel (X-axis Coordinate)
  */
uint16_t getDisplayCoordinateX(uint16_t x_touch, uint16_t y_touch)
{
  uint16_t Xd;
  float temp;
  temp = (A2 * x_touch + B2 * y_touch + C2) / RESCALE_FACTOR;
  Xd = (uint16_t)(temp);
  if (Xd > 60000)
  {
    /* this to avoid negative value */
    Xd = 0;
  }
  return Xd;
}

/**
  * @brief  Returns the Display x-axis Coordinate corresponding to Touch x-axis coordinate.
  *         X and Y are inverted because the display is meant in Landscape
  * @param  x_touch: X coordinate of the Touch Panel
  * @param  y_touch: Y coordinate of the Touch Panel
  * @retval uint16_t - Yd: Coordinate X of the LCD Panel (Y-axis Coordinate)
  */
uint16_t getDisplayCoordinateY(uint16_t x_touch, uint16_t y_touch)
{
  uint16_t Yd;
  float temp;
  temp = (D2 * x_touch + E2 * y_touch + F2) / RESCALE_FACTOR;
  Yd = (uint16_t)(temp);
  if (Yd > 60000)
  {
    /*  this to avoid negative value */
    Yd = 0;
  }
  return Yd;
}

/**
  * @brief  Returns calibration status
  * @param  None
  * @retval SET is calibration done RESET if not done.
  */
FlagStatus TS_IsCalibrationDone(void)
{
  return (calibration_done == 1)? SET : RESET ;
}

/**
  * @}
  */

/**
  * @}
  */

/******************* (C) COPYRIGHT 2011 STMicroelectronics *****END OF FILE****/
