/*
 * Kontrola motora H mostom i enkoderom.c
 *
 * Created: 1.12.2017. 22.01.42
 * Author : Kovacevic Vladan
 */ 

#define  F_CPU 16000000
#include "common.h"
#include "ad_konverzija.h"
#include "pwm.h"
#include "uart.h"
#include "eksterni_interapt.h"
#include "tajmer.h"



char bafer[30];


int main(void)
{
		DDRB |= 1<<PORTB5;	//dig13, radi provere frekvencije; izlaz

		ADC_init();
		pwm_init();
		uart_init();
		eksternInt_init();
		tajmer0_init();
		
		sei();
	
	
	send_str("Start \n");
	
	
    while (1) 
    {	
		
		
		if(flag_prekid_10ms == 1)		//slanje putem UART Tx na svakih 20ms. Podesava se u tajmer.c fajlu
		{
			flag_prekid_10ms = 0;
			
			
			
			itoa((int)(ugaona_brzina), bafer, 10);
			send_str(bafer);
			send_str(" o/min       ");
			

			
			itoa((ref_brzina_sa_pot), bafer, 10);
			send_str(bafer);
			send_str(" \n ");
			//negde postoji greska pri merenju brzine, prikazuje se oko duplo veca brzina
			
			/*
			itoa(((int)impulsi/57.5), bafer, 10);
			send_str(bafer);
			send_str("\n  ");
			
			
			itoa((int)OCR1A*20-500, bafer, 10);
			send_str(bafer);
			send_str(" \n");
			*/
			
			/*
			itoa(adc_res[0], bafer, 10);
			send_str(bafer);
			send_str("  ");
			
			itoa(adc_res[1], bafer, 10);
			send_str(bafer);
			send_str("  ");
			
			itoa(adc_res[2], bafer, 10);
			send_str(bafer);
			send_str("  ");
			
			itoa(adc_res[3], bafer, 10);
			send_str(bafer);
			send_str("\n");
			*/
			
			
		}
		
		
		
    }
}



