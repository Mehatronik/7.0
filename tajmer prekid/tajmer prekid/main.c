/*
 * tajmer prekid.c
 *
 * Created: 25.5.2017. 16.05.23
 * Author : Kovacevic Corporation (c) 2017. All rights not reserved.
 
 */ 

#define F_CPU 16000000UL

#include <avr/io.h>
#include <avr/interrupt.h>



ISR(TIMER0_COMPA_vect)
{
	//PINB |= 1<<PINB5; //toogle pin 5
	PORTB ^= 1<<5;
}






int main(void)
{
    //13 = PB5
	
	cli();
	
	DDRB |= (1<<5);  //PB5 izlazni pin
	OCR0A = 200;     //15624
	
	TCCR0A |= (1<<COM0A0)|(1<<WGM01);
	//TCCR0B |= 1<<WGM12;  //ctc
	TCCR0B |= (1<<CS00);      //(1<<CS12)|(1<<CS10);   //prescaler 1024
	TIMSK0 |= 1<<OCIE0A;          //dozvola prekida tajmera1 usled compare match-a
	
	sei();
	
	

	
	
    while (1) 
    {
		
		
    }
	
	
	
	
}

