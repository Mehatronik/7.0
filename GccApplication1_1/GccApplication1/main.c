/*
 * GccApplication1.c
 *
 * Created: 30-Apr-17 23:24:13
 * Author : Kovacevic
 */ 

#define F_CPU 16000000UL

#include <avr/io.h>
#include <avr/interrupt.h>


int main(void)
{
    PRR &= 0b11011111; //PRTIM0 na nulu za svaki slucaj jer '1' onemogucava rad TIMER modula
	TCCR0A = 0b10;  //ctc mode
	TCCR0B = 0b11; //64 prescaler
	OCR0A = 248;
	TIMSK0 = 0b10; //enable ctc A
	
	sei();
	


	DDRB |= 0b00100000; //PB5 izlaz
	
	PORTB &= ~(0b00100000); //PB5 na nulu
	
	

    while (1) 
    {

    }


 return 0; 


}


ISR(TIMER0_COMPA_vect)
{
	PORTB ^= 0b00100000;


	
}