#include <stdlib.h>
#include "LcdDriver_ILI9320.h"


// *********************************************************************************
// Register Settings for LCD
// *********************************************************************************
#define LCD_REG      (*((volatile unsigned short *) 0x60000000))
#define LCD_RAM      (*((volatile unsigned short *) 0x60020000))


// Work with the direct LCD RAM access
#define R3_CMALWAYS_SET	  0x1000
#define R3_VERTICAL_BIT   5		// Set 0 for Increment, 1 for decrement
#define R3_HORIZONTAL_BIT 4		// Set 0 for Increment, 1 for decrement
#define R3_ADDRESS_BIT    3		// Set 1 for vertical, 0 for horizontal

#define R3_INCRCEMENT     1		// For X and Y directions.
#define R3_DECREMENT      0
#define R3_CHANGEADDR_X   0		// Does address change in X or Y first?
#define R3_CHANGEADDR_Y   1


#if LCD_ROTATION == 0
#define LCD_ROTATE_GETX(x,y) (x)
#define LCD_ROTATE_GETY(x,y) (y)
#define BUILD_R3_CMD(horizontalIncDec, verticalIncDec, addressDir) \
		(R3_CMALWAYS_SET | (horizontalIncDec << R3_VERTICAL_BIT) | (verticalIncDec << R3_HORIZONTAL_BIT) | (addressDir << R3_ADDRESS_BIT))
#elif LCD_ROTATION == 90
#define LCD_ROTATE_GETX(x,y) (y)
#define LCD_ROTATE_GETY(x,y) (ACTUAL_SCREEN_HEIGHT - (x) - 1)
#define BUILD_R3_CMD(horizontalIncDec, verticalIncDec, addressDir) \
		((R3_CMALWAYS_SET) | (!(verticalIncDec) << R3_VERTICAL_BIT) | ((horizontalIncDec) << R3_HORIZONTAL_BIT) | (!(addressDir) << R3_ADDRESS_BIT))
#elif LCD_ROTATION == 180
#define LCD_ROTATE_GETX(x,y) (ACTUAL_SCREEN_WIDTH - (x) - 1)
#define LCD_ROTATE_GETY(x,y) (ACTUAL_SCREEN_HEIGHT - (y) - 1)
#define BUILD_R3_CMD(horizontalIncDec, verticalIncDec, addressDir) \
		((R3_CMALWAYS_SET) | (!(horizontalIncDec) << R3_VERTICAL_BIT) | (!(verticalIncDec) << R3_HORIZONTAL_BIT) | ((addressDir) << R3_ADDRESS_BIT))
#elif LCD_ROTATION == 270
#define LCD_ROTATE_GETX(x,y) (ACTUAL_SCREEN_WIDTH - (y) - 1)
#define LCD_ROTATE_GETY(x,y) (x)
#define BUILD_R3_CMD(horizontalIncDec, verticalIncDec, addressDir) \
		((R3_CMALWAYS_SET) | ((verticalIncDec) << R3_VERTICAL_BIT) | (!(horizontalIncDec) << R3_HORIZONTAL_BIT) | (!(addressDir) << R3_ADDRESS_BIT))
#else
#error Must define LCD_ROTATION to be a direction
#endif


void assert(_Bool condition)
{
	volatile int bugger = 0;
	if (!condition) {
		while (1)
			bugger ++;
	}
}



//////////////////////////////////////////////////////////////////////////////////////////////////////
//
//
//		From LcdHal.c
//
//
//////////////////////////////////////////////////////////////////////////////////////////////////////
//#include "gl_fonts.h"
#include "stm32_eval.h"


LCD_Direction_TypeDef LCD_Direction = _0_degree;
__IO uint32_t LCDType = LCD_ILI9320;



// FROM TFT_DISPLAY (top stuff)
__IO uint16_t LCD_textColor = 0x0000;
__IO uint16_t LCD_backColor = 0xFFFF;





