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
volatile uint8_t flag_prekid_50ms = 0;
volatile uint8_t flag_prekid_debounce_time = 0;
volatile uint8_t flag_prekid_debounce_time_half = 0;

volatile uint8_t brojac_prekida_tajmera0 = 0;
volatile uint8_t brojac_prekida_tajmera0_debounce = 0;
volatile uint8_t brojac_prekida_tajmera0_debounce_half = 0;

volatile uint16_t delay_timer = 0;

void tajmer0_init()
{
	TCCR0A = 0b10;		//CTC mode, TOP=OCR0A
	TCCR0B = 0b11;		//prescaler = 64
	OCR0A = 249;		//da bi se dobila frekvencija 1kHz odnosno prekid na svaki 1ms
	TIMSK0 = 0b10;		//compare match A interrupt enable
	
}

ISR(TIMER0_COMPA_vect)   //1ms prekid
{
	flag_tajmer0_prekid = 1;
	
	brojac_prekida_tajmera0++;
	brojac_prekida_tajmera0_debounce_half++; 
	brojac_prekida_tajmera0_debounce++;
	delay_timer++;		//overflow posle 65.5 sekundi, ali koga briga ne remeti normalan rad
		
	if(brojac_prekida_tajmera0 == 100)	//1ms * 50 = 50ms  !!!brojac je 8-bit znaci ide do max 255 LOLOOLOLOLO
	{
		brojac_prekida_tajmera0 = 0;
		flag_prekid_50ms = 1;
	}
	
	if(brojac_prekida_tajmera0_debounce_half == DEBOUNCE_TIME_half)	//3ms
	{
		brojac_prekida_tajmera0_debounce_half = 0;
		flag_prekid_debounce_time_half = 1;
	}
	
	
	if(brojac_prekida_tajmera0_debounce == DEBOUNCE_TIME)	//1ms * 6 = 6ms
	{
		brojac_prekida_tajmera0_debounce = 0;
		flag_prekid_debounce_time = 1;
	}
	
}