/*
 * PWM timer1.c
 *
 * Created: 1.8.2017. 06.03.50
 * Author : Ko''
 */ 
#define F_CPU 16000000

#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>

volatile int adc_low, adc_high, adc_res, temp;


void AD_C_init()
{
	
	
	PRR = 0; //power reduction off
	
	ADMUX |= (1<<REFS0);  //5V referentni napon, ulazni pin A0
	ADCSRA =0b11101111; //0xFF ;  //ADC enable, auto trigger enable, ADC conv. complete interrupt enable, 128 prescaler
	
	//ADCL, ADCH(2 zadnja bita),
	
	ADCSRB = 0; //free runniing mode
	//DIDR0 = 0xFF; //digital pin disable - na svim analognim pinovima jer nisu potrebni (opciono)
	
	
}



ISR(ADC_vect)
{
	//ADCL mora biti prvi procitan
	adc_low = (int)ADCL;    //koriste se dva 8-bit registra jer je rezultat AD konverzije 10-bitan
	adc_high = (int)ADCH;
	
	/**stapam rezultate iz dva registra u jednu promenljivu na ovaj nacin**/
	if(adc_high==0)
	adc_res = adc_low;
	else if(adc_high==1)
	adc_res = 256 + adc_low;
	else if(adc_high==2)
	adc_res = 256+256 + adc_low;
	else if(adc_high==3)
	adc_res = 256+256+256 + adc_low;

	//faktor max 79
	//OCR1A = adc_res/12.8;
	/*OCR1A = adc_res*5.86;
	OCR1B = OCR1A;*/   //10000 max
	
	//OCR1A = adc_res/0.9765;
	//OCR1B = OCR1A;
	temp = adc_res/3.0;
	
	OCR1A = temp;
	
	if(OCR1A == 0)
	{
		OCR1B = 0;
	}
	else
	{
		OCR1B = OCR1A+50;  //+100 fora za dead time
	}
	
}





int main(void)
{
	/*
	DDRB |= (1<<1)|(1<<2); //OC1A izlaz
	
	TCCR1A |= (1<<COM1A1)|(1<<WGM11)|(1<<WGM10);
	TCCR1B |= (1<<WGM13)|(1<<WGM12)|(1<<CS10);
	OCR1A = 2000;
	ICR1 = 1000;
	
	zasto ne radi ako zelim da mi TOP bude u OCR1A, a
	faktor ispune da definise ICR1
	
	Vazno je primetiti da pri upisivanju faktora ispune, on mora biti
	manji ili jednak od broja koji definise TOP, logicno. Ako je veci
	bice konstantno zakucan na 5V.
    */
	
	
	
	    DDRB |= (1 << DDB1)|(1 << DDB2);
	    // PB1 and PB2 is now an output

	    ICR1 = 350;    //79 + 1 prescaler = 200kHz
	    // set TOP to 16bit

	    OCR1A = 40;
	    

	    OCR1B = OCR1A;
	    

	    //TCCR1A |= (1 << COM1A1); //|(1 << COM1B1);
		TCCR1A = 0b10110000;
	    //INVERTING

	    //TCCR1A |= (1 << WGM11);
	    TCCR1B |= (1 << WGM13);
	    // set Fast PWM mode using ICR1 as TOP
	    
	    TCCR1B |= (1 << CS10);
	    // START the timer with no prescaler
		
		AD_C_init();
		
		sei();
	
	
    while (1) 
    {
		
		//_delay_ms(100);
    }
}

