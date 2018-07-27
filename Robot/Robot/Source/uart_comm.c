/*
 * uart_comm.c
 *
 * Created: 17.11.2016 18:07:50
 *  Author: Tasevski
 */ 


#include "uart_comm.h"

volatile unsigned char poslato=0;
volatile unsigned char input_char;
volatile unsigned char input_buffer[INPUT_BUFF_SIZE],buff_size;

void init_uart()
{
	
	/* USARTE1( PE7 -> USARTE1_TX, PE6 -> USARTE1_RX ) */
	
	PORTE.DIR |= (1 << 7);		// Set pin PE7 as output
	PORTE.DIR &= ~(1 << 6);		// Set pin PE6 as input
	
	USARTE1.CTRLA |= (1 << 4 | 1 << 2);		// Enable receiver and transmitter interrupts at low level
	USARTE1.CTRLB |= (1 << 4 | 1 << 3);		// Enable receiver and transmitter
	USARTE1.CTRLC |= (1 << 1 | 1 << 0);		// No parity, 1 stop bit, 8 bit data size
	
	//baud 115.2k za bluetooth
	USARTE1.BAUDCTRLA =	131;  //110;		//Bsel lower		//12;	// Podesavanje bauda. Pogledati reference manual na stranama 233, 249 i 250.
	USARTE1.BAUDCTRLB = (-3)<<4;  //0b10110000;			//bilo (2 << 4);  

	
}

void send_str(char *str)
{
	while (*str != '\0')   
	{
		USARTE1.DATA=*str;
		poslato=1;
		while(poslato);
		str++;
	}
	
}

void send_chr(char *str)
{
	
		USARTE1.DATA=*str;
		poslato=1;
		while(poslato);
	
}

ISR(USARTE1_TXC_vect)
{
	poslato=0;
}

ISR(USARTE1_RXC_vect)
{
	input_char=USARTE1.DATA;
	
	if(buff_size<INPUT_BUFF_SIZE-1)
	{
		input_buffer[buff_size]=input_char;
		buff_size++;
		input_buffer[buff_size]='\0';
	}
}