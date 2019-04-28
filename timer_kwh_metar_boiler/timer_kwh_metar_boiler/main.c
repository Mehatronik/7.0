/*
 * timer_kwh_metar_boiler.c
 *
 * Created: 26.4.2019. 19.26.28
 * Author : Kovacevic
 */ 

#include "comm.h"
//#include "tajmer.h"
//#include "i2cmaster.h"
//#include "lcd1602.h"
//#include "ad_konverzija.h"
//#include "uart.h"

/* extern-om se u ovom fajlu deklarisu funkcije i promenljive koje se nalaze u drugim .c fajlovima */

/* extern funkcije */
extern void tajmer0_init();
extern void i2c_init();
extern void lcd1602_init();
extern void ADC_init();
extern void uart_init();
extern void send_str(char *str);
extern void lcd1602_clear(void);
extern void lcd1602_goto_xy(char col, char row);
extern void lcd1602_send_string(const char *str);

/* extern promenljive */
extern volatile unsigned char flag_tajmer0_prekid;
extern volatile unsigned char flag_prekid_10ms;

int main(void)
{
    
	char bafer[15];
	int a=255, b=1500, c=-1;

	tajmer0_init();			//NAPOMENA:	PINB5 output
	i2c_init();				//NAPOMENA: uljuceni internal-pullup - ovi na SDA i SCL, unutar ove f-je
	lcd1602_init();
	ADC_init();				//NAPOMENA:	PINB7 output
	uart_init();			//baud 9600
	
	
	sei();	//global interrupt enable
	
	
	lcd1602_clear();
	

	
	//lcd1602_clear(); //ujedno i vrati kursor na 0,0
	//lcd1602_send_string("Counter:");
	//lcd1602_goto_xy(0,1);
	
	
	
    while (1) 
    {
		/* preko uarta se salju stringovi, pa da bi se int poslao mora se prvo konvertovati preko itoa() f-je */
		/* moguce je koristiti i ostale slicne f-je: utoa(), ltoa()..... */
		/* umesto itoa() bolje je koristiti sprintf(buffer, "%3d", int); */
		
		
		lcd1602_clear();
		
		send_str("  ");
		sprintf(bafer, "%d", a);
		lcd1602_send_string(bafer);
		send_str(bafer);
		
		send_str("  ");
		sprintf(bafer, "%d", b);
		lcd1602_goto_xy(10,0);
		lcd1602_send_string(bafer);
		send_str(bafer);
		
		send_str("  ");
		sprintf(bafer, "%d", c);
		lcd1602_goto_xy(0,1);
		lcd1602_send_string(bafer);
		send_str(bafer);
		
		
		
		send_str("\n");  //novi red

		_delay_ms(100);
    }
}

