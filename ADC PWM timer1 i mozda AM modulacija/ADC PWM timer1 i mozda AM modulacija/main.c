/*
 * ADC PWM timer1 i mozda AM modulacija.c
 *
 * Created: 1.8.2017. 21.36.39
 * Author : Ko''
 */ 

#include <avr/io.h>
#include <avr/interrupt.h>

int main(void)
{
    //internal 1.1V ref with external capacitor at Aref pin
	ADMUX |= (1<<REFS1)|(1<<REFS0);
	ADCSRA |= (1<<ADEN)|(1<<ADSC)|(1<<ADATE);
	ADCSRB |= 0b00000011;  //auto trigger timer1 compA
	
    while (1) 
    {
		
    }
}

