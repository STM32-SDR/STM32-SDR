/*
 * Encoder_2.c
 *
 *  Created on: Mar 23, 2013
 *      Author: CharleyK
 */



/*
 * Encoder_1.c
 *
 *  Created on: Feb 25, 2013
 *      Author: CharleyK
 */

#include	"stm32f4xx.h"
#include	"stm32f4xx_rcc.h"
#include	"stm32f4xx_exti.h"
#include	"stm32f4xx_syscfg.h"
#include 	"stm32f4xx_gpio.h"
#include	"misc.h"
#include	"arm_math.h"
#include	"TFT_Display.h"
#include	"SI570.h"
#include	"Init_I2C.h"
#include	"Init_Codec.h"
#include	"Encoder_2.h"
#include	"eeprom.h"
#include	"DMA_IRQ_Handler.h"
#include	"arm_math.h"
#include	"Codec_Gains.h"
#include	"ChangeOver.h"
#include	"ModeSelect.h"
#include	"Encoder_1.h"

EXTI_InitTypeDef   EXTI_InitStructure;


int16_t read_SS2;
int16_t IQData[16];
int16_t DataMin[16];
int16_t DataMax[16];
int16_t IQ_Unit[16];

int16_t IQ_Disp;

int16_t ENC_Sens2;
int8_t DIR2;


int16_t  EEProm_Value2;

void Encoder2_GPIO_Config(void)
{
  GPIO_InitTypeDef   GPIO_InitStructure;
  //NVIC_InitTypeDef   NVIC_InitStructure;

  /* Enable GPIOC clock */
  RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOC, ENABLE);
  /* Enable SYSCFG clock */
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_SYSCFG, ENABLE);

  /* Configure PC1,2,3&4 pin as input with Pull Up */
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
  GPIO_InitStructure.GPIO_Pin = (GPIO_Pin_1|GPIO_Pin_2|GPIO_Pin_3|GPIO_Pin_4);
  GPIO_Init(GPIOC, &GPIO_InitStructure);

  RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB, ENABLE);
  /* Configure PE3,4,5&7 pin as input with Pull Up */
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
  GPIO_InitStructure.GPIO_Pin = (GPIO_Pin_4|GPIO_Pin_5|GPIO_Pin_7);
  GPIO_Init(GPIOB, &GPIO_InitStructure);


  /* Enable SYSCFG clock */
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_SYSCFG, ENABLE);
  /* Connect EXTI Line 7 to PA7 pin */
  SYSCFG_EXTILineConfig(EXTI_PortSourceGPIOB, EXTI_PinSource7);

  /* Configure EXTI Line7 */
  EXTI_InitStructure.EXTI_Line = EXTI_Line7;
  EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;
  EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Falling;
  EXTI_InitStructure.EXTI_LineCmd = ENABLE;
  EXTI_Init(&EXTI_InitStructure);

 // /* Enable and set EXTI Line9-5 Interrupt to the lowest priority */
 // NVIC_InitStructure.NVIC_IRQChannel = EXTI9_5_IRQn;
 // NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0x01;
 // NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0x01;
 // NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
 // NVIC_Init(&NVIC_InitStructure);

}



			void Set_Initial_IQ_Data(void){
			IQData[0]  = 0;  //Rx_Audio
			IQData[1]  = 20;  //Rx_RF
			IQData[2]  = 20;  //SSB_Level
			IQData[3]  = 0;  //CW_Level
			IQData[4]  = 0;  //PSK_Level
			IQData[5]  = 0;  //ST_Level
			IQData[6]  = 5000;  //Rx_Amp
			IQData[7]  = 0;  //Rx_Phase
			IQData[8]  = 5000;  //Tx_Amp
			IQData[9]  = 0;  //Rx_Phase
			IQData[10] = 0;  //Microphone Bias
			IQData[11] = 0;
			IQData[12] = 0;
			IQData[13] = 0;
			IQData[14] = 0;
			IQData[15] = 0;
			}


			void Set_Minimums(void) {
			DataMin[0] = -127;
			DataMin[1] = 0;
			DataMin[2] = 0;
			DataMin[3] = -127;
			DataMin[4] = -127;
			DataMin[5] = 0;
			DataMin[6] = 1000;
			DataMin[7] = -10000;
			DataMin[8] = 1000;
			DataMin[9] = -10000;
			DataMin[10] = 0;
			DataMin[11] = 0;
			DataMin[12] = 0;
			DataMin[13] = 0;
			DataMin[14] = 0;
			DataMin[15] = 0;
			}

			void Set_Maximums(void) {
			DataMax[0] = 48;
			DataMax[1] = 80;
			DataMax[2] = 80;
			DataMax[3] = 29;
			DataMax[4] = 29;
			DataMax[5] = 29;
			DataMax[6] = 10000;
			DataMax[7] = 10000;
			DataMax[8] = 10000;
			DataMax[9] = 10000;
			DataMax[10] = 1;
			DataMax[11] = 100;
			DataMax[12] = 100;
			DataMax[13] = 100;
			DataMax[14] = 100;
			DataMax[15] = 100;
			}

			void Set_IQ_Units(void){
			IQ_Unit[0] = 4;
			IQ_Unit[1] = 1;
			IQ_Unit[2] = 1;
			IQ_Unit[3] = 1;
			IQ_Unit[4] = 1;
			IQ_Unit[5] = 1;
			IQ_Unit[6] = 10;
			IQ_Unit[7] = 10;
			IQ_Unit[8] = 10;
			IQ_Unit[9] = 10;
			IQ_Unit[10] = 1;
			IQ_Unit[11] = 1;
			IQ_Unit[12] = 1;
			IQ_Unit[13] = 1;
			IQ_Unit[14] = 1;
			IQ_Unit[15] = 1;
			}

