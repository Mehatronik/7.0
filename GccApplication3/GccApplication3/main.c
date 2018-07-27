/*
 * GccApplication2.c
 *
 * Created: 01-May-17 02:01:12
 * Author : Kovacevic
 *
 *
 ****** Nije mi jasno kako se uzimaju vrednosti za TOP i vrednosti za faktor ispune - OCRxn registri
 *
 *
 */ 

#define F_CPU 16000000UL

#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>





void IO_init()
{
	DDRB |= ((1<<PORTB2)|(1<<PORTB3)) ; //PB2,PB3 izlazi (10, 11)
	
	
}

void timer2_init()
{
	PRR = 0;  //ili: PRR &= ~(1<<PRTIM2); upis nule za svaki slucaj da se iskljuci sleep mod
	TCCR2A |= ( (1<<COM2A1)|(1<<WGM21)|(1<<WGM20) );  //pwm non-inverted na OC2A pinu (digital 11), TOP = OCR2A (gornja vrednost brojaca),, START Brojaca
	TCCR2B |= 0b01; //1<<CS21;  //prescaler 1/1   f=62.5kHz
	OCR2A = 100;           //fakticki faktor ispune
	TIMSK2 |= 1<<OCIE2A;        //dozvola prekida tajmera #2 usled poredjenja sa OCR2A, tj. dostizanja tog broja
	sei();                 //globalna dozvola prekida
}



ISR(TIMER2_COMPA_vect)
{
	
	//OCR2A = 20;
	
}
	




int main(void)
{
    
	int i;
	int niz[200];

	IO_init(); //inicijalizacija ulazno-izlaznih portva(pinova)
	timer2_init(); //inicijalizacija tajmera #2 
	
	for(i=0; i<200; i++)
	{
		niz[i] = i+20;
	}
	
	i = 0;
	
   /* while (1) 
    {
		
		OCR2A = niz[i];
		i++;
		if(i==199)          //plivajuci faktor ispune
			i = 0;
		_delay_ms(50);
		
		
    }*/
}

