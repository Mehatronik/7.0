/*
 * vezbanje sa diodama i tajmerom.c
 *
 * Created: 3.11.2017. 17.57.30
 * Author : Ko''
 */ 

#include <avr/io.h>
#include <avr/interrupt.h>

#define vreme 63999


int main(void)
{
    
	DDRB = 0xFF;
	
	TCCR1A = 0;
	TCCR1B = 0b100;
	TIMSK1 = 1;
	TCNT1 = vreme;
	
	sei();
	
    while (1) 
    {
		
    }
}

ISR(TIMER1_OVF_vect)
{
	TCNT1 = vreme;
	PORTB =~ PINB;
}