#include	"stm32f4xx_rcc.h"
#include	"stm32f4xx_exti.h"
#include	"stm32f4xx_syscfg.h"
#include 	"stm32f4xx_gpio.h"
#include	"stm32f4xx_tim.h"
#include	"arm_math.h"
#include	"SI570.h"
#include	"Init_I2C.h"
#include	"Init_Codec.h"
#include	"Encoders.h"
#include	"FrequencyManager.h"
#include	"options.h"
#include	"LcdHal.h"
#include	"ChangeOver.h"
#include	"xprintf.h"
#include	"screen_All.h"
#include	"Keyboard_Input.h"
#include	"stm32f4xx_it.h"

//E0 - Clock
#define CLOCK GPIO_Pin_0
//E1 - Data
#define DATA GPIO_Pin_1

unsigned char bData[16];
unsigned char idx=0;
unsigned char tmp=0;

unsigned char readByte(void);
unsigned char processByte(void);
unsigned char waitKey(void);
uint8_t keyhit(void);
unsigned char translate(unsigned char scancode);
extern void USR_KEYBRD_ProcessData(uint8_t pbuf, uint8_t d);

_Bool input(uint8_t pin){
	return GPIO_ReadInputDataBit(GPIOE, pin);
}

volatile uint8_t shiftSet = 0;

uint8_t kbBuffer[16];
uint8_t kbPos = 0;
uint8_t kbSize = 0;
uint8_t capsSet = 0;
volatile uint8_t extFlag = 0;
volatile uint8_t relFlag = 0;
volatile uint8_t key=0;

void configureGPIOps2(void)
{
	xprintf("Configure GPIO\n");
	GPIO_InitTypeDef GPIO_InitStructure;

	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOE, ENABLE);
	/* Configure PE0&1 pin as input with Pull Up */
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
	GPIO_InitStructure.GPIO_Pin = (DATA);

	GPIO_Init(GPIOE, &GPIO_InitStructure);

	TM_EXTI_Attach(GPIOE, CLOCK, 0x0C);
}

void kbInterrupt(void)
{
	unsigned char k;

	if (input(CLOCK) == 0) { //	Check for clock
	    k = processByte();
	    if(k != 0)
	        {
	            kbBuffer[kbPos] = k;
	            kbPos = ++kbPos & 0x0F;
	            kbSize++;
	        }
	    return;
//	    xprintf("%c", k);
	}
}

void ps2Read (void) {
	unsigned char data;
    if(keyhit()){
    	data = waitKey();
    	xprintf("%x %c ", data, data);
    	USR_KEYBRD_ProcessData(data, data);
    }
}

uint8_t keyhit(void)
{
//	xprintf("Test Key Hit\n");
    return kbSize != 0;
}

unsigned char waitKey(void)
{
//	xprintf("Wait for Key\n");
    unsigned char k = 0;
    while(kbSize == 0);
    k = kbBuffer[(kbPos - kbSize)&0x0F];
    kbSize--;
    return k;
}

void hi_clk_delay (void) {
	uint8_t t=0;
	while(input(CLOCK)==1 && t<253){
		Delay(10);
		t++;
	}
}

void lo_clk_delay (void) {
	uint8_t t=0;
	while(input(CLOCK)==0 && t<253){
		Delay(10);
		t++;
	}
}

unsigned char readByte(void) {
uint8_t i=0;
uint8_t tmp=0;

      for(i=0;i<16;i++)
           bData[i]=0;
      while(input(CLOCK)==0);         //start pulse

         for(i=1;i<11;i++){             // following 8 pulses, parity and stop pulse
            hi_clk_delay();
            bData[i]=input(DATA);
            while(input(CLOCK)==0);
            lo_clk_delay();
         }   //for i<8
         hi_clk_delay();         // Inner pulses delay

      for(i=1; i<9; i++) {
 //   	  xprintf("%x ", bData[i]);
          if(bData[i]!=0)
              tmp|=(1<<(i-1));
      }

 //     xprintf("%x", tmp);
      return tmp;

}   //readabyte()

static const unsigned char scantableAZ[]={
0x1C, 0x32, 0x21, 0x23, 0x24, 0x2B, 0x34, 0x33, 0x43, 0x3B, 0x42, 0x4B, 0x3A, // A-Z
0x31, 0x44, 0x4D, 0x15, 0x2D, 0x1B, 0x2C, 0x3C, 0x2A, 0x1D, 0x22, 0x35, 0x1A};


// Normal numeric scancodes, starting with ',' .
static const unsigned char scantable09n[]=
{
0x41, 0x4E, 0x49, 0x4A, // , - . /
0x45, 0x16, 0x1E, 0x26, 0x25, 0x2E, 0x36, 0x3D, 0x3E, 0x46,     //Digits 0-9
0x00, 0x4C, 0x00, 0x55 // 0 ; 0 =
};

