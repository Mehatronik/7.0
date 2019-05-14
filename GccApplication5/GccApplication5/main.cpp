/*
 * GccApplication5.cpp
 *
 * Created: 9.5.2019. 23.24.58
 * Author : Kovacevic
 */ 
#define F_CPU 16000000UL

#include <avr/io.h>
#include <util/delay.h>

int main(void)
{
	
	DDRB |= 1<<PINB5;
	
	
   
    while (1) 
    {
		//PORTB ^= 1<<PINB5;
		PINB |= 1<<PINB5;
		_delay_ms(30);
    }
	
	return 0;
}

