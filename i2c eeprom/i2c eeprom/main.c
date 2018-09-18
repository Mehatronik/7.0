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

//nazovimo ovo page-ovima. Cista adresa eeproma je 10100xxy. xx su bitovi za adresiranje, dok je y read/write bit komanda
#define atmel24C08A_adr_page0 0b10100000      // device address of EEPROM 24C08a, see datasheet; dodata 0 na kraju radi lakseg citanja i pisanja
#define atmel24C08A_adr_page1 0b10100010	  //fizicki se koristi samo jedan pin (A1, A2, A3), dok su druga dva nepovezana eksterno, dok se interno
#define atmel24C08A_adr_page2 0b10100100	  //koriste za adresiranje (10-bit), jer eeprom ima 1024 bajta
#define atmel24C08A_adr_page3 0b10100110



int main(void)
{
	unsigned char ret;
	char bafer[15];
	int i=0;
	int j=0;
	uint8_t adr = atmel24C08A_adr_page0; //pomocna prom da bi mogao menjati page-ove
	
    i2c_init();                                // init I2C interface
	uart_init();
	sei();
    
	//PB5 = dig13 = led - izlaz
	DDRB |= 1<<PORTB5;
	
	
	
	
	 //i2c_start_wait( atmel24C08A_adr_page0 + I2C_WRITE);       
	 //i2c_write(0x00);

	
	//prolece kroz sva 4 page-a i svaku adresu. Sve inicijalizuje na zeljenu vrednost - 255 = 0xFF
	for(i=0; i<4; i++)
	{	
		for(j=0; j<256; j++)
		{

			i2c_start_wait( adr + I2C_WRITE);	// set device address and write mode, last bit is Read-1, Write-0;
			
			i2c_write(j);                       // prvo pisanje je zadavanje adrese (ima ih 256 u jednom page-u)
			i2c_write(255);						// sledece pisanje je upis vrednosti u tu adresu
			
			i2c_stop();							// set stop conditon = release bus
		}	
		adr = (adr >> 1) + 1;  //prvo siftujem da bi uvecao page, jer je LSB R/W komanda, a 1 i 2 bit su za adresiranje
		adr = adr << 1;			//potom vratim da bi bilo 8-bit. LSB se puni nulom
		
	}
	
	
 
	
	/* podesim pocetnu adresu za citanje */
	i2c_start_wait( atmel24C08A_adr_page0 + I2C_WRITE);
	i2c_write(0x00);									//pocetna adresa = 0
	
	
	/* ispis na terrminal citavog sadrzaja. 64*16 = 1024. Tako ispisujem jer je takva interna organizacija memorije u eepromu */
	for(i=0; i<64; i++)
	{
		for(j=0; j<16; j++)
		{
			i2c_start_wait( atmel24C08A_adr_page0 + I2C_READ);    //citanje
			ret = i2c_readNak();                    // read one byte
			
		
			/* slanje preko uarta */
			//itoa( ret , bafer, 10);
			sprintf(bafer, "%3d", ret);   //presao sam igricu. moze i sprintf umesto itoa. %3d znaci da sam formatirao ispis na minimum 3 mesta, za pregledniji ispis.
			send_str(bafer);

			send_str("  ");  //razmak
		}
		
		send_str("\n");  //novi red
	}
	
	i2c_stop();
	
	
	
	
	
	
	
    
    while (1) 
    {
		
		
    }
}

