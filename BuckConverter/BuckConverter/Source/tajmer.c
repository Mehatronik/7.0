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
volatile int16_t Upravljanje = 250;
volatile float Kp = 0.01, Ki = 0.007;
volatile int32_t greska = 0, suma_greske = 0;
const volatile int32_t limit_sume = 20000; 
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
		
		
		
	
		
		
		
		{
		//***********************merenje brzine treba obaviti unutar ISR jer je vremenski kriticno*****************************************
	
		//relativni_ugao = brojac_ext_interaptova / 57.5;	//skaliranje, 58800 impulsa po krugu odgovara 360 stepeni
		/*
		if(smer_obrtanja == 2)		//ODREDJUJEM NA KOJU STRANU SE VRTI
		{
			ugaona_brzina = stepeniPOms_to_rpm * relativni_ugao;				//posto se zna da merenje relativnog ugla traje 1ms, to je zapravo ugaona
															//brzina u stepen/1ms, a 166.67 je faktor konverzije izmedju stepeni/1ms i obrtaja/min
		}
		else
		{
			ugaona_brzina = -(stepeniPOms_to_rpm * relativni_ugao);	//ako je na drugu brzina je negativna
		}
				*/											
	
		
		
		/*
		greska = ref_napon_sa_pot - ugaona_brzina;
		
		Upravljanje = 250 + Kp * greska;		//Proporcionalni regulator; ofset 250 zbog h-mosta. Top je 500
		
		suma_greske += greska;			//suma greske
		
		
		Upravljanje += Ki * suma_greske;			//Integralno dejstvo sabiram sa prethodnim upravljanjem
		
		if(Upravljanje >= 500 )		//ogranicenje jer 500 u OCR1A registru daje maksimalan faktor ispune
									
		{
			Upravljanje = 500;
			suma_greske -= greska;		//ako vec imas max upravljanje nemoj vise povecavati sumu greske; zastita od wind up-a
		}
		else if(Upravljanje <= 0)	//ogranicenje, 0 daje max upravljanje u drugom smeru
		{
			Upravljanje = 0;
			suma_greske -= greska;		//drugi smer, obratna situacija
		}
		*/
		
		
		//OCR1A = Upravljanje;	//mogao sam i direktno da upisujem u OCR1A, ali sam dodao promenljivu 'Upravljanje' da bi bilo baferovano
		//OCR1B = OCR1A + 50;		//+50 za mrtvo vreme
		
		
		//if(OCR1A==0)
			//OCR1B = 0;
		

		//brojac_ext_interaptova =0;			//nuliram da bi brojanje bilo relativno
		}
	
	}
	
	/*Podsetnik: postoje inkrementalni i neki drugi PID zakon, pa ih malo prouci*/
	

			greska = ref_napon_sa_pot - mereni_napon;    //greska napona (u mV)
			
			Upravljanje = Kp*greska;	  //PID,      400 = max (~20V), 0 = min (0V)
			
			
			suma_greske += greska;
			
			if(suma_greske >= limit_sume)
				suma_greske = limit_sume;
			else if (suma_greske <= -limit_sume)
			{
				suma_greske = -limit_sume;
			}
			
			//Upravljanje += Ki * suma_greske;			//Integralno dejstvo sabiram sa prethodnim upravljanjem
			
			if(Upravljanje>=400)
			{
				Upravljanje = 400; //saturacija, tj ogranicenje
				suma_greske -= greska;		//ako vec imas max upravljanje nemoj vise povecavati sumu greske; zastita od wind up-a
			}
			
			if(Upravljanje<=0)
			{
				Upravljanje = 0;
				suma_greske -= greska;		//ako vec imas min upravljanje nemoj vise povecavati sumu greske; zastita od wind up-a
			}
			
			
			OCR1A = Upravljanje;
	
	
}