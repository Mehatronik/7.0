/*
 * LCD_1602_I2C.c
 *
 * Created: 19.9.2018. 20.56.35
 * Author : Kovacevic
 *
 * Prosto neverovatno. Sa Arduino i2c bibliotekom je radilo normalno. Medjutim.
 * Ispostavilo se da se u toj biblioteci koristi softverski i2c. Hardverski protokol zahteva
 * pull-up otpornike na SDA i SCL, jer hardverski modul moze samo da 'sink-uje' struju.
 * Ovaj i2c modul nema stavljene otpornike, pa sam stavio 1k prema 5V i sve je profunkcionisalo sa ovim kodom.
 *
 */ 

#define F_CPU 16000000UL

#include <avr/io.h>
#include "i2c.h"
#include "lcd1602.h"
#include <util/delay.h>



int main(void)
{
    int x = 0;
	char buffer[30] = {0};
	
	i2c_init();
	lcd1602_init();
	lcd1602_clear();
	
	
	//lcd1602_send_char('a');
	//lcd1602_send_string("Hello from");
	//lcd1602_goto_xy(0,1); //kolona, red
	//lcd1602_send_string("Atmel Studio 7");
	lcd1602_send_string("Counter:");
	
	
	
    while (1) 
    {
		lcd1602_goto_xy(0,1);
		
		sprintf(buffer, "%d", x);
		lcd1602_send_string(buffer);
		x++;
		if(x>=32000)
		{
			x = 0;
			lcd1602_clear(); //ujedno i vrati kursor na 0,0
			lcd1602_send_string("Counter:");
			lcd1602_goto_xy(0,1);
		}
			
		//_delay_ms(5);
    }
}

