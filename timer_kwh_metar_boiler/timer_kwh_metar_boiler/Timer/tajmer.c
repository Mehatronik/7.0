/*
 * tajmer.c
 *
 * Created: 2.12.2017. 22.08.59
 *  Author: Kovacevic
 */ 

//Tajmer koji ce da okida svaku 1ms

#include "tajmer.h"
#include "comm.h"


volatile uint8_t flag_tajmer0_prekid = 0;
volatile uint8_t flag_prekid_10ms = 0;
volatile uint8_t flag_prekid_debounce_time = 0;
volatile uint8_t flag_prekid_debounce_time_half = 0;

volatile uint8_t brojac_prekida_tajmera0 = 0;
volatile uint8_t brojac_prekida_tajmera0_debounce = 0;
volatile uint8_t brojac_prekida_tajmera0_debounce_half = 0;

void tajmer0_init()
{
	TCCR0A = 0b10;		//CTC mode, TOP=OCR0A
	TCCR0B = 0b11;		//prescaler = 64
	OCR0A = 249;		//da bi se dobila frekvencija 1kHz odnosno prekid na svaki 1ms
	TIMSK0 = 0b10;		//compare match A interrupt enable
	
	DDRD |= (1<<PIND2)|(1<<PIND3);  //PD2-3 output
	
}

ISR(TIMER0_COMPA_vect)   //1ms prekid
{
	flag_tajmer0_prekid = 1;
	
	brojac_prekida_tajmera0++;
	brojac_prekida_tajmera0_debounce_half++; 
	brojac_prekida_tajmera0_debounce++;
		
	if(brojac_prekida_tajmera0 == 10)	//1ms * 10 = 10ms
	{
		brojac_prekida_tajmera0 = 0;
		flag_prekid_10ms = 1;				
		
	}
	
	if(brojac_prekida_tajmera0_debounce_half == DEBOUNCE_TIME_half)	//25ms
	{
		brojac_prekida_tajmera0_debounce_half = 0;
		flag_prekid_debounce_time_half = 1;
		PIND |= 1<<PIND2;	//toggle PD2
	}
	
	
	if(brojac_prekida_tajmera0_debounce == DEBOUNCE_TIME)	//1ms * 50 = 50ms
	{
		brojac_prekida_tajmera0_debounce = 0;
		flag_prekid_debounce_time = 1;
		PIND |= 1<<PIND3;	//toggle PD3
	}
	
}