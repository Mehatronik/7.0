/*
 * serijska proba.c
 * 
 *
 * Created: 26.5.2017. 04.18.09
 * Author : Kovach
 *
 */ 

#define F_CPU 16000000


#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>
#include <string.h>

#define BAUD 9600    //baud-rate [bps]
#define mojUBR (F_CPU/16/BAUD-1) //broj koji se upisuje u UBR high i low registar, da bi se dobio zeljeni baud-rate
#define buffer_size 128

void Rx_Tx_Enable();
void addSerial(char c);  //dodaje karakter u bafer

void serialWrite(char c[]);   //funkcija
void serialRead();            //funkcija


char serialBuffer[buffer_size]; //bafer 128 bita
uint8_t readPozicija = 0;
uint8_t writePozicija = 0;





int main(void)
{

	
	Rx_Tx_Enable();

	
	

	
	
	serialWrite("Hello world\n\r");
	//serialWrite("Dje ste Srbi\n\r");
	
	
    while (1) 
    {
		serialRead();
    }
}


void Rx_Tx_Enable()
{
	cli();
	
	UCSR0B = (1<<RXEN0)|(1<<TXEN0)|(1<<TXCIE0)|(1<<RXCIE0); //Rx, Tx enable, Tx interrupt enable, Rx interrupt enable
	
	//1 stop bit, no parity
	
	UCSR0C = (1<<UCSZ01)|(1<<UCSZ00); //Char size = 8 bit
	
	//UBR je podeljen u dva registra
	UBRR0H = mojUBR>>8;  //LSB od mojUBR
	UBRR0L = mojUBR;  //MSB od mojUBR
	
	sei();
	
}

void addSerial(char c)     //dodaje karakter u bafer
{
	
	serialBuffer[writePozicija] = c;    //upisi pristigli karakter na poziciju writePozicija
	writePozicija++;                    //kad upises inkrementiraj "pokazivac" pozicije
	
	
	if(writePozicija >= buffer_size) //kad dodjes do kraja vrati se na pocetak
		writePozicija = 0;
	
}

void serialWrite(char c[])
{
	
	for(uint8_t i=0; i<strlen(c); i++)
	{
		addSerial(c[i]);
	}
	
	if(UCSR0A & (1<<UDRE0))  //transmit buffer empty flag. ready to be written
		UDR0 = 0;
	
}

void serialRead()  //da ucitava u uC poslato sa kompa 
{
	
	if( (!(UCSR0A & (1<<UDRE0))) == 0 )
		addSerial(UDR0);
}


ISR(USART_TX_vect)
{
	//prekid usled transmit complete flag
	
	
	if(readPozicija != writePozicija)
	{
		UDR0 = serialBuffer[readPozicija];
		readPozicija++;
		
		if(readPozicija >= buffer_size)
			readPozicija=0;
		
	}

	
}

ISR(USART_RX_vect)
{
	
}