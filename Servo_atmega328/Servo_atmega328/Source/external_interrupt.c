/*
 * external_interrupt.c
 *
 * Created: 21.7.2018. 01.50.46
 *  Author: Kovacevic
 */ 
#include "common.h"
#include "timer.h"

volatile uint_fast8_t opadajuca = 0;


void ext_int_init()
{


	EICRA |= (1<<ISC01);	//The falling edge of INT0 generates an interrupt request.
	EIMSK |= 1<<INT0;		//External Interrupt Request 0 Enable
	
	
}



ISR(INT0_vect)
{
	
	if (!opadajuca)
	{
		opadajuca = 1;
		sys_time = 0;
		TCNT0 = 0; //reset tajmera, tj start
	}
	
	
}