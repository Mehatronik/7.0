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
	
	
	
	DDRB |= (1 << DDB1)|(1 << DDB2); //izlazni pinovi za PWM - D9 i D10 na Arduino Uno ploci
	

	ICR1 = 500;			//frekvencija ~ 16 kHz
	OCR1A = 250;		//faktor ispune 50% na startu sto daje nultu vrednost struje na H mostu
	OCR1B = OCR1A;
	

	TCCR1A = 0b10110000;		//inverting rezim tj. pwm signali su komplementarni

	
	TCCR1B |= (1 << WGM13);		// set Fast PWM mode using ICR1 as TOP
	
	TCCR1B |= (1 << CS10);		// START the timer with no prescaler
	
	//upis i promena faktora ispune se vrsi u ad konverziji
}