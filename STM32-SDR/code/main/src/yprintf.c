/*------------------------------------------------------------------------/
 /  Universal string handler for user console interface
 /-------------------------------------------------------------------------/
 /
 /  Copyright (C) 2011, ChaN, all right reserved.
 /
 / * This software is a free software and there is NO WARRANTY.
 / * No restriction on use. You can use, modify and redistribute it for
 /   personal, non-profit or commercial products UNDER YOUR RESPONSIBILITY.
 / * Redistributions of source code must retain the above copyright notice.
 /
 /-------------------------------------------------------------------------*/

#include "yprintf.h"

#if _USE_YFUNC_OUT
#include <stdarg.h>
void (*yfunc_out)(unsigned char); /* Pointer to the output stream */
static char *outptr;
static void yvprintf(const char* fmt, /* Pointer to the format string */
		va_list arp /* Pointer to arguments */
);

/*----------------------------------------------*/
/* Put a character                              */
/*----------------------------------------------*/

void yputc(char c)
{
	if (_CR_CRLF && c == '\n')
		yputc('\r'); /* CR -> CRLF */

	if (outptr) {
		*outptr++ = (unsigned char) c;
		return;
	}

	if (yfunc_out)
		yfunc_out((unsigned char) c);
}

/*----------------------------------------------*/
/* Put a null-terminated string                 */
/*----------------------------------------------*/

void yputs( /* Put a string to the default device */
		const char* str /* Pointer to the string */
)
{
	while (*str)
		yputc(*str++);
}

void yfputs( /* Put a string to the specified device */
		void (*func)(unsigned char), /* Pointer to the output function */
		const char* str /* Pointer to the string */
)
{
	void (*pf)(unsigned char);

	pf = yfunc_out; /* Save current output device */
	yfunc_out = func; /* Switch output to specified device */
	while (*str) /* Put the string */
		yputc(*str++);
	yfunc_out = pf; /* Restore output device */
}

/*----------------------------------------------*/
/* Formatted string output                      */
/*----------------------------------------------*/
/*  xprintf("%d", 1234);			"1234"
 xprintf("%6d,%3d%%", -200, 5);	"  -200,  5%"
 xprintf("%-6u", 100);			"100   "
 xprintf("%ld", 12345678L);		"12345678"
 xprintf("%04x", 0xA3);			"00a3"
 xprintf("%08LX", 0x123ABC);		"00123ABC"
 xprintf("%016b", 0x550F);		"0101010100001111"
 xprintf("%s", "String");		"String"
 xprintf("%-4s", "abc");			"abc "
 xprintf("%4s", "abc");			" abc"
 xprintf("%c", 'a');				"a"
 xprintf("%f", 10.0);            <xprintf lacks floating point support>
 */

static
void yvprintf(const char* fmt, /* Pointer to the format string */
		va_list arp /* Pointer to arguments */
)
{
	unsigned int r, i, j, w, f;
	unsigned long v;
	char s[16], c, d, *p;

	for (;;) {
		c = *fmt++; /* Get a char */
		if (!c)
			break; /* End of format? */
		if (c != '%') { /* Pass through it if not a % sequense */
			yputc(c);
			continue;
		}
		f = 0;
		c = *fmt++; /* Get first char of the sequense */
		if (c == '0') { /* Flag: '0' padded */
			f = 1;
			c = *fmt++;
		}
		else {
			if (c == '-') { /* Flag: left justified */
				f = 2;
				c = *fmt++;
			}
		}
		for (w = 0; c >= '0' && c <= '9'; c = *fmt++) /* Minimum width */
			w = w * 10 + c - '0';
		if (c == 'l' || c == 'L') { /* Prefix: Size is long int */
			f |= 4;
			c = *fmt++;
		}
		if (!c)
			break; /* End of format? */
		d = c;
		if (d >= 'a')
			d -= 0x20;
		switch (d) { /* Type is... */
		case 'S': /* String */
			p = va_arg(arp, char*);
			for (j = 0; p[j]; j++)
				;
			while (!(f & 2) && j++ < w)
				yputc(' ');
			yputs(p);
			while (j++ < w)
				yputc(' ');
			continue;
		case 'C': /* Character */
			yputc((char) va_arg(arp, int));
			continue;
		case 'B': /* Binary */
			r = 2;
			break;
		case 'O': /* Octal */
			r = 8;
			break;
		case 'D': /* Signed decimal */
		case 'U': /* Unsigned decimal */
			r = 10;
			break;
		case 'X': /* Hexdecimal */
			r = 16;
			break;
		default: /* Unknown type (passthrough) */
			yputc(c);
			continue;
		}

		/* Get an argument and put it in numeral */
		v = (f & 4) ?
				va_arg(arp, long) :
				((d == 'D') ?
						(long) va_arg(arp, int) :
						(long) va_arg(arp, unsigned int) );
		if (d == 'D' && (v & 0x80000000)) {
			v = 0 - v;
			f |= 8;
		}
		i = 0;
		do {
			d = (char) (v % r);
			v /= r;
			if (d > 9)
				d += (c == 'x') ? 0x27 : 0x07;
			s[i++] = d + '0';
		} while (v && i < sizeof(s));
		if (f & 8)
			s[i++] = '-';
		j = i;
		d = (f & 1) ? '0' : ' ';
		while (!(f & 2) && j++ < w)
			yputc(d);
		do
			yputc(s[--i]);
		while (i);
		while (j++ < w)
			yputc(' ');
	}
}

void yprintf( /* Put a formatted string to the default device */
		const char* fmt, /* Pointer to the format string */
		... /* Optional arguments */
)
{
	va_list arp;

	va_start(arp, fmt);
	yvprintf(fmt, arp);
	va_end(arp);
}

