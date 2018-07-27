/*
 * Casovnik.c
 *
 * Created: 25.7.2017. 22.52.30
 * Author : Kovachevich
 */ 

#define F_CPU 16000000

#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>
#include <string.h>

#define BAUD 9600
#define mojUBR (F_CPU/16/BAUD-1)



void timer_init();
void uart_init();
void SerialPrintInt();

volatile int sek=0, min=0, sat=0;




int main(void)
{
    DDRB |= 1<<PORTB5;
	
	
	int broj = 10;
	
	timer_init();
	uart_init();
	
	
	sei();
	
	
    while (1) 
    {
		
		if(sek>=60)
		{
			sek=0;
			min++;
		}
		
		if(min>=60)
		{
			min=0;
			sat++;
		}
		
		if(sat>=24)
		{
			sat=0;
		}
		
		
		
		SerialPrintInt(sat);
		while (!(UCSR0A & (1<<UDRE0)));
		UDR0 = 0x3A; //dvotacka
		
		
		
		SerialPrintInt(min);
		while (!(UCSR0A & (1<<UDRE0)));
		UDR0 = 0x3A; //dvotacka
		
		SerialPrintInt(sek);
		while (!(UCSR0A & (1<<UDRE0)));

		while (!(UCSR0A & (1<<UDRE0)));
		UDR0 = '\n';
			
		_delay_ms(50);
		
		
    }
	return 0;
}

ISR(TIMER1_COMPA_vect)
{
	//PORTB ^= 1<<5;
	
	sek++;
	
	/*if(sek>=60)
	{
		sek=0;
		min++;
	}
	
	if(min>=60)
	{
		min=0;
		sat++;
	}
	
	if(sat>=24)
	{
		sat=0;
	}*/
	
}

void timer_init()
{
		TCCR1A = 0b0;
		TCCR1B |= 0b1100; //0b1100
		
		
		
		OCR1A =62498;   //!!VRLO BITNO!!  u datasheet-u se navodi formula kojom se odredjuje frekvencija f = fclk/(2N(OCRxn-1)), gde je veoma bitno zapaziti
		// da se radi o "waveform" frekvenciji! Odatle dolazi ona dvojka u delitelju. Dakle, ukoliko se zele prekidi u odredjenim trenucima
		//dvojka se izbacuje iz formule
		
		//OCR1AL = 0x22; - bitno je i da broj u ovaj registar uneses odjednom, a ne ovako pojedinacno. Ovako se pravi potpuno pogresna frekvencija
		//OCR1AH = 0xF4; - u datasheet-u je naglaseno da se baci pogled na upisivanje u 16-bitne registre. Ovde takve zavrzlame nisu potrebne jer su programeri
		//                 Atmela resili to maskama, tako da korisnik samo upise kao da upisuje u obicni 8-bitni registar.
		
		TIMSK1 = 0b10;
}

void uart_init()
{
	UBRR0H = (unsigned char)(mojUBR>>8);
	UBRR0L = (unsigned char)mojUBR;
	
	UCSR0B |= 1<<TXEN0;					//tx enable
	UCSR0C |= (1<<UCSZ01)|(1<<UCSZ02);	//8 data bits
	
}

void SerialPrintInt(int a)
{
			/*
			//ispis na komp preko UART-a
			
			/* Wait for empty transmit buffer 
			while (!(UCSR0A & (1<<UDRE0)));
			
			/* Put data into buffer, sends the data 
			UDR0 = (unsigned char)sek+48;
			
			while (!(UCSR0A & (1<<UDRE0)));
			UDR0 = '\n';
			
			_delay_ms(100);
			*/
			
			
			int pristigliBroj = a;
			
			if(a<10)
			{
				while (!(UCSR0A & (1<<UDRE0)));
				UDR0 = a+48;
			}
			
			if(a>9)
			{
				
				while (!(UCSR0A & (1<<UDRE0)));
				UDR0 = (a/10)+48;				

				while (!(UCSR0A & (1<<UDRE0)));
				UDR0 = (a%10)+48;					
			}
			
}
