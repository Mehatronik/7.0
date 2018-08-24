/*
 * BuckConverter.c
 *
 * Created: 24.8.2018. 01.14.39
 * Author : Ko''
 */ 

#define  F_CPU 16000000
#include "common.h"
#include "ad_konverzija.h"
#include "pwm.h"
#include "uart.h"
#include "tajmer.h"
#include <util/delay.h>



int main(void)
{
    ADC_init();
	uart_init();
	pwm_init();
	tajmer0_init();
	
	
	sei();
	
	char bafer[30];
	
    while (1) 
    {
		
		if(flag_prekid_10ms == 1)		//slanje putem UART Tx na svakih 20ms. Podesava se u tajmer.c fajlu
		{
			flag_prekid_10ms = 0;
		
		
		send_str("pot:");
		itoa( ref_napon_sa_pot*100 , bafer, 10);
		send_str(bafer);
		
		
		send_str("V  napon:");
		itoa( mereni_napon*100 , bafer, 10);
		send_str(bafer);
		
		send_str("V  struja:");
		itoa( merena_struja*100 , bafer, 10);
		send_str(bafer);
		
		
		send_str("A\n");
		
		}
    }
}