//#if defined (USE_STM3210E_EVAL) || defined (USE_STM32100E_EVAL) || defined (USE_STM322xG_EVAL)
/**
 * @brief  Writes 1 word to the LCD RAM.
 * @param  RGB_Code: the pixel color in RGB mode (5-6-5).
 * @retval None
 */
void LCD_WriteRAMWord(uint16_t RGB_Code)
{
	LCD_WriteRAM_Prepare();
	LCD_WriteRAM(RGB_Code);
}
//#endif /* USE_STM3210E_EVAL */

/**
 * @brief  LCD_Change_Direction
 * @param  Direction: The Drawing Direction
 *         This parameter can be one of the following values:
 *     @arg  _0_degree
 *     @arg  _90_degree
 *     @arg  _180_degree
 *     @arg  _270_degree
 * @retval None
 */
void LCD_Change_Direction(LCD_Direction_TypeDef Direction)
{
	LCD_Direction = Direction;

	if (LCD_Direction == _0_degree) {
		/* Set GRAM write direction and BGR = 1 */
		/* I/D=01 (Horizontal : increment, Vertical : decrement) */
		/* AM=1 (address is updated in vertical writing direction) */
		LCD_WriteReg(R3, MAKE_R3_CMD(0x1018));
	}
	else if (LCD_Direction == _90_degree) {
		/* Set GRAM write direction and BGR = 1 */
		/* I/D=11 (Horizontal : increment, Vertical : increment) */
		/* AM=0 (address is updated in horizontal writing direction) */
		LCD_WriteReg(R3, MAKE_R3_CMD(0x1030));
	}
	else if (LCD_Direction == _180_degree) {
		/* Set GRAM write direction and BGR = 1 */
		/* I/D=10 (Horizontal : decrement, Vertical : increment) */
		/* AM=1 (address is updated in vertical writing direction) */
		LCD_WriteReg(R3, MAKE_R3_CMD(0x1028));
	}
	else if (LCD_Direction == _270_degree) {
		/* Set GRAM write direction and BGR = 1 */
		/* I/D=00 (Horizontal : decrement, Vertical : decrement) */
		/* AM=0 (address is updated in horizontal writing direction) */
		LCD_WriteReg(R3, MAKE_R3_CMD(0x1000));
	}
}


#if 0
/**
 * @brief  LCD_DrawMonoBMP
 * @param  *Pict:   The pointer to the image
 * @param  Xpos_Init: The X axis position
 * @param  Ypos_Init: The Y axis position
 * @param  Height:    The Height of the image
 * @param  Width:     The Width of the image
 * @retval None
 */
