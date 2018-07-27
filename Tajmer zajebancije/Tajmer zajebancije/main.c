/*
 * Tajmer zajebancije.c
 *
 * Created: 29.7.2017. 05.21.52
 * Author : Ko''
 */ 

#define  F_CPU 16000000

#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>



volatile int adc_low, adc_high, adc_res, i;

void AD_C_init()
{
	
	
	PRR = 0; //power reduction off
	
	ADMUX |= (1<<REFS1)|(1<<REFS0);  //1.1V referentni napon, ulazni pin A0
	ADCSRA = 0xFF ;  //ADC enable, auto trigger enable, ADC conv. complete interrupt enable, sample 125kHz (128 prescaler)
	
	//ADCL, ADCH(2 zadnja bita),
	
	ADCSRB = 0; //free runniing mode
	//DIDR0 = 0xFF; //digital pin disable - na svim analognim pinovima jer nisu potrebni (opciono)
	
	
}

ISR(ADC_vect)
{
	//ADCL mora biti prvi procitan
	adc_low = (int)ADCL;    //koriste se dva 8-bit registra jer je rezultat AD konverzije 10-bitan
	adc_high = (int)ADCH;
	
	/**stapam rezultate iz dva registra u jednu promenljivu na ovaj nacin**/
	if(adc_high==0)
	adc_res = adc_low;
	else if(adc_high==1)
	adc_res = 256 + adc_low;
	else if(adc_high==2)
	adc_res = 256+256 + adc_low;
	else if(adc_high==3)
	adc_res = 256+256+256 + adc_low;
	
	//adc_res = 1024 max
	
	OCR1A = adc_res; //2.56;
	
}

ISR(TIMER1_COMPA_vect)
{
	//PINB |= 1<<PINB5; //toogle pin 5
	PORTB ^= 1<<5;

}

int main(void)
{
    
	DDRB |= 1<<PORTB1;  //PB1 izlazni pin
	
	//DDRD |= 1<<PORTD6; //PD6 izlaz (OC0A pin)
	     
	TCCR1A |= (1<<COM1A0);		//toogle, ctc mode
	TCCR1B |= (1<<WGM12)|(1<<CS10);	//prescaler = 1
	OCR1A = 210;
	TIMSK1 |= 1<<OCIE1A;  //ctc A interrupt enable
	
	
	
	//AD_C_init();
	
	sei();

	
    while (1) 
    {
		for(i =25 ; i<=50; i++)
		{
			OCR1A = i;
			_delay_ms(50);
		}
    }
}

