//===================================================================
//	Si570 Subroutines	3/1/2008
//	Written by John H. Fisher - K5JHF
//===================================================================


#include "Si570.h"
#include "Init_I2C.h"
#include "TFT_Display.h" //Included for putting up testing info


//===================================================================

//long						FOUT_multiplier, FOUT_divisor, Display_multiplier, Display_divisor;

unsigned			char	si570_read [ 6 ];

unsigned 			char 	SI570_Chk;

unsigned	const	char	HS_DIV_VALUE_char [ 6 ] = { 4,5,6,7,9,11 };

unsigned	const	char	N1_VALUE_char	  [ 2 ] = { 2,1 };

unsigned			long	RFREQ_INT,	RFREQ_FRAC ;

					float	HS_DIV,	N1;

					double	RFREQ,	Old_RFREQ,	FXTAL;

					long	FOUT;  //, delta_freq;
			
					double	F0;

					//long	offset;
	
/*======================================================================*/
void	Si570_StartUp ( void )		{

		F0 = 56320000.0;
		FOUT = 14070000*4;
		FOUT = 14058000*4;
		Compute_FXTAL();
		Output_Frequency ( FOUT );
	}


 
void	Output_Frequency ( long Freq_Out )	{

	int Disp4;
	double Disp5;
	int Disp6;


		if ( Freq_Out < 0 ) Freq_Out	=	-Freq_Out;

		Set_HS_DIV_N1 ( Freq_Out );

		RFREQ	=	( ( double ) Freq_Out ) * HS_DIV * N1 / FXTAL;

	
		Disp4 = (int)RFREQ;
		Disp5 = RFREQ - (double)Disp4;
		Disp6 = (int)(Disp5*10000.0);
		//Plot_Integer ( Disp4, 100, 40 );
		//Plot_Integer ( Disp6, 100, 60 );
	
		Pack_Si570_registers ( si570_read );

		Write_Si570_registers ( si570_read );

		Old_RFREQ	=	RFREQ;

	}

/*======================================================================*/

void	Compute_FXTAL ( void )	{

	double Disp0;
	int Disp1;
	double Disp2;
	int Disp3;

		Recall_F0 ();

		Read_Si570_registers ( si570_read );

		Unpack_Si570_registers ( si570_read );

		FXTAL	=	F0 * HS_DIV * N1 / RFREQ;
	
		Disp0 = FXTAL/1000000.0;
		Disp1 = (int)Disp0;
		Disp2 = Disp0 - (double)Disp1;
		Disp3 = (int)(Disp2*10000.0);
		//Plot_Integer ( Disp1, 100, 0 );
		//Plot_Integer ( Disp3, 100, 20 );
		}


/*======================================================================*/



/*======================================================================*/

void	Recall_F0 ( void )	{

	unsigned char i;

		i = I2C_ReadSlave ( SI570_Addr, 135 );

		I2C_WriteRegister( SI570_Addr, 135, i | 0x01 );	//	Recall NVM Freq

		while (  I2C_ReadSlave ( SI570_Addr, 135 ) & 0x01 );


	}

//===================================================================

void	Set_HS_DIV_N1 ( long Freq_long )	{

	long	N1_VALUE, MIN_N1, MAX_N1, FOSC, temp_N1, temp_HS_DIV, temp_FOSC = 0;
	
	unsigned char	i,j;

	Freq_long	/=	10;
 
        if ( Freq_long < 44090909 ) {

		MAX_N1	=	FOSC_MAX_long / ( Freq_long * 4 );
	
		if ( MAX_N1 % 2 ) MAX_N1 += 1;
	
		if ( MAX_N1 > 128 ) MAX_N1 = 128;

		MIN_N1	=	FOSC_MIN_long / ( Freq_long * 11 );	

		if ( MIN_N1 % 2 ) MIN_N1 -= 1;
	
		if ( MIN_N1 < 0 ) MIN_N1 = 0;

		for	( N1_VALUE = MAX_N1; N1_VALUE > MIN_N1; N1_VALUE -= 2 )	{

			for ( i = 0; i < 6; i++ )	{

				FOSC = N1_VALUE * HS_DIV_VALUE_char [ i ] * Freq_long;	

				if ( FOSC >= FOSC_MIN_long && FOSC <= FOSC_MAX_long )	{
				
					if (  temp_FOSC == 0 || FOSC <= temp_FOSC ) {
									
						temp_FOSC	=	FOSC;
					
						temp_HS_DIV	=	HS_DIV_VALUE_char [ i ];

						temp_N1		=	N1_VALUE;

						}
				
					}
		
				}	
	       	
			}
			
		}
		
	else	{	
		
		for	( j = 0; j < 2; j++ )	{

			for ( i = 0; i < 6; i++ )	{

				FOSC = N1_VALUE_char [ j ] * HS_DIV_VALUE_char [ i ] * Freq_long;	

				if ( FOSC >= FOSC_MIN_long && FOSC <= FOSC_MAX_long )	{
				
					if (  temp_FOSC == 0 || FOSC <= temp_FOSC ) {
									
						temp_FOSC	=	FOSC;
					
						temp_HS_DIV	=	HS_DIV_VALUE_char [ i ];

						temp_N1		=	N1_VALUE_char [ j ];

						}
				
					}
		
				}	
	       	
			}
	
		}
		
	HS_DIV	=	(float)	temp_HS_DIV;
	
	N1	=	(float) temp_N1;

	//Plot_Integer ( temp_HS_DIV, 100, 80 );
	//Plot_Integer ( temp_N1, 100, 100 );

	}

