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
	i2c_init();
	lcd1602_init();
	
	sei();
	
	char bafer[30];
	
	unsigned niz_mereni_napon[120];
	unsigned char br_uzoraka = 100;
	int i = 0;
	
	unsigned long AVG_mereni_napon = 0;
	
	
	lcd1602_clear();
	lcd1602_goto_xy(5,0);
	lcd1602_send_string("mV");
	lcd1602_goto_xy(5,1);
	lcd1602_send_string("mA");
	
    while (1) 
    {
		/*napomena: ovakaj dole pokusaj uzimanja srednje vrednosti ne pije vodu jer su petlje prebrze za ad rezultate, tako da se 
		zapravo mali broj ad rezultata obradi, mozda nekoliko. Potrebno je ovo provuci kroz tajmer*/
		AVG_mereni_napon =0;
		
		for(i=0; i<br_uzoraka; i++)
		{
			
			niz_mereni_napon[i] = mereni_napon;
		}
		
		for(i=0; i<br_uzoraka; i++)
		{
			
			AVG_mereni_napon += niz_mereni_napon[i];
		}
	
		AVG_mereni_napon = AVG_mereni_napon/br_uzoraka;
		
		
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
		itoa( AVG_mereni_napon , bafer, 10);
		send_str(bafer);
		lcd1602_goto_xy(0,0);
		lcd1602_send_string("     ");
		lcd1602_goto_xy(0,0);
		lcd1602_send_string(bafer);
		
		
		send_str("  ");
		itoa( merena_struja , bafer, 10);
		send_str(bafer);
		lcd1602_goto_xy(0,1);
		lcd1602_send_string("     ");
		lcd1602_goto_xy(0,1);
		lcd1602_send_string(bafer);
		
		/*
		send_str("  ");
		itoa( suma_greske/100 , bafer, 10);
		send_str(bafer);
		*/
		
		
		send_str("  ");
		itoa( Upravljanje*10 , bafer, 10);
		send_str(bafer);
		
		send_str("  ");
		itoa( 0 , bafer, 10);
		send_str(bafer);
		
		send_str("  ");
		itoa( 9000 , bafer, 10);
		send_str(bafer);
		
		
		/*
		send_str("  ");
		itoa( greska , bafer, 10);
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