void LCD_DrawMonoBMP(const uint8_t *Pict, uint16_t Xpos_Init, uint16_t Ypos_Init, uint16_t Height, uint16_t Width)
{
	int32_t index = 0, counter = 0;

	if (LCD_Direction == _0_degree) {
		LCD_SetDisplayWindow(Xpos_Init, Ypos_Init, Height, Width);
		/* Set GRAM write direction and BGR = 1 */
		/* I/D=00 (Horizontal : decrement, Vertical : decrement) */
		/* AM=1 (address is updated in vertical writing direction) */
		LCD_WriteReg(R3, MAKE_R3_CMD(0x1008));
	}
	else if (LCD_Direction == _90_degree) {
		LCD_SetDisplayWindow(Xpos_Init + Width - 1, Ypos_Init, Width, Height);
		/* Set GRAM write direction and BGR = 1 */
		/* I/D=01 (Horizontal : increment, Vertical : decrement) */
		/* AM=0 (address is updated in horizontal writing direction) */
		LCD_WriteReg(R3, MAKE_R3_CMD(0x1010));
	}
	else if (LCD_Direction == _180_degree) {
		LCD_SetDisplayWindow(Xpos_Init + Height - 1, Ypos_Init + Width - 1, Height, Width);
		/* Set GRAM write direction and BGR = 1 */
		/* I/D=11 (Horizontal : increment, Vertical : increment) */
		/* AM=1 (address is updated in vertical writing direction) */
		LCD_WriteReg(R3, MAKE_R3_CMD(0x1038));
	}
	else if (LCD_Direction == _270_degree) {
		LCD_SetDisplayWindow(Xpos_Init, Ypos_Init + Height - 1, Width, Height);
		/* Set GRAM write direction and BGR = 1 */
		/* I/D=10 (Horizontal : decrement, Vertical : increment) */
		/* AM=0 (address is updated in horizontal writing direction) */
		LCD_WriteReg(R3, MAKE_R3_CMD(0x1020));
	}

	LCD_SetCursor(Xpos_Init, Ypos_Init);

	if ((LCDType == LCD_ILI9320) || (LCDType == LCD_SPFD5408)) {
		/* Prepare to write GRAM */
		LCD_WriteRAM_Prepare();
	}

	for (index = 0; index < (Height * Width) / 8; index++) {
		for (counter = 7; counter >= 0; counter--) {
			if ((Pict[index] & (1 << counter)) == 0x00) {
				LCD_WriteRAM(LCD_backColor);
			}
			else {
				LCD_WriteRAM(LCD_textColor);
			}
		}
	}

	if ((LCDType == LCD_ILI9320) || (LCDType == LCD_SPFD5408)) {
		if (pLcdHwParam.LCD_Connection_Mode == GL_SPI) {
			GL_LCD_CtrlLinesWrite(pLcdHwParam.LCD_Ctrl_Port_NCS, pLcdHwParam.LCD_Ctrl_Pin_NCS, GL_HIGH);
		}
	}
	LCD_Change_Direction(LCD_Direction);
	GL_SetDisplayWindow(LCD_Width - 1, LCD_Height - 1, LCD_Height, LCD_Width);
}
#endif

#if 0
/**
 * @brief  Fill area with color.
 * @param  maxX: Maximum X coordinate
 * @param  minX: Minimum X coordinate
 * @param  maxY: Maximum Y coordinate
 * @param  minY: Minimum Y coordinate
 * @param  ptrBitmap: pointer to the image
 * @retval None
 */
void LCD_FillArea(uint16_t Xpos_Init, uint16_t Ypos_Init, uint16_t Height, uint16_t Width, uint16_t color)
{
	uint32_t area = 0;
	uint32_t index = 0;

	area = Width * Height;
	if (LCD_Direction == _0_degree) {
		GL_SetDisplayWindow(Xpos_Init, Ypos_Init, Height, Width);
		/* Set GRAM write direction and BGR = 1 */
		/* I/D=00 (Horizontal : decrement, Vertical : decrement) */
		/* AM=1 (address is updated in vertical writing direction) */
		LCD_WriteReg(R3, MAKE_R3_CMD(0x1008));
	}
	else if (LCD_Direction == _90_degree) {
		GL_SetDisplayWindow(Xpos_Init + Width - 1, Ypos_Init, Width, Height);
		/* Set GRAM write direction and BGR = 1 */
		/* I/D=01 (Horizontal : increment, Vertical : decrement) */
		/* AM=0 (address is updated in horizontal writing direction) */
		LCD_WriteReg(R3, MAKE_R3_CMD(0x1010));
	}
	else if (LCD_Direction == _180_degree) {
		GL_SetDisplayWindow(Xpos_Init + Height - 1, Ypos_Init + Width - 1, Height, Width);
		/* Set GRAM write direction and BGR = 1 */
		/* I/D=11 (Horizontal : increment, Vertical : increment) */
		/* AM=1 (address is updated in vertical writing direction) */
		LCD_WriteReg(R3, MAKE_R3_CMD(0x1038));
	}
	else if (LCD_Direction == _270_degree) {
		GL_SetDisplayWindow(Xpos_Init, Ypos_Init + Height - 1, Width, Height);
		/* Set GRAM write direction and BGR = 1 */
		/* I/D=10 (Horizontal : decrement, Vertical : increment) */
		/* AM=0 (address is updated in horizontal writing direction) */
		LCD_WriteReg(R3, MAKE_R3_CMD(0x1020));
	}

	LCD_SetCursor(Xpos_Init, Ypos_Init);

	if ((LCDType == LCD_ILI9320) || (LCDType == LCD_SPFD5408)) {
		LCD_WriteRAM_Prepare(); /* Prepare to write GRAM */
	}

	for (index = 0; index < area; index++) {
		LCD_WriteRAM(color);
	}

	if ((LCDType == LCD_ILI9320) || (LCDType == LCD_SPFD5408)) {
		if (pLcdHwParam.LCD_Connection_Mode == GL_SPI) {
			GL_LCD_CtrlLinesWrite(pLcdHwParam.LCD_Ctrl_Port_NCS, pLcdHwParam.LCD_Ctrl_Pin_NCS, GL_HIGH);
		}
	}
	LCD_Change_Direction(LCD_Direction);
	GL_SetDisplayWindow(LCD_Width - 1, LCD_Height - 1, LCD_Height, LCD_Width);
}
#endif

