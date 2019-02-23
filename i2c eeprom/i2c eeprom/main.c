/*
 * i2c eeprom.c
 *
 * Created: 17.9.2018. 23.52.08
 * Author : Kovacevic
 * 
 * EEPROM: ATMEL 24C08A - 1024x8 bit tj. 1024 bajta; Komunikacija putem I2C protokola (tj. TWI)
 * Za detalje oko citanja/pisanja pogledati datasheet
 * Zbog toga sto ima 1024 memorijske lokacije, zahteva 10-bitno adresiranje, tj. slanje adrese 
 * 2 MSB bita za adresiranje se nalaze sakriveni u adresi eeprom uredjaja, dok se ostalih 8 'donjih' bita salje kao podatak
 * Tok komunikacije je sledeci: start - adresa uredjaja i read/write zahtev - adresa citanja/pisanja - podatak(ako se upisuje) - stop
 * Ovo je 'standardno' citanje/pisanje. Postoji i page write opcija gde se jednom posalje start, adresa uredjaja, adresa pisanja, a zatim se podaci salju
 * uzastopno 16 puta, gde se adresa (samo donjih 8 bita) automatski uvecava. Kada istekne 16 komunikacija se terminise od strane eeprom uredjaja
 * Kod citanja je potrebno prvo zapoceti komunikaciju kao kod pisanja, gde se podesi samo pocetna adresa citanja, a zatim ukljuciti Read rezim.
 * Postoji opcija sekvencijalnog citanja gde se adresa citanja potpuno sama uvecava (svih 10 bita), sto sam i koristio dole u kodu.
 */ 

#define  F_CPU 16000000UL

#include <avr/io.h>
#include "i2cmaster.h"
#include <avr/interrupt.h>
#include "uart.h"
#include <util/delay.h>


//Cista adresa eeproma je 10100xxy. xx su bitovi za adresiranje, dok je y read/write bit komanda
#define atmel24C08A_adr_x0 0b10100000      // device address of EEPROM 24C08a, see datasheet; dodata 0 na kraju radi lakseg citanja i pisanja; (0x28 << 1) za Samsung toner eeprom
#define atmel24C08A_adr_x1 0b10100010	  //fizicki se koristi samo jedan pin (A1, A2, A3), dok su druga dva nepovezana eksterno, dok se interno
#define atmel24C08A_adr_x2 0b10100100	  //koriste za adresiranje (10-bit), jer eeprom ima 1024 bajta
#define atmel24C08A_adr_x3 0b10100110


uint8_t adr = atmel24C08A_adr_x0; //pomocna prom da bi mogao menjati gornja 2 bita (od 10-bitne adrese)
unsigned char ret;
char bafer[15];

void eeprom_reset();
void eeprom_read_all_print();  
void eeprom_write_byte(uint16_t I2C_addr, uint16_t addr, uint8_t data);

int main(void)
{
	
	
    i2c_init();                                // init I2C interface
	uart_init();
	sei();
    
	//PB5 = dig13 = led - izlaz
	DDRB |= 1<<PORTB5;
	
	/*
	eeprom_write_byte(atmel24C08A_adr_x0, 511, 200);
	eeprom_write_byte(atmel24C08A_adr_x0, 512, 200);
	
	eeprom_write_byte(atmel24C08A_adr_x0, 767, 111);
	eeprom_write_byte(atmel24C08A_adr_x0, 768, 112);
	
	eeprom_write_byte(atmel24C08A_adr_x0, 1023, 188);
	*/
	
	eeprom_read_all_print();  //procitaj ceo eeprom i odstampaj preko uarta
    
    while (1) 
    {
		
		
    }
}




void eeprom_reset()
{
	int i=0;
	int j=0;
	
	
	
	//prolece kroz sva 4 gornja bita i svaku adresu. Sve inicijalizuje na zeljenu vrednost - 255 = 0xFF
	for(i=0; i<4; i++)
	{
		for(j=0; j<256; j++)
		{

			i2c_start_wait( adr + I2C_WRITE);	// set device address and write mode, last bit is Read-1, Write-0;
			
			i2c_write(j);                       // prvo pisanje je zadavanje adrese (ima ih 256 u jednom delu)
			i2c_write(255);						// sledece pisanje je upis vrednosti u tu adresu
			
			i2c_stop();							// set stop conditon = release bus
		}
		adr = (adr >> 1) + 1;  //prvo siftujem da bi uvecao gornju adresu, jer je LSB R/W komanda, a 1 i 2 bit su za adresiranje
		adr = adr << 1;		   //potom vratim da bi bilo 8-bit. LSB se puni nulom
		
	}
}

