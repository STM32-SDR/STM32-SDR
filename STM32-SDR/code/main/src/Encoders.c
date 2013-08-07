// Manage the encoder knobs
#include	"stm32f4xx_rcc.h"
#include	"stm32f4xx_exti.h"
#include	"stm32f4xx_syscfg.h"
#include 	"stm32f4xx_gpio.h"
#include	"arm_math.h"
#include	"SI570.h"
#include	"Init_I2C.h"
#include	"Init_Codec.h"
#include	"Encoders.h"
#include	"FrequencyManager.h"
#include	"options.h"
#include	"LcdHal.h"
#include	"ChangeOver.h"


// Data for the encoder state.
const int16_t ENC_SENS = 2;
const int8_t ENCODER_STATES[] = {
		0, 1, -1, 0,
		-1, 0, 0, 1,
		1, 0, 0, -1,
		0, -1, 1, 0 };

typedef struct
{
	uint8_t old;
	int8_t dir0;
	int8_t dir1;

	GPIO_TypeDef *pPort;
	uint8_t lowPinNumber;
} EncoderStruct_t;
static EncoderStruct_t s_encoderStruct1 = {0, 0, 0, GPIOC, 5};
static EncoderStruct_t s_encoderStruct2 = {0, 0, 0, GPIOB, 4};

// Prototypes
static void configureGPIOEncoder1(void);
static void configureGPIOEncoder2(void);
static void configureEncoderInterrupt(void);
static void init_encoder1(void);
static void init_encoder2(void);
static int8_t calculateEncoderChange(EncoderStruct_t *pEncoder);
static void applyEncoderChange1(int8_t changeDirection);
static void applyEncoderChange2(int8_t changeDirection);


/* ----------------------
 * Initialization
 * ---------------------- */
void Encoders_Init(void)
{
	configureGPIOEncoder1();
	configureGPIOEncoder2();
	configureEncoderInterrupt();

	init_encoder1();
	init_encoder2();
}

static void configureGPIOEncoder1(void)
{
	EXTI_InitTypeDef EXTI_InitStructure;
	GPIO_InitTypeDef GPIO_InitStructure;

	/* Enable GPIOC clock */
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOC, ENABLE);
	/* Enable SYSCFG clock */
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_SYSCFG, ENABLE);

	/* Configure PC5,6&8 pin as input with Pull Up */
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
	GPIO_InitStructure.GPIO_Pin = (GPIO_Pin_5 | GPIO_Pin_6 | GPIO_Pin_8 );
	GPIO_Init(GPIOC, &GPIO_InitStructure);

	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOE, ENABLE);
	/* Configure PE3,4,5&6 pin as input with Pull Up */
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
	GPIO_InitStructure.GPIO_Pin = (GPIO_Pin_3 | GPIO_Pin_4 | GPIO_Pin_5
			| GPIO_Pin_6 );
	GPIO_Init(GPIOE, &GPIO_InitStructure);

	/* Enable SYSCFG clock */
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_SYSCFG, ENABLE);
	/* Connect EXTI Line8 to PA8 pin */
	SYSCFG_EXTILineConfig(EXTI_PortSourceGPIOC, EXTI_PinSource8 );

	/* Configure EXTI Line8 */
	EXTI_InitStructure.EXTI_Line = EXTI_Line8;
	EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;
	EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Falling;
	EXTI_InitStructure.EXTI_LineCmd = ENABLE;
	EXTI_Init(&EXTI_InitStructure);

}
static void configureGPIOEncoder2(void)
{
	EXTI_InitTypeDef EXTI_InitStructure;
	GPIO_InitTypeDef GPIO_InitStructure;

	/* Enable GPIOC clock */
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOC, ENABLE);
	/* Enable SYSCFG clock */
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_SYSCFG, ENABLE);

	/* Configure PC1,2,3&4 pin as input with Pull Up */
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
	GPIO_InitStructure.GPIO_Pin = (GPIO_Pin_1 | GPIO_Pin_2 | GPIO_Pin_3	| GPIO_Pin_4 );
	GPIO_Init(GPIOC, &GPIO_InitStructure);

	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB, ENABLE);
	/* Configure PB3,4,5&7 pin as input with Pull Up */
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
	GPIO_InitStructure.GPIO_Pin = (GPIO_Pin_4 | GPIO_Pin_5 | GPIO_Pin_7 );
	GPIO_Init(GPIOB, &GPIO_InitStructure);

	/* Enable SYSCFG clock */
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_SYSCFG, ENABLE);
	/* Connect EXTI Line 7 to PA7 pin */
	SYSCFG_EXTILineConfig(EXTI_PortSourceGPIOB, EXTI_PinSource7 );

	/* Configure EXTI Line7 */
	EXTI_InitStructure.EXTI_Line = EXTI_Line7;
	EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;
	EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Falling;
	EXTI_InitStructure.EXTI_LineCmd = ENABLE;
	EXTI_Init(&EXTI_InitStructure);
}
static void configureEncoderInterrupt(void)
{
	NVIC_InitTypeDef NVIC_InitStructure;

	/* Enable and set EXTI Line9-5 Interrupt to the lowest priority */
	NVIC_InitStructure.NVIC_IRQChannel = EXTI9_5_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0x01;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0x01;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);
}

