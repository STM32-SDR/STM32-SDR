//===================================================================
//	Si570 Subroutines	2/16/2008
//	Written by John H. Fisher - K5JHF
//===================================================================

//===================================================================

#define	twoto28th		268435456.0
#define	FOSC_MIN		4850000000.0
#define	FOSC_MIN_long	485000000
#define	FOSC_MAX		5670000000.0
#define	FOSC_MAX_long	567000000
#define SI570_Addr		0x55<<1

//===================================================================
void Read_Si570_registers(unsigned char read[]);
void Write_Si570_registers(unsigned char write[]);
void Recall_F0(void);
void Set_HS_DIV_N1(long Freq_long);
void Pack_Si570_registers(unsigned char reg[]);
void Unpack_Si570_registers(unsigned char reg[]);
void Compute_FXTAL(void);
void Output_Frequency(long Freq_Out);
void Si570_StartUp(void);
char Large_RFREQ_Change(void);
void Check_SI570(void);

//===================================================================
//long		FOUT_multiplier, FOUT_divisor, Display_multiplier, Display_divisor;
unsigned char si570_read[6];
unsigned const char HS_DIV_VALUE_char[6];
unsigned long RFREQ_INT, RFREQ_FRAC;
float HS_DIV, N1;
double RFREQ, Old_RFREQ, FXTAL;
long FOUT;  //, delta_freq;
double F0;
unsigned char SI570_Chk;
//===================================================================

