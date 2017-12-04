/*------------------------------------------------------------------------*/
/* Universal string handler for user console interface  (C)ChaN, 2011     */
/*------------------------------------------------------------------------*/

#define _USE_YFUNC_OUT	1	/* 1: Use output functions */
#define	_CR_CRLF		1	/* 1: Convert \n ==> \r\n in the output char */

#define _USE_YFUNC_IN	0	/* 1: Use input function */
#define	_LINE_ECHO		0	/* 1: Echo back input chars in xgets function */

#if _USE_YFUNC_OUT
#define ydev_out(func) yfunc_out = (void(*)(unsigned char))(func)
extern void (*yfunc_out)(unsigned char);
void yputc(char c);
void yputs(const char* str);
void yfputs(void (*func)(unsigned char), const char* str);
void yprintf(const char* fmt, ...);
void ysprintf(char* buff, const char* fmt, ...);
void yfprintf(void (*func)(unsigned char), const char* fmt, ...);
void yput_dump(const void* buff, unsigned long addr, int len, int width);
#define DW_CHAR		sizeof(char)
#define DW_SHORT	sizeof(short)
#define DW_LONG		sizeof(long)
#endif

#if _USE_YFUNC_IN
#define ydev_in(func) yfunc_in = (unsigned char(*)(void))(func)
extern unsigned char (*yfunc_in)(void);
int ygets (char* buff, int len);
int yfgets (unsigned char (*func)(void), char* buff, int len);
int yatoi (char** str, long* res);
#endif


