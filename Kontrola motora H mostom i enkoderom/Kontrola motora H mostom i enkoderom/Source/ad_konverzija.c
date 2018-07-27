/*
 * ad_konverzija.c
 *
 * Created: 1.12.2017. 22.09.18
 *  Author: Ko''
 */ 
#include "ad_konverzija.h"
#include "common.h"
#include "tajmer.h"


volatile int adc_res[4]={0,0,0,0}, adc_low, adc_high;
volatile int ref_brzina_sa_pot = 0;
volatile int ad_kanal = 0;

void ADC_init()
{
	
	
	PRR = 0;					//power reduction off
	
	ADMUX = (1<<REFS0);				//5V referentni napon, ulazni pin A0
	ADCSRA = 0b11101111;			//ADC enable, auto trigger enable, ADC conv. complete interrupt enable, 128 prescaler
	ADCSRB = 0b11;					//auto trigger - Timer0 compare match A. Startuje AD konverziju na svaki Timer interrupt
	
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
	
	
	ref_brzina_sa_pot = (adc_res[0] / 1.705) - 300; //skalirao bezveze da odgovara obrtajima, max 300 o/min otprilike u oba smera, -300 ofset zbog smera
	Kp = adc_res[1]/100.0;				//skalirano da Kp ide do max 1
	Ki = adc_res[2]/1000.0;				//da Ki ide do max 1
	
	
	ADCSRA &= ~(1<<ADEN);	//iskljucim adc da bi promena u ADMUX bila sigurna, po preporuci iz datasheet-a
	
	
	ad_kanal++;			//inkrementiraj kanal
	if(ad_kanal > 3)	//kreni opet od nule kad dodjes do poslednjeg
		ad_kanal = 0;
	
	//multipleksiranje ad ulaza; tj. promena ad kanala
	
	switch(ad_kanal)
	{
		case 0:
				ADMUX = 0b01000000;		//ref VCC 5V, kanal A0
		break;
		
		case 1:
				ADMUX = 0b01000001;		//ref VCC 5V, kanal A1
		break;
		
		case 2:
				ADMUX = 0b01000010;		//ref VCC 5V, kanal A2
		break;
		
		case 3:
				ADMUX = 0b01000011;		//ref VCC 5V, kanal A3
		break;
	}
	
	
	//napomena: promena ulaza se vrsi ovde, a kad naredni put uleti u ISR vrsi se konverzija tog kanala. 
	
	ADCSRA |= (1<<ADEN);	//ponovo dozvolim adc posle promene u ADMUX
	


}