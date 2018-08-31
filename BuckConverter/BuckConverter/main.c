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
	unsigned niz_napona_shunt[100];
	unsigned niz_struja_shunt[100];
	unsigned char br_semplova = 50;
	int i = 0;
	unsigned long napon_shunt_AVG = 0;
	unsigned long struja_shunt_AVG = 0;
	
    while (1) 
    {
		napon_shunt_AVG =0;
		struja_shunt_AVG =0;
		
		for(i=0; i<br_semplova; i++)
		{
			niz_napona_shunt[i] = merena_struja * 1.068;
			niz_struja_shunt[i] = merena_struja;
		}
		
		for(i=0; i<br_semplova; i++)
		{
			napon_shunt_AVG += niz_napona_shunt[i];
			struja_shunt_AVG += niz_struja_shunt[i];
		}
		
		napon_shunt_AVG = napon_shunt_AVG/br_semplova;
		struja_shunt_AVG = struja_shunt_AVG/br_semplova;
		
		
		if(flag_prekid_10ms == 1)		//slanje putem UART Tx na svakih 10ms. Podesava se u tajmer.c fajlu
		{
			flag_prekid_10ms = 0;
		
		
		//send_str("");
		/*
		itoa( ref_napon_sa_pot*100 , bafer, 10);
		send_str(bafer);
		*/
		/*
		send_str("");
		itoa( struja_shunt_AVG , bafer, 10);
		send_str(bafer);
		*/
		send_str("  ");
		itoa( ref_napon_sa_pot , bafer, 10);
		send_str(bafer);
		
		
		send_str("  ");
		itoa( mereni_napon , bafer, 10);
		send_str(bafer);
		
		send_str("  ");
		itoa( suma_greske/100 , bafer, 10);
		send_str(bafer);
		
		
		
		send_str("  ");
		itoa( Upravljanje*100 , bafer, 10);
		send_str(bafer);
		
		send_str("  ");
		itoa( 0 , bafer, 10);
		send_str(bafer);
		
		send_str("  ");
		itoa( 9000 , bafer, 10);
		send_str(bafer);
		/*
		send_str("  ");
		itoa( 10000 , bafer, 10);
		send_str(bafer);
		
		send_str("  ");
		itoa( 15000 , bafer, 10);
		send_str(bafer);
		
		send_str("  ");
		itoa( 20000 , bafer, 10);
		send_str(bafer);
		*/

		
	
		
		send_str("\n");
		
	
		
		}
    }
}

