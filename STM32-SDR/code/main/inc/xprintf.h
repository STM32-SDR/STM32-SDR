/*------------------------------------------------------------------------*/
/* Universal string handler for user console interface  (C)ChaN, 2011     */
/*------------------------------------------------------------------------*/

#ifndef _STRFUNC
#define _STRFUNC

#define _USE_XFUNC_OUT	1	/* 1: Use output functions */
#define	_CR_CRLF		1	/* 1: Convert \n ==> \r\n in the output char */

#define _USE_XFUNC_IN	0	/* 1: Use input function */
#define	_LINE_ECHO		0	/* 1: Echo back input chars in xgets function */

#if _USE_XFUNC_OUT
#define xdev_out(func) xfunc_out = (void(*)(unsigned char))(func)
extern void (*xfunc_out)(unsigned char);
void xputc(char c);
void xputs(const char* str);
void xfputs(void (*func)(unsigned char), const char* str);
void xprintf(const char* fmt, ...);
void xsprintf(char* buff, const char* fmt, ...);
void xfprintf(void (*func)(unsigned char), const char* fmt, ...);
void put_dump(const void* buff, unsigned long addr, int len, int width);
void debug (int debug_code,	const char* fmt, ... );
#define DW_CHAR		sizeof(char)
#define DW_SHORT	sizeof(short)
#define DW_LONG		sizeof(long)
#endif

#if _USE_XFUNC_IN
#define xdev_in(func) xfunc_in = (unsigned char(*)(void))(func)
extern unsigned char (*xfunc_in)(void);
int xgets (char* buff, int len);
int xfgets (unsigned char (*func)(void), char* buff, int len);
int xatoi (char** str, long* res);
#endif

#endif

//debug enabling codes are used to xprintf information to the console

#define NONE 0 // used to turn off debugging statements
#define GUI 1 // used to print high-level debug statement related to GUI
#define USB 2 // used to print high-level debug statement related to USB
#define CAL 4 // screen calibration
#define TOUCH 8 // touch screen
#define LCD 16 // LCD obviously!
#define ENCODER 32 // Encoders
#define KEYBOARD 64 // Keyboard entry
#define SCREEN 128 // Screen operations
#define INIT 256 // Startup initialization sequence
#define CONTROL 512 // Serial port command sequences
	// etc.


