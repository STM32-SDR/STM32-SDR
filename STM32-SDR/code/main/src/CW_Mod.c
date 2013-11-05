/*
 * Code that handles the CW transmit routines
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

#include "stm32f4xx_rcc.h"
#include "stm32f4xx_gpio.h"
#include "xprintf.h"
#include <assert.h>

// Create a circular buffer used for reading/writing key samples.
// Need not match our sampling rate or the DMA rate. Just our data-storage area
#define KEY_SAMPLE_ARRAY_SIZE 256
static _Bool s_keySamples[KEY_SAMPLE_ARRAY_SIZE];
static int s_keySampleWriteIdx = 0;
static int s_keySampleReadIdx = 0;
static _Bool s_wantToTransmit = 0;

// Number samples required on key to accept a change in state.
#define DEBOUNCE_THRESHOLD 2

// CW Wave form generation constants
#define SLEW_RATE 0.005          // Desired slew rate for CW (in ms)
#define SAMPLE_PERIOD 0.000125   // DMA sampling period for each single sample.
#define CW_AMPLITUDE_RISE_SLOPE (SAMPLE_PERIOD / SLEW_RATE)
#define CW_AMPLITUDE_FALL_SLOPE (-1 * CW_AMPLITUDE_RISE_SLOPE)

// Handle return to RX timeout
#define RETURN_TO_RX_TIME_MS       500
#define RETURN_TO_RX_COUNTER_RESET (RETURN_TO_RX_TIME_MS * 55 / 65)
	// 55 = approximate number of ISRs per DMA ISR
	// 65 = approximate number of ms per DMA ISR


// Prototypes:
static void initCwTimerInterrupt(void);
static void initCwGPIO(void);
//void CW_TestRoutine(void);



void CW_Init(void)
{
	//CW_TestRoutine();

	initCwGPIO();
	initCwTimerInterrupt();
}

void initCwGPIO(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;

	/* Enable GPIOC clock */
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOC, ENABLE);
	/* Enable SYSCFG clock */
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_SYSCFG, ENABLE);

	/* Configure PC9 pin as input floating */
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9;
	GPIO_Init(GPIOC, &GPIO_InitStructure);
}

// Setup the timer interrupt
static void initCwTimerInterrupt(void)
{
	NVIC_InitTypeDef NVIC_InitStructure;

	/* Enable the TIM3 global Interrupt */
	NVIC_InitStructure.NVIC_IRQChannel = TIM3_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);

	/* TIM3 clock enable */
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3, ENABLE);

	/* Time base configuration */
	TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
	TIM_TimeBaseStructure.TIM_Period = 1000 - 1; // 1 MHz down to 1 KHz (1 ms)
	TIM_TimeBaseStructure.TIM_Prescaler = 84 - 1; // 24 MHz Clock down to 1 MHz (adjust per your clock)
	TIM_TimeBaseStructure.TIM_ClockDivision = 0;
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;
	TIM_TimeBaseInit(TIM3, &TIM_TimeBaseStructure);

	/* TIM IT enable */
	TIM_ITConfig(TIM3, TIM_IT_Update, ENABLE);

	/* TIM3 enable counter */
	TIM_Cmd(TIM3, ENABLE);
}


// Sampling
void writeKeySampleToKeyBuffer(char key)
{
	s_keySamples[s_keySampleWriteIdx] = key;
	s_keySampleWriteIdx = (s_keySampleWriteIdx + 1) % KEY_SAMPLE_ARRAY_SIZE;
}

// ISR which samples the key pin.
void CW_KeyPollTimerIRQ(void)
{
	if (TIM_GetITStatus(TIM3, TIM_IT_Update ) != RESET) {
		TIM_ClearITPendingBit(TIM3, TIM_IT_Update );

		// Read pin state
		_Bool isKeyPressed = GPIO_ReadInputDataBit(GPIOC, GPIO_Pin_9) == 0;

		// Debounce
		static _Bool debouncedIsKeyPressed = 0;
		static int debounceCounter = 0;
		if (isKeyPressed != debouncedIsKeyPressed) {
			debounceCounter++;
			if (debounceCounter >= DEBOUNCE_THRESHOLD) {
				debouncedIsKeyPressed = isKeyPressed;
				debounceCounter = 0;
			}
		} else {
			debounceCounter = 0;
		}

		// Handle if we "want" to transmit:
		static int returnToRxCounter = RETURN_TO_RX_COUNTER_RESET;
		if (debouncedIsKeyPressed) {
			s_wantToTransmit = 1;
			returnToRxCounter = RETURN_TO_RX_COUNTER_RESET;
		} else {
			if (returnToRxCounter) {
				returnToRxCounter --;
				if (returnToRxCounter == 0) {
					s_wantToTransmit = 0;
				}
			}
		}

		// Process current state
		writeKeySampleToKeyBuffer(debouncedIsKeyPressed);

		// DEBUG: Count interrupts
		extern volatile int g_numTimer3Interrupts;
		g_numTimer3Interrupts++;
	}
}


_Bool CW_DesiresTransmitMode(void)
{
	return s_wantToTransmit;
}

