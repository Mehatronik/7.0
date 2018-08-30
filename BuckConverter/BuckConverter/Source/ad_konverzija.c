/*
 * ad_konverzija.c
 *
 * Created: 1.12.2017. 22.09.18
 *  Author: Ko''
 */ 
#include "ad_konverzija.h"
#include "common.h"
#include "tajmer.h"




volatile unsigned adc_res[4][BR_SEMPLOVA]= { 0 };   // [ ad kanal ][ sempl tog kanala ]
		
volatile uint8_t brojac_sempla = 0;
volatile static uint8_t i=0, j=0;


volatile unsigned ref_napon_sa_pot = 0, mereni_napon = 0, merena_struja = 0;	//ovde ce biti upisane prosecne vrednosti par semplova
volatile uint8_t adc_low=0, adc_high=0;
volatile uint8_t ad_kanal = 0;
volatile const uint8_t del = 102.966;
volatile const uint8_t del2 = 51.15;
volatile const uint8_t del3 = 4.5454;

void ADC_init()
{
	DDRD |= 1<<DDB7;
	
	PRR = 0;					//power reduction off
	
	ADMUX = 0b11000000;		        //ref internal 1.1V, kanal A0
	ADCSRA = 0b11101110;			//ADC enable, start conversion, auto trigger enable, ADC conv. complete interrupt enable, 64 prescaler = 250kHz, a preporuka je do 200kHz
	ADCSRB = 0b0;					//0b11
	
	ad_kanal = 0; //prvo citam A0
	
}

ISR(ADC_vect)
{
	/*
	AD konverzija
	ISR okine kada je gotova konverzija
	*/
	
	
	
	
	
	
	
	{
		

	//ADCL mora biti prvi procitan
	adc_low = (uint8_t)ADCL;    //koriste se dva 8-bit registra jer je rezultat AD konverzije 10-bitan
	adc_high = (uint8_t)ADCH;
	
		/**stapam rezultate iz dva registra u jednu promenljivu na ovaj nacin**/
		/**rezultat ad konverzije je 10-bit-an, tj. od 0 do 1023**/
		
		if(adc_high==0)
			adc_res[ad_kanal][brojac_sempla] = adc_low;
		else if(adc_high==1)
			adc_res[ad_kanal][brojac_sempla] = 256 + adc_low;
		else if(adc_high==2)
			adc_res[ad_kanal][brojac_sempla] = 512 + adc_low;	//256*2
		else if(adc_high==3)
			adc_res[ad_kanal][brojac_sempla] = 768 + adc_low;	//256*3
			
	}
	
	//suma za dati kanal i izvucen prosek
	if (ad_kanal==0)
	{
			ref_napon_sa_pot = 0;
			
			for (i=0; i<BR_SEMPLOVA; i++)
			{
				ref_napon_sa_pot += adc_res[0][i];
			}
			ref_napon_sa_pot = (ref_napon_sa_pot/BR_SEMPLOVA) * 19.55;
	}

	
	//ref_naponi[brojac_sempla] = adc_res[0];
	
	
	//ref_napon_sa_pot = (adc_res[0] * 19.55);      //  1023 = 20000mV  zadati napon sa potenciometra, skalirano na milivolte i miliampere da se izbegne deljenje
	
	//OCR1A = ref_napon_sa_pot * 20.0;  //top = 400
	OCR1A = 150;
	
	/***** Vref 1.1V je zapravo 1.093 V ******/
	//merena_struja = adc_res[2] * 4.856;                   // / 935.96) * 4.545;         // 1/0.22=4.545				//1023 = 5A (1.1V ref, preko 0.22Ohm otpornika)
	//mereni_napon = (adc_res[1] * 19.55) - (merena_struja * 1.068);				//1023 = 20V  (1.1V referenca) preko razdelnika
	
	{
	
	ADCSRA &= ~(1<<ADEN);	//iskljucim adc da bi promena u ADMUX bila sigurna, po preporuci iz datasheet-a
	
	
	ad_kanal++;			//inkrementiraj kanal
	if(ad_kanal >= 3)	//kreni opet od nule kad dodjes do poslednjeg; 2, da citam samo prva tri
	{
		ad_kanal = 0;
		
		brojac_sempla++;
		if(brojac_sempla >= BR_SEMPLOVA) //5 sempla
			brojac_sempla = 0;
	}
	//multipleksiranje ad ulaza; tj. promena ad kanala
	
	switch(ad_kanal)
	{
		case 0:
				ADMUX &= ~(0b11);		//00;ref internal 1.1V, kanal A0
		break;
		
		case 1:
				ADMUX &= ~(0b10);		//01;ref internal 1.1V, kanal A1
				ADMUX |= (0b1);
		break;
		
		case 2:
				ADMUX &= ~(0b1);		//10;ref internal 1.1V, kanal A2
				ADMUX |= (0b10);
		break;
		
		case 3:
				ADMUX |= (0b11);		//11;ref internal 1.1V, kanal A3
		break;
	}
	
	
	//napomena: promena ulaza se vrsi ovde, a kad naredni put uleti u ISR vrsi se konverzija tog kanala. 
	
	ADCSRA |= (1<<ADEN)|(1<<ADSC);	//ponovo dozvolim adc posle promene u ADMUX i pokrenem opet prvu konverziju da bi htelo da radi u Free running
	
	}
	
	PIND |= 1<<7;       //togle za osciloskop

}