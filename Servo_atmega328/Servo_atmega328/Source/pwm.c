/*
 * pwm.c
 *
 * Created: 20.7.2018. 21.39.48
 *  Author: Kovacevic
 */ 

#include "common.h"


const uint_least16_t TOP_PWM = 39999;

void pwm_init()
{
	DDRB |= 1<<DDB1;    //PB1 = pwm izlaz za servo
	
	TCCR1A |= (1<<COM1A1)|(1<<WGM11);           //Clear OC1A/OC1B on compare match, set OC1A/OC1B at BOTTOM (non-inverting mode); fast pwm mode, top = ICR1
	TCCR1B |= (1<<WGM13)|(1<<WGM12)|(1<<CS11);	//fast pwm mode, top = ICR1;  prescaler = 8
	ICR1 = TOP_PWM;								//top=39999, 50 Hz
	OCR1A = TOP_PWM;							//duty cycle (faktor ispune);  0 - 39999 (0-100%). Inicijalno na max, tj. 5V const
	//TIMSK1 |= 1<<OCIE1A;						//compare match A (OCR1A) interrupt enable
	TCNT1 = 0;									//resetujem tc brojac da ovde krene od nule. Kada udari u ICR1(39999) resetuje se
	
}


ISR(TIMER1_COMPA_vect)	//pwm prekid, ne treba mi?
{
	
}