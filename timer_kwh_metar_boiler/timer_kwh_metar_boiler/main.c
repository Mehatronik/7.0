/*
 * timer_kwh_metar_boiler.c
 *
 * Created: 26.4.2019. 19.26.28
 * Author : Kovacevic
 */ 

#include "comm.h"
#include "rtc_ds3231.h"
#include "pin_change_interrupt.h"
#include "tajmer.h"
#include "i2cmaster.h"
#include "lcd1602.h"
#include "ad_konverzija.h"
#include "uart.h"
#include "tasteri.h"
#include <stdio.h>      /* printf */
#include <string.h>     /* strcat */
#include <stdlib.h>     /* strtol */

/**************************************************** extern promenljive **********************************************************/
extern volatile uint16_t mereni_napon;
extern volatile uint16_t merena_struja;
/**********************************************************************************************************************************/


 typedef struct Time_date_t
 {
	 uint8_t s;			//sekund
	 uint8_t min;		//minut
	 uint8_t hr;		//sat
	 uint8_t dy;		//dan u nedelji
	 uint8_t dt;		//dan u mesecu
	 uint8_t mt;		//mesec
	 uint8_t yr;		//godina
	 uint8_t hr_format;	//=_12_hour_format
	 uint8_t am_pm;     // =am	 
 }Time_date;



uint8_t period_paljenja(Time_date *On_time, Time_date *Off_time, Time_date *CurrentTime);  //typedef struct mora biti pre prototipa da bi je video
const char *byte_to_binary(int x)
{
	static char b[9];
	b[0] = '\0';

	int z;
	for (z = 128; z > 0; z >>= 1)
	{
		strcat(b, ((x & z) == z) ? "1" : "0");
	}

	return b;
}

int main(void)
{

	Time_date vreme_datum;
	Time_date Vreme_paljenja;
	Time_date Vreme_gasenja;
	
	Vreme_paljenja.hr = 23;
	Vreme_paljenja.min = 58;
	Vreme_gasenja.hr = 23;
	Vreme_gasenja.min = 59;
	
	char bafer[20];
	uint8_t ukljuceno = 0;  //0=OFF 1=ON
	uint8_t tasteri = 0xFF;
	
/******************************** Inicijalizacija perifirija ***************************************************/

	tajmer0_init();			////PD2-3 output
	i2c_init();				//NAPOMENA: ISKLJUCENI internal-pullup - ovi na SDA i SCL, unutar ove f-je
	lcd1602_init();
	ADC_init();				//NAPOMENA:	PINB7 output
	uart_init();			//baud 9600
	DS3231_init();			//RTC init
	pc_init();				//pin change interrupt init. NAPOMENA: PINC3 input
	tasteri_init();			//NAPOMENA: PD4-7 i PB0-1 INPUT, INT_PULLUP=ON
	
	DDRB |= 1<<PINB5;		//pinB 5 - DIG13 = OUTPUT LED DIODA
	DDRB |= 1<<PINB4;
	
	sei();	//global interrupt enable
	
/**************************************************************************************************************/
	
	//getTime(&vreme_datum.hr, &vreme_datum.min, &vreme_datum.s, &vreme_datum.am_pm, _24_hour_format);
	//getDate(&vreme_datum.dy, &vreme_datum.dt, &vreme_datum.mt, &vreme_datum.yr);
	
	setTime(23, 57, 55, am, _24_hour_format);
	
	
    while (1) 
    {
		
		tasteri = ocitaj_tastere();	//vrv bolje prebaciti negde u tajmer da se stalno desava
		//tasteri = PIND;
		sprintf(bafer, "%s", byte_to_binary(tasteri));
		lcd1602_goto_xy(0,1);
		lcd1602_send_string(bafer);
		
		if(flag_pc_int)
		{
			flag_pc_int = 0; //resetujem flag koji je u ISR
			
			
			getTime(&vreme_datum.hr, &vreme_datum.min, &vreme_datum.s, &vreme_datum.am_pm, _24_hour_format);
			sprintf(bafer, "%02d:%02d:%02d", vreme_datum.hr, vreme_datum.min, vreme_datum.s);
			//send_str(bafer);
			//send_str("\n"); //novi red
			
			lcd1602_clear();
			lcd1602_send_string(bafer);
			
			
			
			ukljuceno = period_paljenja(&Vreme_paljenja, &Vreme_gasenja, &vreme_datum);
			
			if (ukljuceno)
				PORTB |= 1<<PINB5;   //high
			else
				PORTB &= ~(1<<PINB5);	//low
			
		}
    }
}


uint8_t period_paljenja(Time_date *On_time, Time_date *Off_time, Time_date *CurrentTime)
{
	/* pali bojler u vremenskom intervalu koji se prosledi ovoj funkciji, odnosno gasi ga van intervala */
	/* Dovoljno je uzeti u obzir samo sate i minute, sekunde nisu od znacaja */
	/* Da bih olaksao sebi, sate i minute cu spajati u jednu promenljivu, npr. 22:15 se biti 2215 */
	/* Prelazak preko 00:00 je granicni slucaj i desava se jedino kada je vreme ON VECE od vreme OFF */
	/* Izlazni signal (Grejac upaljen) treba da bude uvek HIGH dok period traje */
	
	uint8_t stanje = 0;
	
	static uint16_t vreme_on;
	static uint16_t vreme_off;
	static uint16_t vreme_trenutno;
	
	char buff[50];
	
	/* prebacim sate i minute u jednu promenljivu */
	vreme_on = (On_time->hr)*100 + On_time->min;
	vreme_off = (Off_time->hr)*100 + Off_time->min;
	vreme_trenutno = (CurrentTime->hr)*100 + CurrentTime->min;
	
	
	sprintf(buff, "ON:%d OFF:%d Tr:%d",vreme_on, vreme_off, vreme_trenutno);
	send_str(buff);
	send_str("\n"); //novi red
	
	
	if (vreme_on >= vreme_off)  // >= ?? razmotri dodatno SVE granicne slucajeve; edit:provereno, sve u fuli radi
	{
		//npr. ON=23:20 OFF=05:30
		//postoji prelaz preko 00:00 !UVEK! kada je ON>OFF
		//izlazni signal treba biti HIGH sve vreme (ako je ispunjen uslov da je Period ukljucenosti u toku,
		//gledano naspram trenutnog vremena
		
		if ( (vreme_trenutno >= vreme_on)	&&	  (vreme_trenutno <= 2359) )		    //prvi grancni slucaj za uslov ukljucenja
		{
			//ukljuci
			stanje = 1;
		}
		else if (  (vreme_trenutno >= 0)	&&	 (vreme_trenutno < vreme_off) )		//drugi granicni slucaj
		{
			//ukljuci
			stanje = 1;
		}
		
	} 
	else  if(vreme_on < vreme_off)
	{
		//NPR ON=05:30 OFF=13:50
		//!NIKADA! ne postoji prelaz preko 00:00
		
		if ( (vreme_trenutno >= vreme_on)	&&	  (vreme_trenutno < vreme_off) )
		{
			//ukljuci
			stanje = 1;
		}
	}
	else
	{
		//iskljuceno
		stanje = 0;
	}
	
	return stanje;
	
}
