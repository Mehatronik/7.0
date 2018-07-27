/** 
 * h_bridge.c
 *
 * Created: 14.12.2016 9:17:58
 *  Author: Mirko
 */ 

#include "common.h"

void init_PWM(void)
{
	HIRESF_CTRLA = 0b00000010; //enable hi-res za TCF1
	// Configure timer
	TCF1.PER = MAX_DuC; //bilo 800, sada 4000 zbog izmene F_CPU, sto daje   ~16kHz, i vecu rezoluciju pwm-a
	TCF1.CTRLB = TC1_CCBEN_bm | TC1_CCAEN_bm | TC_WGMODE_DS_T_gc;
	TCF1.CTRLA = TC_CLKSEL_DIV1_gc;
	
	PORT_SetPinsAsOutput( &PORTF, 0b00110000 ); //PF5 - PWM_L, PF4 - PWM_D
		
	//pinovi za upravljanje mostovima
	PORT_SetPinsAsOutput(&PORTH, 0xFF); //PH0-IN_A1, PH1-EN_A1, PH2-EN_B1, PH3-IN_B1, PH4-IN_A2, PH5-EN_A2, PH6-EN_B2, PH7-IN_B2
	PORT_ClearPins(&PORTH, 0xFF);
	PORT_SetPins(&PORTH, 0b01100110); // EN ALL

	//servo izlazi
	PORT_SetPinsAsOutput(&PORTF, 0x0F);
	PORT_ClearPins(&PORTF, 0x0F);
}