void	init_encoder2	( void )	{

		EEProm_Value2 = Read_Int_EEProm(50);//Read the 50 address to see if IQ data has been stored

		if (EEProm_Value2 != 5678)
		{
		Set_Initial_IQ_Data();
		Store_IQ_Data();
		Write_Int_EEProm(50,5678);
		}

		Read_IQ_Data ();

		Set_Minimums();
		Set_Maximums();
		Set_IQ_Units();
		R_lgain = (float)IQData[6]/10000.0;
		R_xgain = (float)IQData[7]/10000.0;
		T_lgain = (float)IQData[6]/10000.0;
		T_xgain = (float)IQData[7]/10000.0;

		rgain = 0.5;					//temp location, move to sequencer
		LCD_StringLine(0,220,"USB");

		Tx_Flag=0;
		LCD_StringLine(296,220,"RX");  //temp location, move to sequencer

		Receive_Sequence();

		ENC_Sens2 =2;
		check_SS2 ();
		process_SS2();

		//Set Codec Parameters //will probably move these statements elsewhere
		Set_DAC_DVC (IQData[0] );  //Set RX_Audio
		Set_PGA_Gain (IQData[1]); //Set Rx_RF
		if (IQData[10] ==0) Turn_Off_Bias();
		if (IQData[10] ==1) Turn_On_Bias();
		}

    void process_encoder2 (void ) {

		// encoder motion has been detected--determine direction
		static uint8_t old = {0};
		static int8_t enc_states [] = {0,1,-1,0,-1,0,0,1,1,0,0,-1,0,-1,1,0};
		int8_t tempDIR;
		static int8_t DIR0;


	Delay(100 );
	old <<= 2;  //remember previous encoder state
	old |= (GPIO_ReadInputData(GPIOB)>>4 & 0x03);  //get the current state
	tempDIR = enc_states[(old & 0x0F)];
	DIR0 += tempDIR;
	if ((DIR0 < ENC_Sens2) && (DIR0 >-ENC_Sens2)) DIR2 = 0;
	if (DIR0 >= ENC_Sens2) {
		DIR2 = 1;
		DIR0 = 0;
	}
	if (DIR0 <= -ENC_Sens2) {
		DIR2 = -1;
		DIR0 = 0;
	}


	if (DIR2 != 0){
					check_SS2();
					switch(read_SS2) {

					case 0: //Rx_Audio
					if ((IQData[read_SS2]>DataMin[read_SS2]) & (DIR2<0)) IQData[read_SS2]  += IQ_Unit[read_SS2]*DIR2;
					if ((IQData[read_SS2]<DataMax[read_SS2]) & (DIR2>0)) IQData[read_SS2]  += IQ_Unit[read_SS2]*DIR2;
					IQ_Disp = IQData[read_SS2];
					if (Tx_Flag==0) Set_DAC_DVC (IQData[read_SS2] ); //Set RX_Audio
					break;

					case 1: //Rx_RF
					if ((IQData[read_SS2]>DataMin[read_SS2]) & (DIR2<0)) IQData[read_SS2]  += IQ_Unit[read_SS2]*DIR2;
					if ((IQData[read_SS2]<DataMax[read_SS2]) & (DIR2>0)) IQData[read_SS2]  += IQ_Unit[read_SS2]*DIR2;
					IQ_Disp = IQData[read_SS2];
					if (Tx_Flag==0) Set_PGA_Gain (IQData[read_SS2]); //Set Rx_RF
					break;

					case 2: //SSB_Level
					if ((IQData[read_SS2]>DataMin[read_SS2]) & (DIR2<0)) IQData[read_SS2]  += IQ_Unit[read_SS2]*DIR2;
					if ((IQData[read_SS2]<DataMax[read_SS2]) & (DIR2>0)) IQData[read_SS2]  += IQ_Unit[read_SS2]*DIR2;
					IQ_Disp = IQData[read_SS2];
					if ((Tx_Flag==1 )&& (Mode==0)) Set_PGA_Gain (IQData[read_SS2]); //SSB_Level
					break;

					case 3: //CW_Level
					if ((IQData[read_SS2]>DataMin[read_SS2]) & (DIR2<0)) IQData[read_SS2]  += IQ_Unit[read_SS2]*DIR2;
					if ((IQData[read_SS2]<DataMax[read_SS2]) & (DIR2>0)) IQData[read_SS2]  += IQ_Unit[read_SS2]*DIR2;
					IQ_Disp = IQData[read_SS2];
					if ((Tx_Flag==1)&& (Mode==1)) Set_DAC_DVC (IQData[read_SS2] ); //Set CW_Level
					break;

					case 4:  //PSK_Level
					if ((IQData[read_SS2]>DataMin[read_SS2]) & (DIR2<0)) IQData[read_SS2]  += IQ_Unit[read_SS2]*DIR2;
					if ((IQData[read_SS2]<DataMax[read_SS2]) & (DIR2>0)) IQData[read_SS2]  += IQ_Unit[read_SS2]*DIR2;
					IQ_Disp = IQData[read_SS2];
					if ((Tx_Flag==1)&& (Mode==2)) Set_DAC_DVC (IQData[read_SS2] ); //Set PSK_Level
					break;

					case 5:  //Side Tone Level
					if ((IQData[read_SS2]>DataMin[read_SS2]) & (DIR2<0)) IQData[read_SS2]  += IQ_Unit[read_SS2]*DIR2;
					if ((IQData[read_SS2]<DataMax[read_SS2]) & (DIR2>0)) IQData[read_SS2]  += IQ_Unit[read_SS2]*DIR2;
					IQ_Disp = IQData[read_SS2];
					if ((Tx_Flag==1)&& (Mode==1)) Set_HP_Gain(IQData[read_SS2] ); //Set ST_Level
					//if ((Tx_Flag==1)&& (Mode==1)) Set_LO_Gain(IQData[read_SS2] ); //Set ST_Level
					break;

					case 6: //Rx_Amp
					if ((IQData[read_SS2]>DataMin[read_SS2]) & (DIR2<0)) IQData[read_SS2]  += IQ_Unit[read_SS2]*DIR2;
					if ((IQData[read_SS2]<DataMax[read_SS2]) & (DIR2>0)) IQData[read_SS2]  += IQ_Unit[read_SS2]*DIR2;
					IQ_Disp = IQData[read_SS2];
					R_lgain = ((float)IQData[read_SS2])/10000.0;
					break;

					case 7:  //Rx_Phase
					if ((IQData[read_SS2]>DataMin[read_SS2]) & (DIR2<0)) IQData[read_SS2]  += IQ_Unit[read_SS2]*DIR2;
					if ((IQData[read_SS2]<DataMax[read_SS2]) & (DIR2>0)) IQData[read_SS2]  += IQ_Unit[read_SS2]*DIR2;
					IQ_Disp = IQData[read_SS2];
					R_xgain = ((float) IQData[read_SS2]) /10000.0;
					break;

					case 8:  //Tx_Amp
					if ((IQData[read_SS2]>DataMin[read_SS2]) & (DIR2<0)) IQData[read_SS2]  += IQ_Unit[read_SS2]*DIR2;
					if ((IQData[read_SS2]<DataMax[read_SS2]) & (DIR2>0)) IQData[read_SS2]  += IQ_Unit[read_SS2]*DIR2;
					IQ_Disp = IQData[read_SS2];
					T_lgain = ((float)IQData[read_SS2])/10000.0;
					break;

					case 9:  //Tx_Phase
					if ((IQData[read_SS2]>DataMin[read_SS2]) & (DIR2<0)) IQData[read_SS2]  += IQ_Unit[read_SS2]*DIR2;
					if ((IQData[read_SS2]<DataMax[read_SS2]) & (DIR2>0)) IQData[read_SS2]  += IQ_Unit[read_SS2]*DIR2;
					IQ_Disp = IQData[read_SS2];
					T_xgain = ((float) IQData[read_SS2]) /10000.0;
					break;

					case 10:  //Microphone Bias
					if ((IQData[read_SS2]>DataMin[read_SS2]) & (DIR2<0)) IQData[read_SS2]  += IQ_Unit[read_SS2]*DIR2;
					if ((IQData[read_SS2]<DataMax[read_SS2]) & (DIR2>0)) IQData[read_SS2]  += IQ_Unit[read_SS2]*DIR2;
					IQ_Disp = IQData[read_SS2];
					if (IQData[read_SS2] ==0) Turn_Off_Bias();
					if (IQData[read_SS2] ==1) Turn_On_Bias();
					break;


			} //end of switch read_input

	}
			Plot_Integer (IQ_Disp,0,0);

    }   //end of process_encoder