// Shift scancodes, starting at '!'
static const unsigned char scantable09s[]=
{
    0x16, 0x52, 0x26, 0x25, 0x2E, 0x3D, 0x00, 0x46, 0x45, 0x3E, 0x55
};

// Normal misc. scancode map. Scancode, ASCII value
static const unsigned char scanmapn[]=
{0x54, '[', 0x5B, ']', 0x0E, '`', 0x5D, '\\', 0x52, '\''};

// Shifted misc. scancode map. Scancode, ASCII value
static const unsigned char scanmaps[]=
{0x1E, '@', 0x36, '^', 0x4E, '_', 0x54, '{', 0x5B, '}', 0x5D, '|',
0x4C, ':', 0x41, '<', 0x49, '>', 0x4A, '?', 0x0E, '~'};


// Scancode map independent of Shift
static const unsigned char scanmapx[]=
{0x29, ' ', 0x5A, '\r', 0x0D, '\t', 0x76, 27, 0x66, 0x08};

// Extended scancode map
static const unsigned char scanmape[]=
{
0x71, 0x08
};

// Extended scancode map
static const unsigned char scanmapSp1[]=
{ //0 - F9(88), 3 F5(84), 4 F3(82), 5 F1(80), 6 F2(81), 7 F12(8B), 9 F10(89), a F8(87), b F6(85), c F4(83)
0x00, 0x88, 0x00, 0x84, 0x82, 0x80, 0x81, 0x8b, 0x00, 0x89, 0x87, 0x85, 0x83
};

// Extended scancode map
static const unsigned char scanmapSp2[]=
{ //66 - BS(08), 6B left(A1), 72 down(A2), 74 right(A0), 75 up(A3), 78 F11(8A), 83 F7(86)
0x08, 0x00, 0x00, 0x00, 0x00, 0xA1, 0x00, 0x00, 0x00, 0x00,
0x00,0x00, 0xA2, 0x00, 0xA0, 0xA3, 0x00, 0x00, 0x8A, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x86
};

unsigned char translate(unsigned char scancode)
{
	xprintf ("%x ", scancode);
    int i=0;

    if(extFlag == 0)
    {
        for(i=0; i<10; i+=2)
            if(scanmapx[i] == scancode)
                return scanmapx[i+1];

        for(i=0; i<26; i++) {
            if(scantableAZ[i] == scancode){
                if(shiftSet ^ capsSet)
                    return i+65;
                else
                    return i+65 + 32;
            }
        }
        if(shiftSet)
        {
            for(i=0; i<11; i++)
                if(scantable09s[i] == scancode)
                    return i+'!';

            for(i=0; i<22; i+=2)
                if(scanmaps[i] == scancode)
                    return scanmaps[i+1];
        } else
        {
            for(i=0; i<18; i++)
                if(scantable09n[i] == scancode)
                    return i+',';

            for(i=0; i<10; i+=2)
                if(scanmapn[i] == scancode)
                    return scanmapn[i+1];
        }
    } else      // Extended keys
    {
        for(i=0; i<2; i+=2)
            if(scanmape[i] == scancode)
                return scanmape[i+1];
    }

    if (scancode <= 0xC)
    	return scanmapSp1[scancode];
    if (scancode >= 0x66 && scancode <= 0x83)
    	return scanmapSp2[scancode - 0x66];

    return 0x00;
}

unsigned char processByte(void)
{
    unsigned char i, j;
    i = readByte();
	xprintf ("\n%x ", i);

    if(i == 0xF0)       //A key is being released
    {
        relFlag = 1;
        return 0;
    }

    if(i == 0xE0)       // Extended key operation
    {
        extFlag = 1;
        relFlag = 0;    //0xE0 always first in sequence, OK to clear this
        return 0;
    }

    if(relFlag == 0)    // Pressing a key
    {
        if(extFlag == 0)    // Non-extended key pressed
        {
            if(i == 0x12 || i == 0x59)
            {
                shiftSet = 1;
                return 0;
            }

            if(i == 0x58)
            {
//                sendByte(0xED);
                if(capsSet == 0)
                {
                    capsSet = 1;
//                    sendByte(0x04);
                } else
                {
                    capsSet = 0;
//                    sendByte(0x00);
                }
                return 0;
            }

        }
        j = translate(i);
        extFlag = 0;
        return j;
    }

    if(relFlag == 1)    //Releasing a key
    {
        relFlag = 0;    //If a flag was set but not handled, we wouldn't have gotten here
        extFlag = 0;    //OK to clear this here

        if(extFlag == 0)   // Releasing a nonextended key
        {
            if(i == 0x12 || i == 0x59)
            {
                shiftSet = 0;
                return 0;
            }

        }
//        } else              // Releasing an extended key
//        {
//            return 0;
//        }

//        return 0;
    }
    return 0;
}
