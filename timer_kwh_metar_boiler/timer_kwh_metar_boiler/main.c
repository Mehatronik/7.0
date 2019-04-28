/*
 * timer_kwh_metar_boiler.c
 *
 * Created: 26.4.2019. 19.26.28
 * Author : Kovacevic
 */ 

#include "comm.h"
#include "rtc_ds3231.h"
#include "pin_change_interrupt.h"
//#include "tajmer.h"
//#include "i2cmaster.h"
//#include "lcd1602.h"
//#include "ad_konverzija.h"
//#include "uart.h"

/* extern-om se u ovom .c fajlu deklarisu funkcije i promenljive koje se nalaze u drugim .c fajlovima */

/******************************************* extern funkcije ********************************************************************/
extern void tajmer0_init();
extern void i2c_init();
extern void lcd1602_init();
extern void ADC_init();
extern void uart_init();
extern void send_str(char *str);
extern void lcd1602_clear(void);
extern void lcd1602_goto_xy(char col, char row);
extern void lcd1602_send_string(const char *str);
/**********************************************************************************************************************************/

/**************************************************** extern promenljive **********************************************************/
extern volatile unsigned char flag_tajmer0_prekid;
extern volatile unsigned char flag_prekid_10ms;
extern volatile uint16_t mereni_napon;
extern volatile uint16_t merena_struja;
/**********************************************************************************************************************************/

int main(void)
{
    typedef struct Time_date_t
	{
		uint8_t s;			//sekund
		uint8_t min;		//minut
		uint8_t hr;			//sat
		uint8_t dy;			//dan u nedelji
		uint8_t dt;			//dan u mesecu
		uint8_t mt;			//mesec
		uint8_t yr;			//godina
		uint8_t hr_format;	//=_12_hour_format
		uint8_t am_pm;      // =am
		
	}Time_date;
	
	Time_date vreme_datum;
	char bafer[20];
	
/******************************** Inicijalizacija perifirija ***************************************************/

	tajmer0_init();
	i2c_init();				//NAPOMENA: ISKLJUCENI internal-pullup - ovi na SDA i SCL, unutar ove f-je
	lcd1602_init();
	ADC_init();				//NAPOMENA:	PINB7 output
	uart_init();			//baud 9600
	DS3231_init();			//RTC init
	pc_init();				//pin change interrupt init. NAPOMENA: PINC3 input, PINB5 output
	
	
	sei();	//global interrupt enable
	
/**************************************************************************************************************/
	
	//getTime(&vreme_datum.hr, &vreme_datum.min, &vreme_datum.s, &vreme_datum.am_pm, _24_hour_format);
	//getDate(&vreme_datum.dy, &vreme_datum.dt, &vreme_datum.mt, &vreme_datum.yr);
	
	
	
    while (1) 
    {
		
		
		if(flag_pc_int)
		{
			flag_pc_int = 0; //resetujem flag koji je u ISR
			
			
			getTime(&vreme_datum.hr, &vreme_datum.min, &vreme_datum.s, &vreme_datum.am_pm, _24_hour_format);
			sprintf(bafer, "%02d:%02d:%02d", vreme_datum.hr, vreme_datum.min, vreme_datum.s);
			send_str(bafer);
			send_str("\n"); //novi red
			
			lcd1602_clear();
			lcd1602_send_string(bafer);
			
		}
		
	
		
		//lcd1602_clear();
//
		//sprintf(bafer, "%d", mereni_napon);
		//
		////na LCD
		//lcd1602_goto_xy(0,0);
		//lcd1602_send_string("A0:");
		//lcd1602_send_string(bafer);
		//
		////na uart
		//send_str("A0:");
		//send_str(bafer);
		//
		//send_str("  ");	//razmak
		//
		//sprintf(bafer, "%d", merena_struja);
		//
		////na LCD
		//lcd1602_goto_xy(0,1);
		//lcd1602_send_string("A1:");
		//lcd1602_send_string(bafer);
		//
		////na uart
		//send_str("A1:");
		//send_str(bafer);
		//
		//send_str("\n"); //novi red
//
		//_delay_ms(100);
    }
	
}

