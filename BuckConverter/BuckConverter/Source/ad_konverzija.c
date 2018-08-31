/*
 * ad_konverzija.c
 *
 * Created: 1.12.2017. 22.09.18
 *  Author: Ko''
 */ 
#include "ad_konverzija.h"
#include "common.h"
#include "tajmer.h"




volatile uint16_t adc_res[4][BR_SEMPLOVA]= { 0 };   // [ ad kanal ][ sempl tog kanala ] = [red][kolona]
													//u ad_konv.h se menja BR_Semplova		
volatile uint8_t brojac_sempla = 0;

volatile uint16_t ref_napon_sa_pot = 0, merena_struja = 0;	//ovde ce biti upisane prosecne vrednosti par semplova
volatile int16_t mereni_napon = 0;                          //signed, zbog mogucnosti da mozda ode negativno, dole kod racunanja
volatile uint8_t adc_low=0, adc_high=0;
volatile uint8_t ad_kanal = 0;


volatile void sumator();
volatile void _2x8bit_reg_2_1x16bit_reg();

void ADC_init()
{
	DDRD |= 1<<DDB7;
	
	PRR = 0;						//power reduction off
	
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
	
	
	_2x8bit_reg_2_1x16bit_reg();   //upis adc rezultata u matricu



	
	if (ad_kanal==0)
			sumator();      //suma za dati kanal i izvucen prosek i upisan u odg promenljivu
	else if (ad_kanal==1)		
			sumator();
	else if (ad_kanal==2)
			sumator();
	
	
	
	
	//OCR1A = ref_napon_sa_pot * 20.0;  //top = 400
	//OCR1A = 200;

	
	
	ADCSRA &= ~(1<<ADEN);	//iskljucim adc da bi promena u ADMUX bila sigurna, po preporuci iz datasheet-a
	
	
	ad_kanal++;			//inkrementiraj kanal
	if(ad_kanal > 2)	//kreni opet od nule kad dodjes do poslednjeg; 2, da citam samo prva tri
	{
		ad_kanal = 0;
		
		brojac_sempla++; //kad dodjes do kraja kanala predji na sledeci red semplova
		if(brojac_sempla >= BR_SEMPLOVA) //kad dodjes do kraja kreni opet od nule
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
		
		default: {}
		//postojao je problem ako hocu da menjam i referencu kada prebacujem kanal, da mi daje bezveze rezultate.
		//mislim da u datasheetu pise da prvo citanje posle menjanja reference moze biti lose	
	}
	
	
	//napomena: promena ulaza se vrsi ovde, a kad naredni put uleti u ISR vrsi se konverzija tog kanala. 
	
	ADCSRA |= (1<<ADEN)|(1<<ADSC);	//ponovo dozvolim adc posle promene u ADMUX i pokrenem opet prvu konverziju da bi htelo da radi u Free running
	
	
	
	PIND |= 1<<7;       //togle za osciloskop

}



volatile void sumator()
{
	//radi sumu po kolonama matrice, tj po kanalu, izvlaci prosek i upisuje u odgovarajucu promenljivu
	volatile static uint8_t i=0;


	
	if (ad_kanal == 0) //ref_napon_sa_pot
	{
		ref_napon_sa_pot = 0; //nuliram da ne bi uticalo na sumiranje i prosek i stvarne vrednosti
		
		for (i=0; i<BR_SEMPLOVA; i++)
		{
			ref_napon_sa_pot += adc_res[ad_kanal][i];		//suma po nultoj koloni (nultom kanalu)
		}
		ref_napon_sa_pot = (ref_napon_sa_pot/BR_SEMPLOVA) * 19.613;  //1023 = 20000 mV
		
		
	}
	else if (ad_kanal == 1)	//merena struja
	{
		merena_struja = 0;
		
		for (i=0; i<BR_SEMPLOVA; i++)
		{
			merena_struja += adc_res[ad_kanal][i];
		}
		merena_struja = (merena_struja/BR_SEMPLOVA) * 4.895;  // 1023 = 1093 mV, R=220 mOhm, I=V/R = 4968 mA (MAX); izracunato i skalirano da se za max input dobija ~5A
															  //malo je kompenzovan mnozitelj merenjem pomocu voltmetra
															 /***** Vref 1.1V je zapravo 1.093 V ******/
	}
	else if (ad_kanal == 2)	//mereni napon
	{
		mereni_napon = 0;
		
		for (i=0; i<BR_SEMPLOVA; i++)
		{
			mereni_napon += adc_res[ad_kanal][i];
		}
		mereni_napon = ((mereni_napon/BR_SEMPLOVA) * 19.613) - (merena_struja * 0.22);  //1023 = 20000 mV - naposnki pad preko sant otpornika (220 mOhm)
																						//mnozenje napona isto malo komenzovano-kalibrisano
																				//mozda postoji sansa da mereni napon ode negativno ako se nekako desi da napon na
																				//santu bude veci od merenog napona - npr u slucaju obrnutog povezivanja baterija
																				//na buck, ili pak zbog ne bas real time podataka sa ad ulaza jer im uzimam prosek
																				//Svakako trebam ovo jos malo razmotriti
	}


}



volatile void _2x8bit_reg_2_1x16bit_reg()
{
	
	//upis adc rezultata u matricu
	
	
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