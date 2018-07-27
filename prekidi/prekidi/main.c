/*
 * prekidi.c
 *
 * Created: 24.5.2017. 20.14.52
 * Author : Kovacevic Corporation
 */ 

#include <avr/io.h>
#include <avr/interrupt.h>


int main(void)
{
    sei();

	GPIOR1 |= 1<<7;
	
    while (1) 
    {
		
    }
}

