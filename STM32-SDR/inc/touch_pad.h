
#include "stm32f4xx.h"

#define TOUCH_CS_PORT	 GPIOB
#define TOUCH_CS_PIN	 GPIO_Pin_12

uint16_t TouchFlag;
uint16_t	X_Point;
uint16_t	Y_Point;


void GetTouchPoint(void);
/* Private typedef -----------------------------------------------------------*/
typedef	struct POINT
{
   uint16_t x;
   uint16_t y;
}Coordinate;





/* Private define ------------------------------------------------------------*/
/* AD channel selection command and register */
#define	CHX 	0x90 	/* channel Y+ selection command */
#define	CHY 	0xd0	/* channel X+ selection command*/

#define TP_CS(x)	x ? GPIO_SetBits(GPIOB,GPIO_Pin_12): GPIO_ResetBits(GPIOB,GPIO_Pin_12)

#define TP_INT_IN   GPIO_ReadInputDataBit(GPIOD,GPIO_Pin_6)

/* Private function prototypes -----------------------------------------------*/
Coordinate *Read_Ads7846(void);
void TouchPanel_Calibrate(void);
void DrawCross(uint16_t Xpos,uint16_t Ypos);
void TP_DrawPoint(uint16_t Xpos,uint16_t Ypos);
void Draw_Touch_Point(uint16_t x,uint16_t y);
void touch_init(void);
void touch_interrupt_init(void);
