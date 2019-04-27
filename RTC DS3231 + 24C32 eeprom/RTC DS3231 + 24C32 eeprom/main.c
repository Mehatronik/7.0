/*
 * RTC DS3231 + 24C32 eeprom.c
 *
 * Created: 25.2.2019. 21.49.43
 * Author : Kovacevic
 */ 

#define  F_CPU 16000000UL

#include <avr/io.h>
#include "i2cmaster.h"
#include <avr/interrupt.h>
#include "uart.h"
#include <util/delay.h>


#define RTC_I2C_address (0x68<<1)	//siftujem da bi bilo 8-bit, jer to zahteva f-ja za i2c. Prvi bit je R/W (1/0)


int main(void)
{
	
	i2c_init();                       // init I2C interface
	uart_init();
	sei();
	
	uint8_t ret = 0;
	uint8_t cifra_donja=0, cifra_gornja=0;	//npr 59s, donja cifra je 9, gornja cifra je 5
	uint8_t sek = 0;
	uint8_t buffer[16];
	
	
	
	
    
    while (1) 
    {
		
		
		i2c_start_wait(RTC_I2C_address+I2C_WRITE);     // set device address and write mode
		i2c_write(0x00);                        //  address = 0
		i2c_start_wait(RTC_I2C_address+I2C_READ);       // set device address and read mode
		
		ret = i2c_readNak(); //read one byte, read is followed by a stop condition
		
		/* prva cifra za sekunde je na prva 4 bita na 0x00 adresi */
		cifra_donja = ret & 0x0F; //maskiram da izvucem samo prva 4 bita, ostatak nuliram
		cifra_gornja = ret >> 4; //maskiram gornja 4 bita
		sek = (cifra_gornja*10) + cifra_donja;
		
				//i2c_stop(); //probaj bez zbog i2c readNak
		sprintf(buffer, "%2d", sek);
		send_str(buffer);
		
		send_str("\n");
		_delay_ms(500);
		

    }
}

