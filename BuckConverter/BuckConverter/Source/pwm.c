/*
 * pwm.c
 *
 * Created: 1.12.2017. 22.23.40
 *  Author: Ko''
 */ 
#include "pwm.h"
#include "common.h"

void pwm_init()
{
	
	DDRB |= 1<<DDB0;
	
	DDRB |= (1 << DDB1)|(1 << DDB2); //izlazni pinovi za PWM - D9 i D10 na Arduino Uno ploci
	

	ICR1 = 200;			//frekvencija ~ 20 kHz
	OCR1A = 0;		    //faktor ispune 0% 
	OCR1B = OCR1A;
	

	TCCR1A = 0b10110000;		//inverting rezim tj. pwm signali su komplementarni

	
	TCCR1B |= (1 << WGM13);		// set Fast PWM mode using ICR1 as TOP
	
	TCCR1B |= (1 << CS10);		// START the timer with no prescaler
	
	TIMSK1 = 0b10;     //enable comp a interupts
	
	
	//upis i promena faktora ispune se vrsi u ad konverziji
}

ISR(TIMER1_COMPA_vect)
{
	//ADCSRA |= 1<<ADSC; //start ad conversion
	
	PINB |= 0b1;
}