// Called in the DMA IRQ to get the desired amplitudes for each sample.
// Buffer to be filled with values between 0 and 1; I/Q encoding done
// by calling code.
void CW_FillTxAmplitudeBuffer(float amplitudeBuffer[], int bufferSize)
{
	// Record algorithm state between executions
	// ..amplitude percentage
	static float curAmplitude = 0.0;

	// Compute the scaling:
	int latestSampleIdx = (s_keySampleWriteIdx - 1 + KEY_SAMPLE_ARRAY_SIZE) % KEY_SAMPLE_ARRAY_SIZE;
	int numKeySamples = (latestSampleIdx - s_keySampleReadIdx + 1 + KEY_SAMPLE_ARRAY_SIZE) % KEY_SAMPLE_ARRAY_SIZE;
//	xprintf("#key=%d\n", numKeySamples);

	if (numKeySamples == 0) {
		assert(0);
		return;
	}

	// Use integer math to interpolate between key-sample points.
	int keyOffsetStep = numKeySamples; // in units of amplitude-buffer-samples.
	int keyOffsetAccumulator = 0;
	for (int ampBuffIdx = 0; ampBuffIdx < bufferSize; ampBuffIdx++) {

		// Get the target state:
		_Bool isKeyPressed = s_keySamples[s_keySampleReadIdx];

		// Handle ramping up/down:
		float amplitudeChange = isKeyPressed ? CW_AMPLITUDE_RISE_SLOPE : CW_AMPLITUDE_FALL_SLOPE;
		curAmplitude += amplitudeChange;
		if (curAmplitude > 1.0) {
			curAmplitude = 1.0;
		}
		if (curAmplitude < 0) {
			curAmplitude = 0;
		}

		// Store the value:
		amplitudeBuffer[ampBuffIdx] = curAmplitude;

		// Move to next key sample (if required)
		keyOffsetAccumulator += keyOffsetStep;
//		xprintf("Idx=%d, Acc=%d, Size=%d\n", s_keySampleReadIdx, keyOffsetAccumulator, bufferSize);
		s_keySampleReadIdx += (keyOffsetAccumulator / bufferSize);
		s_keySampleReadIdx %= KEY_SAMPLE_ARRAY_SIZE;
		keyOffsetAccumulator %= bufferSize;
	}

	// Ensure the above interpolation algorithm applied the expected number of steps:
	assert(s_keySampleReadIdx == (latestSampleIdx+ 1) % KEY_SAMPLE_ARRAY_SIZE);
}




/*
 * CW TESTING CODE
 */
#if 0
#define DMA_BUFFER_SIZE             512
#define KEY_SAMPLES_PER_DMA_BUFFER  55	// What hardware currently achieves.

#define SAMPLES_PER_DOT    10
#define SAMPLES_PER_DASH   (SAMPLES_PER_DOT * 3)
#define SAMPLES_BETWEEN_DOTDASH    SAMPLES_PER_DOT
#define SAMPLES_PER_SPACE  (SAMPLES_PER_DOT * 3)
#define SAMPLES_PER_SLASH  SAMPLES_PER_DOT // There are spaces on either size of a /, should be worth 7 dots, but minus 3*2 = 1
#define SAMPLE_AT_END      (KEY_SAMPLES_PER_DMA_BUFFER) // There are spaces on either size of a /, should be worth 7, but minus 3*2 = 1



void cwTestTx(_Bool isKeyDown, int sampleCount)
{
	static int numKeySamplesWritten = 0;

	for (int i = 0; i < sampleCount; i++) {
		// Insert this key data
		xprintf("%d,", isKeyDown);
		writeKeySampleToKeyBuffer(isKeyDown);
		numKeySamplesWritten++;

		// Simulate DMA buffer swap:
		if (numKeySamplesWritten  > 0 &&
			numKeySamplesWritten % KEY_SAMPLES_PER_DMA_BUFFER == 0)
		{
			float cwAmplitudes[DMA_BUFFER_SIZE];
			CW_FillTxAmplitudeBuffer(cwAmplitudes, DMA_BUFFER_SIZE);

			xprintf("\nDMA Buffer Amplitudes:\n");
			for (int i = 0; i < DMA_BUFFER_SIZE; i++) {
				int amp = cwAmplitudes[i] * 100;
				xprintf("%d,", amp);
			}

			xprintf("\n\nKey Samples:\n");

		}
	}
}

void CW_TestRoutine(void)
{
	xprintf("*********************************\n");
	xprintf("Beginning CW Testing....\n");
	xprintf("*********************************\n");
//	const char* message = "   --- ...";
	//const char* message = "   --- ... --- ... --- ... --- ... --- ... --- ... --- ... --- ... --- ... --- ...";
	const char* message = "   .... . .-.. .-.. --- / .-- --- .-. .-.. -.. / - .... .. ... / .. ... / .- / - . ... - / -- . ... ... .- --. . / .-- .... .. -.-. .... / .-- .. .-.. .-.. / -... . / -.-. --- -. ...- . .-. - . -.. / - --- / -- --- .-. ... . / -.-. --- -.. . .-.-.-";


	// Generate the message:
	_Bool wasDotOrDash = 0;
	for (int charIdx = 0; message[charIdx] != 0; charIdx++) {
		char nextChar = message[charIdx];

		_Bool isDotOrDash = (nextChar == '.' || nextChar == '-');
		if (wasDotOrDash && isDotOrDash) {
			cwTestTx(0, SAMPLES_BETWEEN_DOTDASH);
		}
		wasDotOrDash = isDotOrDash;

		switch (nextChar) {
		case '.':
			cwTestTx(1, SAMPLES_PER_DOT);
			break;
		case '-':
			cwTestTx(1, SAMPLES_PER_DASH);
			break;
		case ' ':
			cwTestTx(0, SAMPLES_PER_SPACE);
			break;
		case '/':
			cwTestTx(0, SAMPLES_PER_SLASH);
			break;
		default:
			assert(0);
		}
	}
	cwTestTx(0, SAMPLE_AT_END);

	xprintf("*********************************\n");
	xprintf("Done CW Testing.\n");
	xprintf("*********************************\n");

	// Don't operate otherwise we'll likely transmit part of the sample message!
	assert(0);

}
#endif