void ysprintf( /* Put a formatted string to the memory */
		char* buff, /* Pointer to the output buffer */
		const char* fmt, /* Pointer to the format string */
		... /* Optional arguments */
)
{
	va_list arp;

	outptr = buff; /* Switch destination for memory */

	va_start(arp, fmt);
	yvprintf(fmt, arp);
	va_end(arp);

	*outptr = 0; /* Terminate output string with a \0 */
	outptr = 0; /* Switch destination for device */
}

void yfprintf( /* Put a formatted string to the specified device */
		void (*func)(unsigned char), /* Pointer to the output function */
		const char* fmt, /* Pointer to the format string */
		... /* Optional arguments */
)
{
	va_list arp;
	void (*pf)(unsigned char);

	pf = yfunc_out; /* Save current output device */
	yfunc_out = func; /* Switch output to specified device */

	va_start(arp, fmt);
	yvprintf(fmt, arp);
	va_end(arp);

	yfunc_out = pf; /* Restore output device */
}

/*----------------------------------------------*/
/* Dump a line of binary dump                   */
/*----------------------------------------------*/

void yput_dump(const void* buff, /* Pointer to the array to be dumped */
		unsigned long addr, /* Heading address value */
		int len, /* Number of items to be dumped */
		int width /* Size of the items (DF_CHAR, DF_SHORT, DF_LONG) */
)
{
	int i;
	const unsigned char *bp;
	const unsigned short *sp;
	const unsigned long *lp;

	yprintf("%08lX ", addr); /* address */

	switch (width) {
	case DW_CHAR:
		bp = buff;
		for (i = 0; i < len; i++) /* Hexdecimal dump */
			yprintf(" %02X", bp[i]);
		yputc(' ');
		for (i = 0; i < len; i++) /* ASCII dump */
			yputc((bp[i] >= ' ' && bp[i] <= '~') ? bp[i] : '.');
		break;
	case DW_SHORT:
		sp = buff;
		do /* Hexdecimal dump */
			yprintf(" %04X", *sp++);
		while (--len);
		break;
	case DW_LONG:
		lp = buff;
		do /* Hexdecimal dump */
			yprintf(" %08LX", *lp++);
		while (--len);
		break;
	}

	yputc('\n');
}

#endif /* _USE_YFUNC_OUT */

#if _USE_YFUNC_IN
unsigned char (*yfunc_in)(void); /* Pointer to the input stream */

/*----------------------------------------------*/
/* Get a line from the input                    */
/*----------------------------------------------*/

int ygets ( /* 0:End of stream, 1:A line arrived */
		char* buff, /* Pointer to the buffer */
		int len /* Buffer length */
)
{
	int c, i;

	if (!yfunc_in) return 0; /* No input function specified */

	i = 0;
	for (;;) {
		c = yfunc_in(); /* Get a char from the incoming stream */
		if (!c) return 0; /* End of stream? */
		if (c == '\r') break; /* End of line? */
		if (c == '\b' && i) { /* Back space? */
			i--;
			if (_LINE_ECHO) yputc(c);
			continue;
		}
		if (c >= ' ' && i < len - 1) { /* Visible chars */
			buff[i++] = c;
			if (_LINE_ECHO) yputc(c);
		}
	}
	buff[i] = 0; /* Terminate with a \0 */
	if (_LINE_ECHO) yputc('\n');
	return 1;
}

int yfgets ( /* 0:End of stream, 1:A line arrived */
		unsigned char (*func)(void), /* Pointer to the input stream function */
		char* buff, /* Pointer to the buffer */
		int len /* Buffer length */
)
{
	unsigned char (*pf)(void);
	int n;

	pf = yfunc_in; /* Save current input device */
	yfunc_in = func; /* Switch input to specified device */
	n = ygets(buff, len); /* Get a line */
	yfunc_in = pf; /* Restore input device */

	return n;
}

/*----------------------------------------------*/
/* Get a value of the string                    */
/*----------------------------------------------*/
/*	"123 -5   0x3ff 0b1111 0377  w "
 ^                           1st call returns 123 and next ptr
 ^                        2nd call returns -5 and next ptr
 ^                3rd call returns 1023 and next ptr
 ^         4th call returns 15 and next ptr
 ^    5th call returns 255 and next ptr
 ^ 6th call fails and returns 0
 */

int yatoi ( /* 0:Failed, 1:Successful */
		char **str, /* Pointer to pointer to the string */
		long *res /* Pointer to the valiable to store the value */
)
{
	unsigned long val;
	unsigned char c, r, s = 0;

	*res = 0;

	while ((c = **str) == ' ') (*str)++; /* Skip leading spaces */

	if (c == '-') { /* negative? */
		s = 1;
		c = *(++(*str));
	}

	if (c == '0') {
		c = *(++(*str));
		switch (c) {
		case 'x': /* hexdecimal */
			r = 16; c = *(++(*str));
			break;
		case 'b': /* binary */
			r = 2; c = *(++(*str));
			break;
		default:
			if (c <= ' ') return 1; /* single zero */
			if (c < '0' || c > '9') return 0; /* invalid char */
			r = 8; /* octal */
		}
	}
	else {
		if (c < '0' || c > '9') return 0; /* EOL or invalid char */
		r = 10; /* decimal */
	}

	val = 0;
	while (c > ' ') {
		if (c >= 'a') c -= 0x20;
		c -= '0';
		if (c >= 17) {
			c -= 7;
			if (c <= 9) return 0; /* invalid char */
		}
		if (c >= r) return 0; /* invalid char for current radix */
		val = val * r + c;
		c = *(++(*str));
	}
	if (s) val = 0 - val; /* apply sign if needed */

	*res = val;
	return 1;
}

#endif /* _USE_YFUNC_IN */
