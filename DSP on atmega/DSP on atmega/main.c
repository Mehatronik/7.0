/*
 * DSP on atmega.c
 *
 * Created: 20.9.2018. 01.58.32
 * Author : Kovacevic
 */ 

#define F_CPU 16000000UL

#include "DSPmega.h"
#include <avr/io.h>
#include "uart.h"
#include <util/delay.h>

int main(void)
{
    char buffer[15] = {0};
	
	uart_init();
	ADCSetup();
	PWMSetup();
	buttonSetup();
	
	DDRB |= 1<<PORTB5; //led
	
	
    
	//effects&= ~PITCHUP;
	//effects|=BITCRUSH;
    while (1) 
    {
		
		sprintf(buffer, "%d", ADCdata);
		send_str(buffer);
		send_str("\n");
		
		//effects|=ECHO;
		/*
		if (PINB & (1<<PORTB6)){
			effects&= ~PITCHUP;
			}else{
			effects|=PITCHUP;
		}
		if (PINB & (1<<PORTB7)){
			effects&= ~PITCHDOWN;
			}else{
			effects|=PITCHDOWN;
		}
		if (PIND & (1<<PORTD6)){
			effects&= ~BITCRUSH;
			}else{
			effects|=BITCRUSH;
		}
		
		if (PIND & (1<<PORTD7)){
			effects&= ~ECHO;
			}else{
			effects|=ECHO;
		}
		if (PINB & (1<<PORTB0)){
			effects&= ~PHASOR;
			}else{
			effects|=PHASOR;
		}
		if (PINB & (1<<PORTB1)){
			effects&= ~LOWPASS;
			}else{
			effects|=LOWPASS;
		}
		*/
		
		
		
    }
}