/**
 * @brief  LCD_DrawColorBMP
 * @param  *ptrBitmap:   The pointer to the image
 * @param  Xpos_Init: The X axis position
 * @param  Ypos_Init: The Y axis position
 * @param  Height:    The Height of the image
 * @param  Width:     The Width of the image
 * @retval None
 */
void LCD_DrawColorBMP(uint8_t* ptrBitmap, uint16_t Xpos_Init, uint16_t Ypos_Init, uint16_t Height, uint16_t Width)
{
#if 0
	uint32_t uDataAddr = 0, uBmpSize = 0;
	uint16_t uBmpData = 0;

	BmpBuffer32BitRead(&uBmpSize, ptrBitmap + 2);
	BmpBuffer32BitRead(&uDataAddr, ptrBitmap + 10);

	if (LCD_Direction == _0_degree) {
		GL_SetDisplayWindow(Xpos_Init, Ypos_Init, Height, Width);
		/* Set GRAM write direction and BGR = 1 */
		/* I/D=00 (Horizontal : decrement, Vertical : decrement) */
		/* AM=1 (address is updated in vertical writing direction) */
		LCD_WriteReg(R3, MAKE_R3_CMD(0x1008));
	}
	else if (LCD_Direction == _90_degree) {
		GL_SetDisplayWindow(Xpos_Init + Width - 1, Ypos_Init, Width, Height);
		/* Set GRAM write direction and BGR = 1 */
		/* I/D=01 (Horizontal : increment, Vertical : decrement) */
		/* AM=0 (address is updated in horizontal writing direction) */
		LCD_WriteReg(R3, MAKE_R3_CMD(0x1010));
	}
	else if (LCD_Direction == _180_degree) {
		GL_SetDisplayWindow(Xpos_Init + Height - 1, Ypos_Init + Width - 1, Height, Width);
		/* Set GRAM write direction and BGR = 1 */
		/* I/D=11 (Horizontal : increment, Vertical : increment) */
		/* AM=1 (address is updated in vertical writing direction) */
		LCD_WriteReg(R3, MAKE_R3_CMD(0x1038));
	}
	else if (LCD_Direction == _270_degree) {
		GL_SetDisplayWindow(Xpos_Init, Ypos_Init + Height - 1, Width, Height);
		/* Set GRAM write direction and BGR = 1 */
		/* I/D=10 (Horizontal : decrement, Vertical : increment) */
		/* AM=0 (address is updated in horizontal writing direction) */
		LCD_WriteReg(R3, MAKE_R3_CMD(0x1020));
	}

	LCD_SetCursor(Xpos_Init, Ypos_Init);

	if ((LCDType == LCD_ILI9320) || (LCDType == LCD_SPFD5408)) {
		/* Prepare to write GRAM */
		LCD_WriteRAM_Prepare();
	}

	/* Read bitmap data and write them to LCD */
	for (; uDataAddr < uBmpSize; uDataAddr += 2) {
		uBmpData = (uint16_t) (*(ptrBitmap + uDataAddr)) + (uint16_t) ((*(ptrBitmap + uDataAddr + 1)) << 8);
		LCD_WriteRAM(uBmpData);
	}
	if ((LCDType == LCD_ILI9320) || (LCDType == LCD_SPFD5408)) {
		if (pLcdHwParam.LCD_Connection_Mode == GL_SPI) {
			GL_LCD_CtrlLinesWrite(pLcdHwParam.LCD_Ctrl_Port_NCS, pLcdHwParam.LCD_Ctrl_Pin_NCS, GL_HIGH);
		}
	}
	LCD_Change_Direction(LCD_Direction);
	GL_SetDisplayWindow(LCD_Width - 1, LCD_Height - 1, LCD_Height, LCD_Width);
#endif
}




