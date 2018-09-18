/*
 * uart.c
 *
 * Created: 1.12.2017. 23.00.35
 *  Author: Jovica
 *  Modified: Kovacevic
 *
 * Potrebno u main funkciji omoguciti interapte globalno: sei()
 *
 */ 

#include <avr/interrupt.h>
#include <stdint.h>
#include <stdlib.h>
#include "uart.h"
#include <string.h>

volatile unsigned char poslato;
volatile unsigned char input_char;
volatile unsigned char input_buffer[INPUT_BUFF_SIZE], buff_size=0;

ISR(USART_TX_vect)
{
	poslato=0;
}

ISR(USART_RX_vect)
{
	input_char=UDR0;
	
	if (buff_size<INPUT_BUFF_SIZE)
	{
		input_buffer[buff_size]=input_char;
		buff_size++;
		input_buffer[buff_size]='\0';
	}
}

void uart_init()
{
	UCSR0A = 0b10;		//dupla brzina slanja
	UCSR0B = 0b11011000;	//RX complete interupt enable; TX complete interupt enable; Receiver enable; Transmitter enable
	UCSR0C = 0b110;			//asinhroni rezim, 8-bit, no parity, 1 stop bit
	UBRR0 = 207;	//Baud 9600 bps
}

void send_str(unsigned char *str)
{
	//funkcija za slanje stringova
	
	
	while (*str!='\0')
	{
		UDR0=*str;
		poslato=1;
		while (poslato);
		str++;
	}
}