void eeprom_read_all_print()
{
	
	int i=0;
	int j=0;
	
	/* podesim pocetnu adresu za citanje */
	i2c_start_wait( atmel24C08A_adr_x0 + I2C_WRITE);
	i2c_write(0x00);									//pocetna adresa = 0
	i2c_start_wait( atmel24C08A_adr_x0 + I2C_READ);    //citanje
	
	/* ispis na terrminal citavog sadrzaja. 64*16 = 1024. Tako ispisujem jer je takva interna organizacija memorije u eepromu */
	for(i=0; i<64; i++)
	{
		for(j=0; j<16; j++)
		{
			ret = i2c_readAck();                    // read one byte, request more data from device
			
			
			/* slanje preko uarta */
			//itoa( ret , bafer, 10);
			sprintf(bafer, "%3d", ret);   //presao sam igricu. moze i sprintf umesto itoa. %3d znaci da sam formatirao ispis na minimum 3 mesta, za pregledniji ispis.
			send_str(bafer);

			send_str("  ");  //razmak
		}
		
		send_str("\n");  //novi red
	}
	
	i2c_stop();
	
}

void eeprom_write_byte(uint16_t I2C_addr, uint16_t addr, uint8_t data)
{
	//za 1024 bajta EEPROM vazi ofa funkcija
	//podeljen je u cetiri dela od 256
	
	//addr - adresa za upisivanje u eepromu		(0-1023)
	//data - podatak za upisivanje na datu adresu	(0-255)
	
	if (addr < 256)	//I deo EEPROMA
	{
		i2c_start_wait( I2C_addr + I2C_WRITE);	// set device address and write mode, last bit is Read-1, Write-0;
		
		i2c_write(addr);                       // prvo pisanje je zadavanje adrese (ima ih 256 u jednom delu)
		i2c_write(data);						// sledece pisanje je upis vrednosti u tu adresu
		
		i2c_stop();							// set stop conditon = release bus
	}
	else if (addr >= 256 && addr < 512)	//II deo EEPROMA
	{
		I2C_addr = (I2C_addr >> 1) + 1;  //prvo siftujem da bi uvecao gornju adresu, jer je LSB R/W komanda, a 1 i 2 bit su za adresiranje
		I2C_addr = I2C_addr << 1;		   //potom vratim da bi bilo 8-bit. LSB se puni nulom
		
		i2c_start_wait( I2C_addr + I2C_WRITE);	// set device address and write mode, last bit is Read-1, Write-0;
		
		i2c_write(addr-256);                       // prvo pisanje je zadavanje adrese (ima ih 256 u jednom delu)
		i2c_write(data);						// sledece pisanje je upis vrednosti u tu adresu
		
		i2c_stop();
	}
	else if (addr >= 512 && addr < 768) //III deo EEPROMA
	{
		I2C_addr = (I2C_addr >> 1) + 2;  //prvo siftujem da bi uvecao gornju adresu, jer je LSB R/W komanda, a 1 i 2 bit su za adresiranje
		I2C_addr = I2C_addr << 1;		   //potom vratim da bi bilo 8-bit. LSB se puni nulom
		
		i2c_start_wait( I2C_addr + I2C_WRITE);	// set device address and write mode, last bit is Read-1, Write-0;
		
		i2c_write(addr-512);                       // prvo pisanje je zadavanje adrese (ima ih 256 u jednom delu)
		i2c_write(data);						// sledece pisanje je upis vrednosti u tu adresu
		
		i2c_stop();
	}
	else if (addr >= 768 && addr < 1024) //IV deo EEPROMA
	{
		I2C_addr = (I2C_addr >> 1) + 3;  //prvo siftujem da bi uvecao gornju adresu, jer je LSB R/W komanda, a 1 i 2 bit su za adresiranje
		I2C_addr = I2C_addr << 1;		   //potom vratim da bi bilo 8-bit. LSB se puni nulom
		
		i2c_start_wait( I2C_addr + I2C_WRITE);	// set device address and write mode, last bit is Read-1, Write-0;
		
		i2c_write(addr-768);                       // prvo pisanje je zadavanje adrese (ima ih 256 u jednom delu)
		i2c_write(data);						// sledece pisanje je upis vrednosti u tu adresu
		
		i2c_stop();
	}
	
}