//////////////////////////////////////////////////////////////////////////////////////////////////////
//
//
//		From TFT_Display.c
//
//
//////////////////////////////////////////////////////////////////////////////////////////////////////



// ***********************************************************************************8
// Low level access routines
// ***********************************************************************************8
void LCD_WriteRAM_Prepare(void)
{
	LCD_REG = 0x22;
}

uint16_t LCD_ReadReg(uint8_t LCD_Reg)
{
	// TODO: Not sure if this works; copied from a different file (stm32100e_eval_lcd.c).
	/* Write 16-bit Index (then Read Reg) */
	LCD_REG = LCD_Reg;
	/* Read 16-bit Reg */
	uint16_t regValue = LCD_RAM;
	regValue = LCD_RAM;
	regValue = LCD_RAM;
	regValue = LCD_RAM;
	regValue = LCD_RAM;
	regValue = LCD_RAM;
	regValue = LCD_RAM;
	regValue = LCD_RAM;
	regValue = LCD_RAM;
	return regValue;
}

/**
  * @brief  Reads the LCD RAM.
  * @param  None
  * @retval uint16_t - LCD RAM Value.
  */
uint16_t LCD_ReadRAM(void)
{
    return LCD_ReadReg(R34);
}

typedef enum
{
	LCD_WriteRAMDir_Right = 0,
	LCD_WriteRAMDir_Left,
	LCD_WriteRAMDir_Up,
	LCD_WriteRAMDir_Down
} LCD_WriteRAM_Direction;


void LCD_WriteRAM_PrepareDir(LCD_WriteRAM_Direction direction)
{
	uint16_t r3Cmd = 0;

	switch (direction) {
	case LCD_WriteRAMDir_Right:
		/* Set GRAM write direction and BGR = 1 */
		/* I/D=00 (Horizontal : increment, Vertical : increment) */
		/* AM=1 (address is updated in horizontal writing direction) */
		//		r3Cmd = MAKE_R3_CMD(0x1030);
		r3Cmd = BUILD_R3_CMD(R3_INCRCEMENT, R3_INCRCEMENT, R3_CHANGEADDR_X);
		break;
	case LCD_WriteRAMDir_Left:
		/* Set GRAM write direction and BGR = 1 */
		/* I/D=01 (Horizontal : decrement, Vertical : increment) */
		/* AM=0 (address is updated in horizontal writing direction) */
		//		r3Cmd = MAKE_R3_CMD(0x1020);
		r3Cmd = BUILD_R3_CMD(R3_DECREMENT, R3_INCRCEMENT, R3_CHANGEADDR_X);
		break;
	case LCD_WriteRAMDir_Up:
		/* Set GRAM write direction and BGR = 1 */
		/* I/D=00 (Horizontal : increment, Vertical : decrement) */
		/* AM=1 (address is updated in vertical writing direction) */
		//		r3Cmd = MAKE_R3_CMD(0x1018);
		r3Cmd = BUILD_R3_CMD(R3_INCRCEMENT, R3_DECREMENT, R3_CHANGEADDR_Y);
		break;
	case LCD_WriteRAMDir_Down:
		/* Set GRAM write direction and BGR = 1 */
		/* I/D=00 (Horizontal : increment, Vertical : increment) */
		/* AM=1 (address is updated in vertical writing direction) */
		r3Cmd = MAKE_R3_CMD(0x1038);
		r3Cmd = BUILD_R3_CMD(R3_INCRCEMENT, R3_INCRCEMENT, R3_CHANGEADDR_Y);
		break;
	default:
		assert(0);
	}
	LCD_WriteReg(R3, r3Cmd);

	LCD_WriteRAM_Prepare();
}

