/*
 * RxTx.c
 *
 * Created: 3.6.2017. 00.30.42
 * Author : Kovacevic
 */ 


#define F_CPU 16000000


#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>
#include <string.h>

#define BAUD 9600    //baud-rate [bps]
#define mojUBR (F_CPU/16/BAUD-1) //broj koji se upisuje u UBR high i low registar, da bi se dobio zeljeni baud-rate


volatile char niz[100];
volatile int i;
volatile int adc_low, adc_high, adc_res;




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
	
	ADMUX |= (1<<REFS1)|(1<<REFS0);  //1.1V referentni napon, ulazni pin A0
	ADCSRA = 0xFF ;  //ADC enable, auto trigger enable, ADC conv. complete interrupt enable, sample 125kHz (128 prescaler)
	
	//ADCL, ADCH(2 zadnja bita), 
	
	ADCSRB = 0; //free runniing mode
	//DIDR0 = 0xFF; //digital pin disable - na svim analognim pinovima jer nisu potrebni (opciono)
	
	
}

void PWM_init()
{
	DDRB |= (1 << DDB1)|(1 << DDB2);
	// PB1 and PB2 is now an output

	ICR1 =100; //399;
	// set TOP to 16bit

	OCR1A = 50;
	// set PWM for 25% duty cycle @ 16bit

	OCR1B = 50;
	// set PWM for 75% duty cycle @ 16bit

	TCCR1A |= (1 << COM1A1)|(1 << COM1B1);
	// set none-inverting mode - valjda INVERTING, ili??

	TCCR1A |= (1 << WGM11);
	TCCR1B |= (1 << WGM12)|(1 << WGM13);
	// set Fast PWM mode using ICR1 as TOP
	
	TCCR1B |= (1 << CS10);
	// START the timer with no prescaler
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
		
	//adc_res = 1024 max
	
	OCR1A = adc_res/5; //2.56;
	
}




int main(void)
{


	cli();
	

	AD_C_init();
	//Rx_Tx_Init();
	PWM_init();
	
	sei();

	while(1)
	{
		
		
		
		/*while( !(UCSR0A &(1<<UDRE0)) );
		UDR0 = adc_res;
		
		_delay_ms(50);*/
		
	}
	
    
}

