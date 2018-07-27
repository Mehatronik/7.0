/*
 * serijska.c
 *
 * Created: 25.5.2017. 22.56.58
 * Author : Ko
 */ 
#define F_CPU 16000000


#include <avr/io.h>
#include <util/delay.h>


#define BAUD 9600
#define BRC ((F_CPU/16/BAUD) - 1)



int main(void)
{
	int i;
	char niz[10] = "Vladan ";
	
	
	
	UBRR0H = BRC>>8;
	UBRR0L = BRC;
	
	UCSR0B = 1<<TXEN0;
	UCSR0C = (1<<UCSZ01)|(1<<UCSZ00);
	
    
    while (1) 
    {
		

		for(i=0; i<=6; i++)
		{
			while( !( UCSR0A & (1<<UDRE0)) );
			
			UDR0= niz[i];
			if(i==6)
				_delay_ms(10);
			
			
			
		}
		
		
		
    }
}

