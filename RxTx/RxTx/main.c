/*
 * RxTx.c
 *
 * Created: 3.6.2017. 00.30.42
 * Author : Ko''
 */ 


#define F_CPU 16000000


#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>
#include <string.h>

#define BAUD 9600    //baud-rate [bps]
#define mojUBR (F_CPU/16/BAUD-1) //broj koji se upisuje u UBR high i low registar, da bi se dobio zeljeni baud-rate


volatile int adc_low, adc_high, adc_res;
volatile int pwm_dc; //u procentima za potrebe ispisa


void IO_init()
{
	DDRB |= ((1<<PORTB2)|(1<<PORTB3)) ; //PB2,PB3 izlazi (10, 11)
	
	
}

void Rx_Tx_Init()
{
	
		UCSR0B = (1<<RXEN0)|(1<<TXEN0); //Rx, Tx enable, Tx interrupt enable, Rx interrupt enable      (1<<TXCIE0)|(1<<RXCIE0)
		
		//1 stop bit, no parity
		
		UCSR0C = (1<<UCSZ01)|(1<<UCSZ00); //Char size = 8 bit
		
		//UBR je podeljen u dva registra
		UBRR0H = mojUBR>>8;  //LSB od mojUBR
		UBRR0L = mojUBR;  //MSB od mojUBR

}

void AD_C_init()
{
	
	
	PRR = 0; //power reduction off
	
	ADMUX |= (1<<REFS0);  //AVcc referentni napon, ulazni pin A0
	ADCSRA = 0xFF ;  //ADC enable, auto trigger enable, ADC conv. complete interrupt enable; (1<<ADEN)|(1<<ADATE)|(1<<ADIE)|(1<<ADSC)

	ADCSRB = 0; //free runniing mode
	//DIDR0 = 0xFF; //digital pin disable - na svim analognim pinovima jer nisu potrebni (opciono)
	
	
}

void timer2_init()
{
	
	
	/*** prema datasheet-u fast pwm pale bitovi WGM20 i WGM21. Iz nekog razloga oni mi pale phase correct mode, a WGM20 koji bi 
	**** trebao da pali phase correct pali fast pwm. Dakle obrnuto od datasheet-a 
	*/
	
	TCCR2A |= ( (1<<COM2A1)|(1<<WGM20) );  //pwm non-inverted na OC2A pinu (digital 11), TOP = 0xFF (gornja vrednost brojaca),, START Brojaca    |(1<<WGM21)
	TCCR2B = 0b010;  //010-prescaler fclk/8*256   f=7.8kHz
	OCR2A = 0;           //fakticki faktor ispune
	TIMSK2 |= 1<<TOIE2;        //dozvola prekida tajmera #2 usled poredjenja sa OCR2A, tj. dostizanja tog broja
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
		adc_res = 512 + adc_low;  //256+256
	else if(adc_high==3)
		adc_res = 768 + adc_low;  //256+256+256
		
	adc_res = adc_res/4.0; //skaliram 1024 na 256
	pwm_dc = (adc_res/255.0)*100; //skalirano na procente za potrebe ispisa
	
}

ISR(TIMER2_OVF_vect)
{
	
	OCR2A = adc_res; //upis faktora ispune
	
}


int main(void)
{


	cli();
	
	IO_init();
	AD_C_init();
	Rx_Tx_Init();
	timer2_init();
	
	sei();

	while(1)
	{
		
		while( !(UCSR0A &(1<<UDRE0)) ); //cekaj dok ne bude Ready to transmit
		UDR0 = pwm_dc;   //ispis vrednosti konverzije u procentima
		
		//_delay_ms(250);
	}
	
    
}

