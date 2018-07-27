/*
 * eksterni_interapt.c
 *
 * Created: 2.12.2017. 00.27.26
 *  Author: Ko''
 */ 

#include "common.h"
#include "eksterni_interapt.h"

volatile unsigned int impulsi = 0;
volatile unsigned char smer_obrtanja = 0;
volatile double ugao = 0;
volatile unsigned int brojac_ext_interaptova = 0;

void eksternInt_init()
{
	DDRD &= ~(0b1100);		//PD2(INT0) i PD3(INT1) ulazi - hardwerski interapti
	
	EICRA = 0b11;		//rastuca ivica na INT0
	EIMSK = 0b1;			//dozvola prekida na INT0 
	
	
	
}

ISR(INT0_vect)
{
	
		//interapt opali na rastucu ivicu INT0; ovo sam uradio zbog velike rezolucije enkodera -1024 * 57.5 na izlazu, tj na tocku zbog 2 reduktora
	
	brojac_ext_interaptova++;
	

	
		if ( PIND & (1<<PORTD3) )	//smer 1; proverava se logicki nivo drugog kanala
		{
			smer_obrtanja = 1;
			impulsi++;
		} 
		else								//smer 2
		{
			smer_obrtanja = 2;
			impulsi--;	
		}
			
	
	
	
	
	
	if(impulsi > 58879)		//za apsolutno merenje ugla
		impulsi = 0;
	else if(impulsi < 0 )
		impulsi = 58879;
		
	//gornja ogranicenja sam stavio zato sto se tako najpreciznije dobija pozicija, tj. sa najmanjom greskom
	//problem se javlja kada ove impulse zelim da iskoristim za merenje brzine, a pri prelazu preko granicne vrednosti sa 58879 na 0 i obrnuto
	//zato sam ubacio jos jedan brojac koji broji prekide "brojac_ext_interaptova" relativno, a ne apsolutno
		
	//ugao = impulsi / 57.5;	//skaliranje, 58879 impulsa po krugu odgovara 360 stepeni tocka; !!!atmega se ovde zaglavi pri umerenoj frekvenciji prekida!!!
	//ugao = 100;
}

