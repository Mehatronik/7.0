/*
 * i2c eeprom.c
 *
 * Created: 17.9.2018. 23.52.08
 * Author : Ko''
 */ 

#define  F_CPU 16000000UL

#include <avr/io.h>
#include "i2cmaster.h"

#include <avr/interrupt.h>
#include "uart.h"
#include <util/delay.h>


#define atmel24C08A_adr  0b1010000      // device address of EEPROM 24C08a, see datasheet



int main(void)
{
	unsigned char ret;
	char bafer[15];
	int i=0;
	
    i2c_init();                                // init I2C interface
	uart_init();
	sei();
    
	//PB5 = dig13 = led - izlaz
	DDRB |= 1<<PORTB5;
	
	
	
	/* write 93 to eeprom address 0x05 (Byte Write) */
	 i2c_start_wait( (atmel24C08A_adr << 1) + I2C_WRITE);       // set device address and write mode, last bit is Read-1, Write-0; shift zato sto je addr 7-bit-na
			
	
	/* issuing start condition ok, device accessible */
	//i2c_write(0x01);                       // write address = 0
	//i2c_write(111);                       // ret=0 -> Ok, ret=1 -> no ACK 
	i2c_stop();                            // set stop conditon = release bus
	//PORTB |= 1<<PORTB5; //set led
               
 
	
	
	/* podesim pocetnu adresu za citanje */
	i2c_start_wait( (atmel24C08A_adr << 1) + I2C_WRITE);
	i2c_write(0x00);										//pocetna adresa = 0
	
	
	
	
	for(i=0; i<1024; i++)
	{
		
		
		i2c_start_wait( (atmel24C08A_adr << 1) + I2C_READ);    //citanje
		ret = i2c_readNak();                    // read one byte
		//i2c_stop();
		
		
		/* slanje preko uarta */
		itoa( ret , bafer, 10);
		send_str(bafer);

		send_str("\n");  //novi red

		//_delay_ms(20);

	}
	
	
	
	
	
	
	
	
	
    
    while (1) 
    {
		
		
    }
}

