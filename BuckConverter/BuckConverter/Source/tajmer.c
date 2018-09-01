/*
 * tajmer.c
 *
 * Created: 2.12.2017. 22.08.59
 *  Author: Ko''
 */ 

//Tajmer koji ce da okida svaku 1ms

#include "tajmer.h"
#include "common.h"
//#include "eksterni_interapt.h"
#include "ad_konverzija.h"

volatile unsigned char flag_tajmer0_prekid = 0, flag_prekid_10ms = 0;
volatile unsigned int brojac_prekida_tajmera0;
volatile int16_t Upravljanje = 0;
volatile float Kp = 0.002, Ki = 0.003, Kd=0.085;  //default Kp=0.005, Ki=0.003
volatile int32_t greska = 0, suma_greske = 0, greska_prethodna = 0;
const volatile int32_t limit_sume = 500000;  //cisto da se ogradim od overflow-a
//const volatile double stepeniPOms_to_rpm = 16.67;		//faktor konverzije izmedju izmerenog broja stepeni u prozoru od 10ms u obrtaje po minutu
//volatile double relativni_ugao = 0;

void tajmer0_init()
{
	TCCR0A = 0b10;		//CTC mode, TOP=OCR0A
	TCCR0B = 0b11;		//prescaler = 64
	OCR0A = 249;		//da bi se dobila frekvencija 1kHz odnosno prekid na svaki 1ms
	TIMSK0 = 0b10;		//compare match A interrupt enable
	
	DDRB |= 1<<PINB5;
}

ISR(TIMER0_COMPA_vect)   //1ms prekid
{
	flag_tajmer0_prekid = 1;
	
	brojac_prekida_tajmera0++;	//prekid okida svaki 1ms
	
		
	if(brojac_prekida_tajmera0 == 10)
	{
		brojac_prekida_tajmera0 = 0;
		flag_prekid_10ms = 1;
		PINB |= 1<<PINB5;					//toogle pin 5 - DIG13, test da vidim da li je korektna frekvencija
		
	}
		

	
	/*Podsetnik: postoje inkrementalni i neki drugi PID zakon, pa ih malo prouci*/
	


			greska_prethodna = greska; //zapamtim proslu gresku pre racunanja nove

			greska = (int32_t)ref_napon_sa_pot - mereni_napon;    //greska napona (u mV)
																 //desavao se overflow zato sto je ref_napon unsigned, posle kastovanja sve u redu
				
			Upravljanje = Kp*greska;	  //Proporcionalno dejstvo,      400 = max (~20V), 0 = min (0V)
			
			
			suma_greske += greska;
			
			if(suma_greske >= limit_sume)
				suma_greske = limit_sume;
			else if (suma_greske <= 0)
				suma_greske = 0;
			
			
			Upravljanje += Ki * suma_greske;		//Integralno dejstvo sabiram sa P dejstvom
			
			
			Upravljanje += Kd * (greska - greska_prethodna);
			
			
			if(Upravljanje>=400)
			{
				Upravljanje = 400; //saturacija, tj ogranicenje
				suma_greske -= greska;		//ako vec imas max upravljanje nemoj vise povecavati sumu greske; zastita od wind up-a
			}
			
			else if(Upravljanje<=0)
			{
				Upravljanje = 0;
				suma_greske -= greska;		//ako vec imas min upravljanje nemoj vise povecavati sumu greske; zastita od wind up-a
			}
			
			
			OCR1A = (uint16_t)Upravljanje;
	
	
}