void init_encoder1(void)
{
	// TODO: Display the SI570 error in a better way.
	if (SI570_Chk == 3) {
		LCD_StringLine(234, 60, " SI570_?? ");
	}
	else {
		Compute_FXTAL();
	}
}
void init_encoder2(void)
{
	Receive_Sequence();
}



/*
 * Process Encoder Changes
 */

void Encoders_CalculateAndProcessChanges(void)
{
	// TODO:- Is delay needed for encoder? Should it be doubled (as would have been before encoder refactor)?
	Delay(100);

	int change = 0;
	change = calculateEncoderChange(&s_encoderStruct1);
	applyEncoderChange1(change);

	change = calculateEncoderChange(&s_encoderStruct2);
	applyEncoderChange2(change);
}

static int8_t calculateEncoderChange(EncoderStruct_t *pEncoder)
{
	// Encoder motion has been detected--determine direction
	Delay(100);

	// Remember previous encoder state and add current state (2 bits)
	pEncoder->old <<= 2;
	pEncoder->old |= (GPIO_ReadInputData(pEncoder->pPort) >> pEncoder->lowPinNumber & 0x03);

	int8_t tempDir = ENCODER_STATES[(pEncoder->old & 0x0F)];
	pEncoder->dir0 += tempDir;
	if ((pEncoder->dir0 < ENC_SENS) && (pEncoder->dir0 > -ENC_SENS))
		pEncoder->dir1 = 0;
	if (pEncoder->dir0 >= ENC_SENS) {
		pEncoder->dir1 = 1;
		pEncoder->dir0 = 0;
	}
	if (pEncoder->dir0 <= -ENC_SENS) {
		pEncoder->dir1 = -1;
		pEncoder->dir0 = 0;
	}

	return pEncoder->dir1;
}

static void applyEncoderChange1(int8_t changeDirection)
{
	// Check for no change
	if (changeDirection == 0) {
		return;
	}

	_Bool isEncoderPressedIn = !GPIO_ReadInputDataBit(GPIOC, GPIO_Pin_8);

	// Are we pressed in?
	if (isEncoderPressedIn) {
		if (changeDirection > 0) {
			FrequencyManager_DecreaseFreqStepSize();
		} else {
			FrequencyManager_IncreaseFreqStepSize();
		}
	}
	else {
		if (changeDirection > 0) {
			FrequencyManager_StepFrequencyUp();
		} else {
			FrequencyManager_StepFrequencyDown();
		}
	}
}
static void applyEncoderChange2(int8_t changeDirection)
{
	// Check for no change
	if (changeDirection == 0) {
		return;
	}

	/*
	 * Check the limits
	 */
	int curOptIdx = Options_GetSelectedOption();
	int16_t currentValue = Options_GetValue(curOptIdx);
	int16_t newValue = currentValue + Options_GetChangeRate(curOptIdx) * changeDirection;
	int16_t minValue = Options_GetMinimum(curOptIdx);
	int16_t maxValue = Options_GetMaximum(curOptIdx);
	if (newValue < minValue) {
		newValue = minValue;
	}
	if (newValue > maxValue) {
		newValue = maxValue;
	}

	// Set the value & Display it
	Options_SetValue(curOptIdx, newValue);
}