void LCD_WriteRAM(uint16_t RGB_Code)
{
	LCD_RAM = RGB_Code;

	// TODO: Why is this delay needed? Without it, nothing appears on LCD!??
	extern void TFT_Delay(uint32_t nTime);
	//	TFT_Delay(50);
	TFT_Delay(10);
}

void LCD_WriteReg(uint8_t LCD_Reg, uint16_t LCD_RegValue)
{
	LCD_REG = LCD_Reg;
	LCD_RAM = LCD_RegValue;
}

/**
 * @brief  Sets a display window used for RAM writes.
 * @param  x: specifies the X top left corner.
 * @param  y: specifies the Y top left corner.
 * @param  Height: display window height.
 * @param  Width: display window width.
 * @retval None
 */
void LCD_SetDisplayWindow(uint8_t x, uint16_t y, uint16_t Height, uint16_t Width)
{
	uint16_t rotatedY1 = LCD_ROTATE_GETY(x, y);
	uint16_t rotatedY2 = LCD_ROTATE_GETY(x + Width - 1, y + Height - 1);
	uint16_t rotatedX1 = LCD_ROTATE_GETX(x, y);
	uint16_t rotatedX2 = LCD_ROTATE_GETX(x + Width - 1, y + Height - 1);

#define MAX(a, b) ((a)>(b)?(a):(b))
#define MIN(a, b) ((a)<(b)?(a):(b))

	LCD_WriteReg(0x0050, MIN(rotatedX1, rotatedX2)); // Window X start address
	LCD_WriteReg(0x0051, MAX(rotatedX1, rotatedX2)); // Window X end address
	LCD_WriteReg(0x0052, MIN(rotatedY1, rotatedY2)); // Window Y start address
	LCD_WriteReg(0x0053, MAX(rotatedY1, rotatedY2)); // Window Y end address

	LCD_SetCursor(x, y);
}
void LCD_ResetDisplayWindow(void)
{
	LCD_SetDisplayWindow(0, 0, LCD_Height, LCD_Width);
}


void LCD_SetCursor(uint16_t Xpos, uint16_t Ypos)
{
	LCD_WriteReg(0x20, LCD_ROTATE_GETX(Xpos, Ypos));
	LCD_WriteReg(0x21, LCD_ROTATE_GETY(Xpos, Ypos));
}


void LCD_DisplayOn(void)
{
	LCD_WriteReg(0x07, 0x0173);
}

void LCD_DisplayOff(void)
{
	LCD_WriteReg(0x07, 0x0000);
}



// Draw a 16-bit bitmap to the screen.
// Image Generation Tip:
// 	Use Gimp, load image, File --> Export, type C source code,
// 	"Save as RGB454 (16-bit)"
void LCD_DrawBMP16Bit(int x, int y, int height, int width, uint16_t* pBitmap, _Bool revByteOrder)
{
	int numPixels = width * height;

	LCD_SetDisplayWindow(x, y, height, width);
	LCD_WriteRAM_PrepareDir(LCD_WriteRAMDir_Right);

	// Push bitmap data to screen.
	for (int wordNum = 0; wordNum < numPixels; wordNum++) {
		uint16_t color = (*(pBitmap + wordNum));
		if (revByteOrder) {
			color = ((color & 0x00FF) << 8) | ((color & 0xFF00) >> 8);
		}
		LCD_WriteRAM( color );
	}
	LCD_ResetDisplayWindow();
}

