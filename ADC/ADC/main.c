/*
 * ADC.c
 *
 * Created: 3.6.2017. 03.39.45
 * Author : K
 */ 

#define F_CPU 16000000

#include <avr/io.h>
#include <avr/interrupt.h>






void AD_C_init()
{
	
	
	PRR = 0; //power reduction off
	
	ADMUX |= (1<<REFS0);  //AVcc referentni napon, ulazni pin A0
	ADCSRA |= (1<<ADEN)|(1<<ADATE)|(1<<ADIE)|(1<<ADSC);  //ADC enable, auto trigger enable, ADC conv. complete interrupt enable, conv start
	
	//ADCL, ADCH(2 zadnja bita), ADCL mora biti prvi procitan
	
	ADCSRB = 0; //free runniing mode
	DIDR0 = 0xFF; //digital pin disable - na svim analognim pinovima jer nisu potrebni (opciono)
	
	
}

ISR(ADC_vect)
{
	
}




int main(void)
{
	cli();
	
	
	AD_C_init();
    
	
	sei();
	
	
	
    while (1) 
    {
		
    }
}

