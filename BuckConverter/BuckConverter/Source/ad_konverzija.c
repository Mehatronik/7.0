/*
 * ad_konverzija.c
 *
 * Created: 1.12.2017. 22.09.18
 *  Author: Ko''
 */ 
#include "ad_konverzija.h"
#include "common.h"
#include "tajmer.h"


volatile int adc_res[4]={0,0,0,0}; 
volatile float ref_napon_sa_pot = 0, mereni_napon = 0, merena_struja = 0;
volatile int adc_low=0, adc_high=0;
volatile int ad_kanal = 0;

void ADC_init()
{
	DDRD |= 1<<DDB7;
	
	PRR = 0;					//power reduction off
	
	ADMUX = (1<<REFS0);				//5V referentni napon, ulazni pin A0
	ADCSRA = 0b11101111;			//ADC enable, start conversion, auto trigger enable, ADC conv. complete interrupt enable, 128 prescaler
	ADCSRB = 0b0;					//0b11
	
	ad_kanal = 0; //prvo citam A0
	
}

ISR(ADC_vect)
{
	/*
	AD konverzija
	ISR okine kada je gotova konverzija
	*/
	
	
	//ADCL mora biti prvi procitan
	adc_low = (int)ADCL;    //koriste se dva 8-bit registra jer je rezultat AD konverzije 10-bitan
	adc_high = (int)ADCH;
	
		/**stapam rezultate iz dva registra u jednu promenljivu na ovaj nacin**/
		/**rezultat ad konverzije je 10-bit-an, tj. od 0 do 1023**/
		if(adc_high==0)
			adc_res[ad_kanal] = adc_low;
		else if(adc_high==1)
			adc_res[ad_kanal] = 256 + adc_low;
		else if(adc_high==2)
			adc_res[ad_kanal] = 512 + adc_low;	//256*2
		else if(adc_high==3)
			adc_res[ad_kanal] = 768 + adc_low;	//256*3
	
	
	ref_napon_sa_pot = (adc_res[0] / 51.15); //  1023 = 20V  zadati napon sa potenciometra
	
	OCR1A = ref_napon_sa_pot * 20.0;  //top = 400
	
	merena_struja = (adc_res[2] / 930.0) * 4.5454;         // 1/0.22=4.545				//1023 = 5A (1.1V ref, preko 0.22Ohm otpornika)
	mereni_napon = (adc_res[1] / 51.15) - (merena_struja / 4.5454);				//1023 = 20V  (1.1V referenca) preko razdelnika
	
	ADCSRA &= ~(1<<ADEN);	//iskljucim adc da bi promena u ADMUX bila sigurna, po preporuci iz datasheet-a
	
	
	ad_kanal++;			//inkrementiraj kanal
	if(ad_kanal > 3)	//kreni opet od nule kad dodjes do poslednjeg
		ad_kanal = 0;
	
	//multipleksiranje ad ulaza; tj. promena ad kanala
	
	switch(ad_kanal)
	{
		case 0:
				ADMUX = 0b11000000;		//ref internal 1.1V, kanal A0
		break;
		
		case 1:
				ADMUX = 0b11000001;		//ref internal 1.1V, kanal A1
		break;
		
		case 2:
				ADMUX = 0b11000010;		//ref internal 1.1V, kanal A2
		break;
		
		case 3:
				ADMUX = 0b11000011;		//ref internal 1.1V, kanal A3
		break;
	}
	
	
	//napomena: promena ulaza se vrsi ovde, a kad naredni put uleti u ISR vrsi se konverzija tog kanala. 
	
	ADCSRA |= (1<<ADEN)|(1<<ADSC);	//ponovo dozvolim adc posle promene u ADMUX i pokrenem opet prvu konverziju da bi htelo da radi u Free running
	
	PIND |= 1<<7;

}