// Draw a 1-bit bitmap (monochrome) to the screen.
// Assume each row of bits starts on a new byte (i.e., pad last byte per row with 0 bits)
void LCD_DrawBMP1Bit(int x, int y, int height, int width, uint8_t* pBitmap, _Bool revBitOrder)
{
	LCD_SetDisplayWindow(x, y, height, width);
	LCD_WriteRAM_PrepareDir(LCD_WriteRAMDir_Right);

	// Push bitmap data to screen.
	int byte = 0;
	for (int row = 0; row < height; row++) {
		for (int col = 0; col < width; col++) {
			// Setup the byte:
			if (col % 8 == 0 && col > 0) {
				byte++;
			}

			int bit = col % 8;
			if (revBitOrder) {
				bit = 7 - bit;
			}

			uint8_t turnOn = pBitmap[byte] & (1 << bit);
			if (turnOn) {
				LCD_WriteRAM( LCD_textColor );
			} else {
				LCD_WriteRAM( LCD_backColor );
			}
		}
		// Assume next row starts on new byte:
		byte++;
	}
	LCD_ResetDisplayWindow();
}

// ***********************************************************************************
// Drawing Routines
// ***********************************************************************************
void LCD_SetTextColor(__IO uint16_t Color)
{
	LCD_textColor = Color;
}

void LCD_SetBackColor(__IO uint16_t Color)
{
	LCD_backColor = Color;
}

void LCD_Clear(uint16_t Color)
{
	LCD_ResetDisplayWindow();
	LCD_WriteRAM_PrepareDir(LCD_WriteRAMDir_Right);

	for (uint16_t y = 0; y < LCD_Height; y++) {
		for (uint16_t x = 0; x < LCD_Width; x++) {
			LCD_WriteRAM(Color);
		}
	}
}

void LCD_DrawLine(uint16_t Xpos, uint16_t Ypos, uint16_t Length, uint8_t Direction)
{
	LCD_SetCursor(Xpos, Ypos);
	switch (Direction) {
	case LCD_DIR_HORIZONTAL: LCD_WriteRAM_PrepareDir(LCD_WriteRAMDir_Right); break;
	case LCD_DIR_VERTICAL:   LCD_WriteRAM_PrepareDir(LCD_WriteRAMDir_Down); break;
	default: assert(0);
	}

	for (uint16_t i = 0; i < Length; i++) {
		LCD_WriteRAM(LCD_textColor);
	}
}

void LCD_DrawRect(uint16_t Xpos, uint16_t Ypos, uint8_t Height, uint16_t Width)
{
	LCD_DrawLine(Xpos, Ypos, Width, LCD_DIR_HORIZONTAL);
	LCD_DrawLine(Xpos, Ypos + Height - 1, Width, LCD_DIR_HORIZONTAL);
	LCD_DrawLine(Xpos, Ypos, Height, LCD_DIR_VERTICAL);
	LCD_DrawLine(Xpos + Width - 1, Ypos, Height, LCD_DIR_VERTICAL);
}




/**
 * @brief  Draw one pixel at position given by Xpos, Ypos of color Color.
 * @param  Xpos: specifies X position
 * @param  Ypos: specifies Y position
 * @param  Color: RGB color of point
 * @retval None
 */
void LCD_PutPixel(uint16_t Xpos, uint16_t Ypos, uint16_t Color)
{
	LCD_SetCursor(Xpos, Ypos);
	LCD_WriteRAM_Prepare();
	LCD_WriteRAM(Color);
}

/**
 * @brief  Get color of pixel located at appropriate position
 * @param  Xpos: specifies X position
 * @param  Ypos: specifies Y position
 * @retval uint16_t - RGB color of required pixel.
 */