void check_SS2 (void)		{
//Read PC1,2,3,4 as four bit word
read_SS2 = ~GPIO_ReadInputData(GPIOC)>>1 & 0x000F;

}

void process_SS2 (void)		{
//Read PC1,2,3,4 as four bit word


					switch(read_SS2) {

					case 0:
              		IQ_Disp = IQData[read_SS2];
              		LCD_StringLine(0,20," Rx_Audio ");
                	break;

					case 1:
              		IQ_Disp = IQData[read_SS2];
              		LCD_StringLine(0,20,"   Rx_RF  ");
                	break;

					case 2:
              		IQ_Disp = IQData[read_SS2];
              		LCD_StringLine(0,20," SSB_Level");
                	break;

					case 3:
              		IQ_Disp = IQData[read_SS2];
              		LCD_StringLine(0,20," CW_Level ");
                	break;

					case 4:
              		IQ_Disp = IQData[read_SS2];
              		LCD_StringLine(0,20," PSK Level");
                	break;

					case 5:
              		IQ_Disp = IQData[read_SS2];
              		LCD_StringLine(0,20," ST_Level ");
                	break;

					case 6:
              		IQ_Disp = IQData[read_SS2];
              		LCD_StringLine(0,20,"   Rx_Amp ");
                	break;

					case 7:
              		IQ_Disp = IQData[read_SS2];
              		LCD_StringLine(0,20," Rx_Phase ");
                	break;

					case 8:
              		IQ_Disp = IQData[read_SS2];
              		LCD_StringLine(0,20,"   Tx_Amp ");
                	break;

					case 9:
              		IQ_Disp = IQData[read_SS2];
              		LCD_StringLine(0,20," Tx_Phase ");
                	break;

					case 10:
              		IQ_Disp = IQData[read_SS2];
              		LCD_StringLine(0,20," Mic Bias ");
                	break;

					case 11:
              		IQ_Disp = IQData[read_SS2];
              		LCD_StringLine(0,20," Future 1 ");
                	break;


					case 12:
              		IQ_Disp = IQData[read_SS2];
              		LCD_StringLine(0,20," Future 2 ");
                	break;

					case 13:
              		IQ_Disp = IQData[read_SS2];
              		LCD_StringLine(0,20," Future 3 ");
                	break;

					case 14:
              		IQ_Disp = IQData[read_SS2];
              		LCD_StringLine(0,20," Future 4 ");
                	break;

					case 15:
              		IQ_Disp = IQData[read_SS2];
              		LCD_StringLine(0,20," Future 5 ");
                	break;

					}

					Plot_Integer (IQ_Disp, 0,0 );

			}

			void Store_Defaults(void) {
				LCD_StringLine(234,40," Default  ");
				Set_Initial_SI570_Data();
				Store_SI570_Data();
				check_SS1();
				process_SS1();

				LCD_StringLine(0,40," Default  ");
				Set_Initial_IQ_Data();
				Store_IQ_Data();
				check_SS2();
				process_SS2();
			}

