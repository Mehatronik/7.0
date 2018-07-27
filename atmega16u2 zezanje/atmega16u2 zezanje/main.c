/*
 * atmega16u2 zezanje.c
 *
 * Created: 6.10.2017. 18.17.42
 * Author : Ko''
 */ 

#define F_CPU 16000000


#include <avr/io.h>
#include <util/delay.h>


int main(void)
{
    
	
	
	
    while (1) 
    {
		PINB |= 1<<PINB7;  //toggle pb7 pin bez obzira na output/input
		_delay_ms(1);
		
    }
	
	return 0;
}