uint16_t LCD_GetPixel(uint16_t Xpos, uint16_t Ypos)
{
	uint16_t tmpColor = 0, tmpRed = 0, tmpBlue = 0;

	/*Read the color of given pixel*/
	LCD_WriteRAM_PrepareDir(LCD_WriteRAMDir_Right);
	LCD_SetCursor(Xpos, Ypos);
//	tmpColor = LCD_ReadRAM();

	uint16_t regValue = LCD_RAM;
	regValue = LCD_RAM;
	regValue = LCD_RAM;
	regValue = LCD_RAM;
	regValue = LCD_RAM;
	regValue = LCD_RAM;
	regValue = LCD_RAM;
	regValue = LCD_RAM;
	regValue = LCD_RAM;

	tmpColor = regValue;

	/*Swap the R and B color channels*/
	tmpRed = (tmpColor & Blue) << 11;
	tmpBlue = (tmpColor & Red) >> 11;
	tmpColor &= ~(Red | Blue);
	tmpColor |= (tmpRed | tmpBlue);

	return tmpColor;
}


//////////////////////////////////////////////////////////////////////////////////////////////////////
//		Test functions
//////////////////////////////////////////////////////////////////////////////////////////////////////
void LCD_TestDisplayScreen(void)
{
	// Test clear screen
	LCD_Clear(Blue);
	LCD_Clear(Red);
	LCD_Clear(Green);
	LCD_Clear(Yellow);
	LCD_Clear(Black);

	// Test horizontal and vertical lines
	// Lines from origin (top-left)
	LCD_SetTextColor(White);
	LCD_DrawLine(0,0, 100, LCD_DIR_HORIZONTAL);
	LCD_DrawLine(0,10, 100, LCD_DIR_HORIZONTAL);
	LCD_SetTextColor(Yellow);
	LCD_DrawLine(0,0, 100, LCD_DIR_VERTICAL);

	// Lines into bottom right corner.
	LCD_SetTextColor(Red);
	LCD_DrawLine(LCD_Width-200-1,LCD_Height-1, 200, LCD_DIR_HORIZONTAL);
	LCD_SetTextColor(Green);
	LCD_DrawLine(LCD_Width-1,LCD_Height-50-1, 50, LCD_DIR_VERTICAL);

	// Draw + in middle of screen
	int midX = LCD_Width / 2;
	int midY = LCD_Height / 2;
	LCD_SetBackColor(Blue);
	uint8_t data[] = {
			0xFF, 0x00, 0xCC, 0x33, 0xAA, 0x55,
			0xFF, 0x00, 0xCC, 0x33, 0xAA, 0x55,
			0xFF, 0x00, 0xCC, 0x33, 0xAA, 0x55,
			0xFF, 0x00, 0xCC, 0x33, 0xAA, 0x55,
			0xFF, 0x00, 0xCC, 0x33, 0xAA, 0x55,
	};
	LCD_SetTextColor(White);
	LCD_DrawBMP1Bit(midX, midY, 5, 48, data, 0);


	// Test DrawRect
	LCD_SetTextColor(Red);
	LCD_DrawRect(10, 50, 150, 50);
	LCD_SetTextColor(Green);
	LCD_DrawRect(60, 50, 150, 50);
	LCD_SetTextColor(Blue);
	LCD_DrawRect(110, 50, 150, 50);

	// Test Display Off/On
	LCD_DisplayOff();
	LCD_DisplayOn();

	// Test drawing a bitmap
	extern const struct {
		unsigned int 	 width;
		unsigned int 	 height;
		unsigned int 	 bytes_per_pixel; /* 2:RGB16, 3:RGB, 4:RGBA */
		char         	*comment;
		unsigned char	 pixel_data[320 * 240 * 2 + 1];
	} gimp_image;
	LCD_DrawBMP16Bit(0,0, gimp_image.height, gimp_image.width, (uint16_t*) gimp_image.pixel_data, 0);

	// Test put/get pixel
	LCD_PutPixel(10, 10, 0x1234);
	assert(LCD_GetPixel(10, 10) == 0x1234);
	LCD_PutPixel(10, 10, Red);
	assert(LCD_GetPixel(10, 10) == Red);
	LCD_PutPixel(10, 10, Green);
	assert(LCD_GetPixel(10, 10) == Green);
	LCD_PutPixel(10, 10, Blue);
	assert(LCD_GetPixel(10, 10) == Blue);

	volatile int a = 0;
	while (1)
		a ++;
}
