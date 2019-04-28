/*
 * tajmer.c
 *
 * Created: 2.12.2017. 22.08.59
 *  Author: Kovacevic
 */ 

//Tajmer koji ce da okida svaku 1ms

#include "tajmer.h"
#include "comm.h"


volatile unsigned char flag_tajmer0_prekid = 0, flag_prekid_10ms = 0;
volatile unsigned int brojac_prekida_tajmera0;


void tajmer0_init()
{
	TCCR0A = 0b10;		//CTC mode, TOP=OCR0A
	TCCR0B = 0b11;		//prescaler = 64
	OCR0A = 249;		//da bi se dobila frekvencija 1kHz odnosno prekid na svaki 1ms
	TIMSK0 = 0b10;		//compare match A interrupt enable
	
	DDRB |= 1<<PINB5;		//pinB 5 - DIG13 = OUTPUT
}

ISR(TIMER0_COMPA_vect)   //1ms prekid
{
	flag_tajmer0_prekid = 1;
	
	brojac_prekida_tajmera0++;	//prekid okida svaki 1ms
	
		
	if(brojac_prekida_tajmera0 == 10)	//1ms * 10 = 10ms
	{
		brojac_prekida_tajmera0 = 0;
		flag_prekid_10ms = 1;
		PINB |= 1<<PINB5;					//toogle pinB 5 - DIG13, test da vidim da li je korektna frekvencija
		
	}
	
	
}