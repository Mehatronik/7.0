/*
 * diode.c
 *
 * Created: 2.11.2017. 21.03.02
 * Author : Ko''
 */ 

#define F_CPU 16000000

#include <avr/io.h>
#include <util/delay.h>



int i = 0;


int main(void)
{
    
	DDRB = 0xFF;
	
	
    while (1) 
    {

		
		for(i=0; i<5; i++)
		{
			PORTB = 1<<i;
			_delay_ms(100);
			PORTB = 0;
		}
		
		for(i=3; i>0; i--)
		{
			PORTB = 1<<i;
			_delay_ms(100);
			PORTB = 0;
		}
		
    }
}