//===================================================================

void	Pack_Si570_registers ( unsigned char reg[ ] )	{

	//long	temp;

	reg[0]	=	( ( HS_DIV - 4.0 ) * 32.0 ) +  ( ( N1 - 1.0 ) / 4.0 );

 	reg[1]	=	( ( (char) ( N1 -1.0 ) & 0b0000011 ) ) << 6;

 	RFREQ_INT = (uint32_t)RFREQ;

 	//reg[1]	=	reg[1] + RFREQ / 16.0;
 	reg[1]	=	reg[1] | RFREQ_INT>>4;


 	RFREQ_FRAC	=	(long)((RFREQ-(double)RFREQ_INT) * twoto28th);

	reg[5]	=	  RFREQ_FRAC        & 0xFF;

	reg[4]	=	( RFREQ_FRAC >> 8 ) & 0xFF;

	reg[3]	=	( RFREQ_FRAC >> 16) & 0xFF;

	reg[2]	=	( RFREQ_FRAC >> 24) & 0x0F;
	
	RFREQ_INT = RFREQ_INT<<4 & 0xF0;
	
	reg[2]	=   reg[2]	| RFREQ_INT;
	
	//Plot_Integer ( reg[0], 0, 120 );
	//Plot_Integer ( reg[1], 0, 100 );
	//Plot_Integer ( reg[2], 0, 80 );
	//Plot_Integer ( reg[3], 0, 60 );
	//Plot_Integer ( reg[4], 0, 40 );
	//Plot_Integer ( reg[5], 0, 20 );

	}
	
//===================================================================

void	Unpack_Si570_registers ( unsigned char reg[ ] )	{

        HS_DIV	=	( ( reg[0] & 0b11100000 ) >> 5 ) + 4;
        
        N1		= 	( ( reg[0] & 0b00011111 ) << 2 ) +
        
        			( ( reg[1] & 0b11000000 ) >> 6 ) + 1;

  	RFREQ_INT =	( reg[1] & 0b00111111 );
  	
  	RFREQ_INT =	( RFREQ_INT << 4 ) + ( ( reg[2] & 0b11110000 ) >> 4 ); 

	RFREQ_FRAC =	( reg[2] & 0b00001111 );
        		
  	RFREQ_FRAC =	( RFREQ_FRAC << 8 ) +    reg[3];
  	 
  	RFREQ_FRAC =	( RFREQ_FRAC << 8 ) +    reg[4];
  	 
  	RFREQ_FRAC =	( RFREQ_FRAC << 8 ) +    reg[5]; 

	RFREQ	=	RFREQ_INT + RFREQ_FRAC / twoto28th;

	}

void Check_SI570 (void)

{
		SI570_Chk = I2C_ReadSlave(SI570_Addr, 135);
}





//===================================================================

void	Read_Si570_registers ( unsigned char read[] )	{

	unsigned char	i;
	
	for ( i = 0; i < 6; i++ ) read [ i ] = I2C_ReadSlave ( SI570_Addr, i + 7 );
        
	}
	 				
//===================================================================

void	Write_Si570_registers ( unsigned char write[] )	{

	char	i;

	i	=	I2C_ReadSlave ( SI570_Addr, 137 );

	I2C_WriteRegister ( SI570_Addr, 137, i | 0x10 );	//	Freeze DCO

	I2C_WriteRegister_N ( SI570_Addr, 7, write, 6 );

	i	=	I2C_ReadSlave ( SI570_Addr, 137 );

	I2C_WriteRegister ( SI570_Addr, 137, i & ~0x10 );	//	Unfreeze DCO

	//if ( Large_RFREQ_Change (  ) || ( encoder_index < 7 ) )	{  // changed from John's code since we are
	//not yet using encoder_index

	if ( Large_RFREQ_Change (  ) )	{

		i	=	I2C_ReadSlave ( SI570_Addr, 135 );

		I2C_WriteRegister ( SI570_Addr, 135, i | 0x40 );	//	New Freq Applied
	
		while (  I2C_ReadSlave ( SI570_Addr, 135 ) & 0x40 );
 	 		
		}

	}
	 				
//===================================================================

char	Large_RFREQ_Change ( void )	{

	double	delta_RFREQ;
	
	if ( Old_RFREQ == RFREQ ) return 0;

	if ( Old_RFREQ < RFREQ )	delta_RFREQ = RFREQ - Old_RFREQ;
		
	else			delta_RFREQ = Old_RFREQ - RFREQ;

	if (  (  delta_RFREQ / Old_RFREQ  ) <= 0.0035 )	return 0;
	
	else					return 1;

	}

//===================================================================
