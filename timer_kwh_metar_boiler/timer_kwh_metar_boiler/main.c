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
#include "state_machine.h"


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
	char *p = b;
	
	int z;
	for (z = 128; z > 0; z >>= 1)
	{
		//strcat(b, ((x & z) == z) ? "1" : "0");
		*p++ = (x & z) ? '1' : '0';
	}

	return b;
}

int main(void)
{

	Time_date vreme_datum;
	Time_date vreme_paljenja;
	Time_date vreme_gasenja;
	Time_date sanp_shot_vremena;	//za potrebe podesavanje vremena
	
	vreme_paljenja.hr = 23;
	vreme_paljenja.min = 30;
	vreme_gasenja.hr = 5;
	vreme_gasenja.min = 40;
	
	char bafer[20];
	uint8_t ukljuceno = 0;  //0=OFF 1=ON
	uint8_t tasteri = 0xFF;

	uint8_t STATE = DISPL1;
	int8_t kursor = 0;
	uint8_t flag_pod_vremena = 1;
	uint8_t provera = 0;
/******************************** Inicijalizacija perifirija ***************************************************/

	tajmer0_init();			////PD2-3 output
	i2c_init();				//NAPOMENA: ISKLJUCENI internal-pullup - ovi na SDA i SCL, unutar ove f-je
	lcd1602_init();
	ADC_init();				
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
	
	//setTime(23, 59, 55, am, _24_hour_format);
	
	
    while (1) 
    {
		/* polling tastera */
		tasteri = ocitaj_tastere();
		
		
		/* bez obzira na STATE provera vremena treba da ide na 1s odnosno provera
		   da li grejac treba biti ukljucen ili iskljucen. Donji deo koda (swithc-case) ne bi trebao da koci program */
		if (flag_pc_int_pomocni)		//ide na 1 sekund
		{
			flag_pc_int_pomocni = 0;
			/* da sam koristio isti flag kao u automatu stanja, a ovde ga resetujem, dole nikada
			   ne bi bio ispunjen uslov za flag_pc_int==1 */
			
			getTime(&vreme_datum.hr, &vreme_datum.min, &vreme_datum.s, &vreme_datum.am_pm, _24_hour_format);
			
			
			/* paljenje/gasenje releja > grjaca bojlera */
			ukljuceno = period_paljenja(&vreme_paljenja, &vreme_gasenja, &vreme_datum);
			
			if (ukljuceno)
				PORTB |= 1<<PINB5;   //high
			else
				PORTB &= ~(1<<PINB5);	//low
				
			sprintf(bafer, "%d", provera);
			send_str(bafer);
			send_str("\n");
		}
		
		
		
		/* automat stanja za menije na displeju */
		switch(STATE)
		{
			
			case DISPL1:
						/* ispis vremena svaki sekund dok je u ovom CASE-u */
						if(flag_pc_int)		//pc int usled signala koji dolazi sa SQW pin sa RTC modula; 1 sekund
						{
							flag_pc_int = 0; //resetujem flag koji je u ISR
							
							getTime(&vreme_datum.hr, &vreme_datum.min, &vreme_datum.s, &vreme_datum.am_pm, _24_hour_format);
							sprintf(bafer, "%02d:%02d:%02d", vreme_datum.hr, vreme_datum.min, vreme_datum.s);
							//send_str(bafer);
							//send_str("\n"); //novi red
							
							//lcd1602_clear();
							
							/*izbegao sam celear-ovanje ekrana koje izaziva teperenje dipleja, sa dodavanjem razmaka pre i posle zeljenog ispisa */
							lcd1602_goto_xy(0,0);
							lcd1602_send_string("    ");
							lcd1602_send_string(bafer);
							lcd1602_send_string("    ");
							
							lcd1602_goto_xy(0,1);
							lcd1602_send_string("  ");
							sprintf(bafer, "%02d:%02d->%02d:%02d", vreme_paljenja.hr, vreme_paljenja.min, vreme_gasenja.hr, vreme_gasenja.min);
							lcd1602_send_string(bafer);
							lcd1602_send_string("  ");
		
						}
						
						if ( ocitaj_jedan_taster(tasteri, TASTER_ENTER) )	//taster enter stisnut   !(tasteri & (1<<TASTER_ENTER))
							STATE = MENU1;
			break;
			
			case MENU1:
						//lcd1602_clear();
						
						lcd1602_goto_xy(0, kursor);
						lcd1602_send_string(">");
						lcd1602_goto_xy(0, !kursor);
						lcd1602_send_string(" ");
						
						lcd1602_goto_xy(1,0);
						lcd1602_send_string("PODESI SAT     ");
						lcd1602_goto_xy(1,1);
						lcd1602_send_string("PODESI PERIOD  ");
						
						if ( ocitaj_jedan_taster(tasteri, TASTER_GORE) ) //ocitaj_jedan_taster(tasteri, TASTER_DOLE)
						{
							kursor++;
							if(kursor > 1)
								kursor = 1;
						}
						else if ( ocitaj_jedan_taster(tasteri, TASTER_DOLE) )
						{
							kursor--;
							if(kursor < 0)
								kursor = 0;
						}
						else if(kursor == 0 && ocitaj_jedan_taster(tasteri, TASTER_ENTER))
						{
							kursor = 0;			//resetujem kursor jer ostane memorisan
							STATE = POD_SAT;	//meni za podesavanje sata
						}
						else if(kursor == 1 && ocitaj_jedan_taster(tasteri, TASTER_ENTER))
						{
							kursor = 0;			//resetujem kursor jer ostane memorisan
							STATE = POD_ON_OFF;	//sub_meni za podesavanje on ili off vremena
						}
						else if ( ocitaj_jedan_taster(tasteri, TASTER_NAZAD) )				//taster nazad stisnut
						{
							kursor = 0;			//resetujem kursor jer ostane memorisan
							STATE = DISPL1;		//vraca se na prethodni meni, tj. glavni meni
						}		
			break;
			
			case POD_SAT:
						
						/* da ocita trenutno vreme samo prvi puta kada upadne u ovaj case	*/
						if (flag_pod_vremena)
						{
							flag_pod_vremena = 0;
							sanp_shot_vremena = vreme_datum;
							sprintf(bafer, "%02d:%02d:%02d", sanp_shot_vremena.hr, sanp_shot_vremena.min, sanp_shot_vremena.s);
							
							kursor = 5; //na 5 je hh, na 8 je mm a na 11 je ss
							
							lcd1602_goto_xy(0,0);
							lcd1602_send_string("PODESAVANJE SATA");
							
							lcd1602_goto_xy(0,1);
							lcd1602_send_string("    ");
							lcd1602_send_string(bafer);
							lcd1602_send_string("    ");
							
							lcd1602_goto_xy(kursor,1);
							lcd1602_cursor_blink(1);
						}
						
						if ( ocitaj_jedan_taster(tasteri, TASTER_DESNO) )		//kursor desno
						{
							kursor += 3;
							if(kursor > 11)
								kursor = 11;
							lcd1602_goto_xy(kursor,1);
						}
						else if (ocitaj_jedan_taster(tasteri, TASTER_LEVO))		//kursor levo
						{
							kursor -= 3;
							if(kursor < 5)
								kursor = 5;
							lcd1602_goto_xy(kursor,1);
						}
						else if (ocitaj_jedan_taster(tasteri, TASTER_GORE) && kursor==5)	//podesava SATE ++
						{
							sanp_shot_vremena.hr++;
							if(sanp_shot_vremena.hr >= 24 && sanp_shot_vremena.hr < 30)		//<30 posto je hr = unsigned int
								sanp_shot_vremena.hr = 0;
							
							sprintf(bafer, "%02d", sanp_shot_vremena.hr);
							lcd1602_goto_xy(kursor-1,1);
							lcd1602_send_string(bafer);
							lcd1602_goto_xy(kursor,1);
							setTime(sanp_shot_vremena.hr, vreme_datum.min, vreme_datum.s, am, _24_hour_format);
						}
						else if (ocitaj_jedan_taster(tasteri, TASTER_DOLE) && kursor==5)	//podesava SATE --
						{
							sanp_shot_vremena.hr--;
							if(sanp_shot_vremena.hr >= 250 && sanp_shot_vremena.hr <= 255)	//posto je hr = unsigned int 8 bit i overflow se desi
								sanp_shot_vremena.hr = 23;
							
							sprintf(bafer, "%02d", sanp_shot_vremena.hr);
							lcd1602_goto_xy(kursor-1,1);
							lcd1602_send_string(bafer);
							lcd1602_goto_xy(kursor,1);
							setTime(sanp_shot_vremena.hr, vreme_datum.min, vreme_datum.s, am, _24_hour_format);
						}
						else if ( ocitaj_jedan_taster(tasteri, TASTER_GORE) && kursor==8 )	//podesava MINUTE ++
						{
							provera = 1;
							//nece da uskoci u ovaj deo koda jebemu sunce suncevo mu jebem, ne znam zasto, nema logike
							//pokusao sam sa razlicitim nivoima optimizacije, i sa iskljucenom optimizacijom i isto
							//kada sam gore za menjanje hh stavio kursor==8 onda je upao u njega
							//kursor promenljiva je ok, provereno
							//da li kod za menjanje sata nesto cudno uradi, ali ne nema smisla zato sto
							//ne djiram sate a kad stavim kursor na minute opet ne radi nebo mu jebem, probacu sutra
						}
						else if ( ocitaj_jedan_taster(tasteri, TASTER_NAZAD) )			//IZLAZ iz menija
						{
							flag_pod_vremena = 1; //dozvolim ponovno citanje tr vremena kada se udje u ovaj case
							kursor = 0;			  //reset kursora
							lcd1602_cursor_blink(0);	  //isklucim blinking cursor
							STATE = MENU1;	//vraca se u prethodni meni
						}
						
			break;
			
			default: {}
			
		}
		
		
		
		
		
		//polling
		//tasteri = ocitaj_tastere();	//vrv bolje prebaciti negde u tajmer da se stalno desava
		//tasteri = PIND;
		
		
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
	
	
	//sprintf(buff, "ON:%d OFF:%d Tr:%d",vreme_on, vreme_off, vreme_trenutno);
	//send_str(buff);
	//send_str("\n"); //novi red
	
	
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
