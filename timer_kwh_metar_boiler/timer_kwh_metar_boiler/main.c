/*
 * timer_kwh_metar_boiler.c
 *
 * Created: 26.4.2019. 19.26.28
 * Author : Kovacevic
 */ 

#include "comm.h"
#include "i2cmaster.h"
#include "uart.h"
#include "lcd1602.h"


int main(void)
{
    
	char bafer[15];
	int a=10, b=5, c=255;

	uart_init();
	i2c_init();				//NAPOMENA: uljuceni internal-pullup - ovi na SDA i SCL, unutar ove f-je
	lcd1602_init();
	
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
		
		send_str("  ");
		sprintf(bafer, "%d", a);
		lcd1602_send_string(bafer);
		send_str(bafer);
		
		send_str("  ");
		sprintf(bafer, "%d", b);
		lcd1602_send_string(bafer);
		send_str(bafer);
		
		send_str("  ");
		sprintf(bafer, "%d", c);
		lcd1602_send_string(bafer);
		send_str(bafer);

		
		
		send_str("\n");  //novi red

		_delay_ms(100);
    }
}

