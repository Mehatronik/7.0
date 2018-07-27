/*
 * GccApplication1.c
 *
 * Created: 24.5.2017. 19.19.53
 * Author : Vladan Kovacevic
 */ 

#define F_CPU 16000000UL

#include <avr/io.h>
#include <util/delay.h>

int main(void)
{
    
	//DDRB |= 0b100000;
	DDRB |= 0x20;
	
    while (1) 
    {
		PORTB |= 0x20;
		_delay_ms(200);
		PORTB &= 0xDF;  //0xDF = 0b11011111
		_delay_ms(200);
    }
}

