/*
 * R2R ladder.c
 *
 * Created: 1.8.2017. 03.51.25
 * Author : Ko''
 */ 

#define F_CPU 16000000

#include <avr/io.h>
#include <util/delay.h>



#define cekanje 10

int main(void)
{
	
	DDRB |= (1<<PORTB0)|(1<<PORTB1)|(1<<PORTB2);    //izlazi (arduino D8-D10)
	
	PORTB = 0;
    
    while (1) 
    {
		/*PORTB = 0b00000000;
		_delay_us(cekanje);
		PORTB = 0b00000001;
		_delay_us(cekanje);
		PORTB = 0b00000010;
		_delay_us(cekanje);
		PORTB = 0b00000011;
		_delay_us(cekanje);
		PORTB = 0b00000100;
		_delay_us(cekanje);
		PORTB = 0b00000101;
		_delay_us(cekanje);
		PORTB = 0b00000110;
		_delay_us(cekanje);
		PORTB = 0b00000111;
		_delay_us(cekanje);*/
		
		PORTB ++;
		
		
    }
}

