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
#include "eeprom.h"


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

Time_date vreme_datum;
Time_date vreme_paljenja;
Time_date vreme_gasenja;
Time_date jednokratno_paljenje;
Time_date jednokratno_gasenje;

/* pomocne strukture za djiranje kroz menije, tacnije temp promenljive */
Time_date snap_vreme_paljenja;
Time_date snap_vreme_gasenja;
Time_date snap_shot_vremena;	//za potrebe podesavanje vremena

char bafer[20];
uint8_t ukljuceno = 0;  //0=OFF 1=ON
uint8_t tasteri = 0xFF;
uint8_t STATE = DISPL1;	//pocetno stanje
uint8_t displ_flag_shot = 1;	//inicijalno 1, pomocni flag kod uletanja u menije, da se samo prvi put izvrsava ispis, da ne djira bezveze
int8_t kursor = 0;																				//TODO: ubaci drugu promenljivu za horizontalni kursor, da bi mogao da pamtis vertikalni, kada se vracas u prethodni meni da se ne resetuje
//uint8_t flag_pod_vremena = 1;	//TODO: mislim da se ovde 3 "flag-shota" mogu svesti pod jednu promenljivu. EDIT: done.
//uint8_t flag_pod_ONOFF = 1;
uint8_t jednok_on_off = 0;	//za on/off jednokratnog perioda
uint8_t jednok_se_desio = 0;


//za dodavanje novih menije jednostavno se doda novi tekst, i definise redni broj kursora
const char menu1_txt[][16]  = { "JEDNOKRATNO",
								 "PODESI PERIOD  ",
								 "PODESI SAT     ",
								 "TBC VAWWh ON-OF"	};		

const uint8_t brojRedova = sizeof(menu1_txt) / sizeof(menu1_txt[1]) - 1;	//sa logikom gde je 0 prvi broj, zato -1

/* mapiram vrednost kursora za menije, TREBA da se POKLAPA sa redosledom u gornjem nizu stringova !!!!!! */
#define KURSOR_PODSAT    2
#define KURSOR_PODONOF   1
#define KURSOR_JEDNOKRAT 0
#define KURSOR_MENU1_MAX	brojRedova		//max broj kursora za MENI 1. Za 3 reda teksa je ovo =2	
	

/************************** prototipovi funkcija ***************************************/
uint8_t period_paljenja(Time_date *On_time, Time_date *Off_time, Time_date *CurrentTime);  //typedef struct mora biti pre prototipa da bi je video
void fsm_lcd_menu();

/************************* extern funkcije *********************************************/
extern void sati_ispis(uint8_t *sat, char *buff, int8_t *cursor, uint8_t inc_dec);
extern void minuti_ispis(uint8_t *minut, char *buff, int8_t *cursor, uint8_t inc_dec);
extern void sekundi_ispis(uint8_t *sekund, char *buff, int8_t *cursor, uint8_t inc_dec);

const char *byte_to_binary(int x)
{
	static char b[9];
	b[0] = '\0';
	char *p = b;
	
	int z;
	for (z = 128; z > 0; z >>= 1)
		*p++ = (x & z) ? '1' : '0';

	return b;
}

int main(void)
{
	
	
/******************************** Inicijalizacija perifirija ***************************************************/

	tajmer0_init();			
	i2c_init();				//NAPOMENA: ISKLJUCENI internal-pullup na SDA i SCL, PC4-PC5 
	lcd1602_init();
	ADC_init();				
	uart_init(500000);		//vidi f-ju za opcije bauda; 500k
	DS3231_init();			//RTC init
	pc_init();				//pin change interrupt init. NAPOMENA: PINC3 input
	tasteri_init();			//NAPOMENA: PD2-7 INPUT, INT_PULLUP=ON
	
	DDRB |= 1<<PINB5;		//pinB 5 - DIG13 = OUTPUT LED DIODA
	DDRB |= 1<<PINB4;		//pinB 4 OUTPUT 
	
	sei();	//global interrupt enable
	
/**************************************************************************************************************/
	
	//getTime(&vreme_datum.hr, &vreme_datum.min, &vreme_datum.s, &vreme_datum.am_pm, _24_hour_format);
	//getDate(&vreme_datum.dy, &vreme_datum.dt, &vreme_datum.mt, &vreme_datum.yr);
	//setTime(23, 59, 55, am, _24_hour_format);
	//int i;
	
	//EEPROM_write(UPALI_HR_ADR, 23);
	//EEPROM_write(UPALI_MIN_ADR, 25);
	//EEPROM_write(UGASI_HR_ADR, 5);
	//EEPROM_write(UGASI_MIN_ADR, 45);
	
	/* citanje on-off vremena iz eeproma pri inicijalizaciji */
	vreme_paljenja.hr = EEPROM_read(UPALI_HR_ADR);
	vreme_paljenja.min = EEPROM_read(UPALI_MIN_ADR);
	vreme_gasenja.hr = EEPROM_read(UGASI_HR_ADR);
	vreme_gasenja.min = EEPROM_read(UGASI_MIN_ADR);
	
	jednokratno_paljenje.hr = EEPROM_read(JEDNOK_UPALI_HR_ADR);
	jednokratno_paljenje.min = EEPROM_read(JEDNOK_UPALI_MIN_ADR);
	jednokratno_gasenje.hr = EEPROM_read(JEDNOK_UGASI_HR_ADR);
	jednokratno_gasenje.min = EEPROM_read(JEDNOK_UGASI_MIN_ADR);
	
	snap_vreme_paljenja = vreme_paljenja;
	snap_vreme_gasenja = vreme_gasenja;
	
	/*
	//ispis eeproma na terminal
	for (i=0; i<1024; i++)
	{
		sprintf(bafer, "%d: %4d \n", i, EEPROM_read(i));
		uart_send_str(bafer);		
	}
	*/
	
	
	
    while (1) 
    {
		
		/* polling tastera */
		//tasteri = PIND;
		tasteri = ocitaj_tastere();
		
		/* ocitava napon i struju */
		adc_read();			
		
		
		sprintf(bafer, "%4d  ", adc_napon_raw);
		uart_send_str(bafer);
		sprintf(bafer, "%4d  ", adc_struja_raw);
		uart_send_str(bafer);
		
		sprintf(bafer, "%4d  ", napon);
		uart_send_str(bafer);
		dtostrf(struja, 4, 1, bafer);
		uart_send_str(bafer);
		uart_send_str("  ");
		dtostrf(snaga, 4, 1, bafer);
		uart_send_str(bafer);
		uart_send_str("  ");
		dtostrf(energija, 5, 1, bafer);
		uart_send_str(bafer);
		uart_send_str("\n");
		
		sprintf(bafer, "%d", DS3231_Read(ageoffsetREG));	//citam registar za kalibraciju u RTC 
		uart_send_str(bafer);
		uart_send_str("\n");
		
		/* bez obzira na STATE provera vremena treba da ide na 1s odnosno provera
		   da li grejac treba biti ukljucen ili iskljucen. Donji deo koda (swithc-case) ne bi trebao da koci program */
		if (flag_pc_int_pomocni)		//ide na 1 sekund
		{
			flag_pc_int_pomocni = 0;
			/* da sam koristio isti flag kao u automatu stanja, a ovde ga resetujem, dole nikada
			   ne bi bio ispunjen uslov za flag_pc_int==1 */
			
			getTime(&vreme_datum.hr, &vreme_datum.min, &vreme_datum.s, &vreme_datum.am_pm, _24_hour_format);
			
			/* integraljenje(sumiranje) snage je enerija. E = P * t     */
			energija += (snaga/3600.0);		//posto merim u kWh, a ovo ide na 1 sekund, a sat ima 3600s
			
			//sprintf(bafer, "%02d:%02d:%02d", vreme_datum.hr, vreme_datum.min, vreme_datum.s);
			//uart_send_str(bafer);
			//uart_send_str("\n"); //novi red
		
			
			/* JEDNOKRATNI PERIOD PALJENJA */						
			if (jednok_on_off==1)
			{
				/* ako je setovan flag, pali bojler jednokratno */
				/* potrebno je resetovati flag po isteku perioda */
				if ( period_paljenja(&jednokratno_paljenje, &jednokratno_gasenje, &vreme_datum) )	//vraca 0 ili 1 (ugasi - upali)
				{
					ukljuceno = 1;			//ako je IF ispunjen setuj promenljivu "ukljuceno"
					jednok_se_desio = 1;
				}
				else if(jednok_se_desio)												//jednok. setovan, ALI ISTEKAO
				{																		//proverava se tek kad period paljenja vraca 0, tj kada gornji IF nije ispunjen
					ukljuceno = 0;
					jednok_on_off = 0;		//resetujem flag		
					jednok_se_desio = 0;	//resetujem i ovaj flag. Da nema ovoga Jednokratno paljenje nikada ne bi startovalo ako tek treba da se desi
				}
			}
			else
				ukljuceno = 0;
			
			/* GLAVNI PERIOD; paljenje/gasenje releja > grjaca bojlera; edit: zapravo o grejacu odlucuje termostat bojlera, ovim se pali bojler */
			ukljuceno |= period_paljenja(&vreme_paljenja, &vreme_gasenja, &vreme_datum);	//ILI veza glavnog i jednok. perioda
			
			if (ukljuceno)
				PORTB |= 1<<PINB5;   //high
			else
				PORTB &= ~(1<<PINB5);	//low
					
		}
		
		
		
		/* automat stanja za menije na displeju */
		fsm_lcd_menu();
		

    }
}


uint8_t period_paljenja(Time_date *On_time, Time_date *Off_time, Time_date *CurrentTime)
{
	/* pali bojler u vremenskom intervalu koji se prosledi ovoj funkciji, odnosno gasi ga van intervala */
	/* RETURN: 0 OR 1 */
	/* Dovoljno je uzeti u obzir samo sate i minute, sekunde nisu od znacaja */
	/* Da bih olaksao sebi, sate i minute cu spajati u jednu promenljivu, npr. 22:15 se biti 2215 */
	/* Prelazak preko 00:00 je granicni slucaj i desava se jedino kada je vreme ON VECE od vreme OFF */
	/* Izlazni signal (Grejac upaljen) treba da bude uvek HIGH dok period traje */
	
	uint8_t stanje = 0;
	
	static uint16_t vreme_on;
	static uint16_t vreme_off;
	static uint16_t vreme_trenutno;
	
	//char buff[50];
	
	/* prebacim sate i minute u jednu promenljivu */
	vreme_on = (On_time->hr)*100 + On_time->min;
	vreme_off = (Off_time->hr)*100 + Off_time->min;
	vreme_trenutno = (CurrentTime->hr)*100 + CurrentTime->min;
	
	
	//sprintf(buff, "ON:%d OFF:%d Tr:%d",vreme_on, vreme_off, vreme_trenutno);
	//send_str(buff);
	//send_str("\n"); //novi red
	
	
	if (vreme_on >= vreme_off)  // >= ?? razmotri dodatno SVE granicne slucajeve; edit:provereno, sve u fulu radi
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

void fsm_lcd_menu()
{
	int pom;
	
	switch(STATE)
	{
		
		case DISPL1:
				/* prvi, tj. glavni meni sa ispisom tren. vremena i vremena on/off */
				/* ispis vremena svaki sekund dok je u ovom CASE-u */
				
				if ( displ_flag_shot )	//samo prvi put kad se uleti
				{
					displ_flag_shot = 0; //resetujem flag, i zabranim ponovni ulazak
					timer_disp_cycle = 0;	//start tajmera
				}
				//onemogucim meni ispis napona, struje.. za svrhe debagovanja
				if (timer_disp_cycle > 7000)	//7 sekundi
				{
					displ_flag_shot = 1; //opet dozvolim, pri izlazku iz ovog stejta
					STATE = DISPL2;
				}
						
				if(flag_pc_int)		//pc int usled signala koji dolazi sa SQW pin sa RTC modula; 1 sekund
				{
					flag_pc_int = 0; //resetujem flag koji je u ISR
			
					getTime(&vreme_datum.hr, &vreme_datum.min, &vreme_datum.s, &vreme_datum.am_pm, _24_hour_format);
					sprintf(bafer, "%02d:%02d:%02d", vreme_datum.hr, vreme_datum.min, vreme_datum.s);
					
					//lcd1602_clear();
			
					/*izbegao sam celear-ovanje ekrana koje izaziva teperenje dipleja, sa dodavanjem razmaka pre i posle zeljenog ispisa */
					lcd1602_goto_xy(0,0);
					lcd1602_send_string("    ");
					lcd1602_send_string(bafer);
					lcd1602_send_string("    ");
			
					lcd1602_goto_xy(0,1);
					lcd1602_send_string("  ");
					sprintf(bafer, "%02d:%02d--%02d:%02d", vreme_paljenja.hr, vreme_paljenja.min, vreme_gasenja.hr, vreme_gasenja.min);
					lcd1602_send_string(bafer);
					lcd1602_send_string("  ");
			
				}
				
				if ( ocitaj_jedan_taster(tasteri, TASTER_ENTER) )	//taster enter stisnut
				{
					displ_flag_shot = 1;	//opet dozvolim, pri izlazku iz ovog stejta
					STATE = MENU1;
				}
					
		break;
		
		case DISPL2:
				//ispisuje napon, struju, snagu... Smenjuje se periodicno sa DISPL1, uz pomoc tajmera
				
				if(displ_flag_shot)	//startujem tajmer samo prvi put za ciklicno menjanje DIPL1 i DISPL2
				{
					displ_flag_shot = 0; //resetujem flag
					timer_disp_cycle = 0;	//start tajmera
				}
				
				if ( flag_prekid_100ms_VAkWh )	//Ispisujem napon, struju snagu, sa periodom 100ms, da nije zamrznut ispis vec da se vide promene
				{
					
					flag_prekid_100ms_VAkWh = 0; //reset flaga
					
					
					lcd1602_clear();
					
					sprintf(bafer, "%4dV", napon);
					lcd1602_goto_xy(1,0);
					lcd1602_send_string(bafer);
					
					dtostrf(struja, 4, 1, bafer);
					lcd1602_goto_xy(1,1);
					lcd1602_send_string(bafer);
					lcd1602_send_string("A");
					
					dtostrf(snaga, 4, 1, bafer);
					lcd1602_goto_xy(9,0);
					lcd1602_send_string(bafer);
					lcd1602_send_string("kW");
					
					/* dummy ispis */
					//lcd1602_goto_xy(0,0);
					//lcd1602_send_string("224V      2.2kW");
					//lcd1602_goto_xy(0,1);
					//lcd1602_send_string("9.88A    396kWh");
				}
				if (timer_disp_cycle > 4000)	//4 sekunde
				{
					displ_flag_shot = 1; //opet dozvolim, pri izlazku iz ovog stejta
					STATE = DISPL1;
				}
				
				
				if ( ocitaj_jedan_taster(tasteri, TASTER_ENTER) )	//taster enter stisnut
				{
					displ_flag_shot = 1; //opet dozvolim, pri izlazku iz ovog stejta
					STATE = MENU1;
				}
					
		break;
		
		case MENU1:
					//lcd1602_clear();
					pom = kursor;
					
					lcd1602_goto_xy(0, 0);
					lcd1602_send_string(">");	//fiksno, a djiram text za menije vertikalno
					lcd1602_goto_xy(0,1);
					lcd1602_send_string(" ");	//prazno polje ispod ">"

					lcd1602_goto_xy(1,0);
					lcd1602_send_string(menu1_txt[kursor]);
					if (kursor == KURSOR_JEDNOKRAT)			//jednokrat na prvoj liniji
					{
						if (jednok_on_off == 1)					//ako je on ispisi <ON> pored JEDNOKRATNO
						{
							lcd1602_goto_xy(12,0);
							lcd1602_send_string("<ON>");
						}
						else					//ako je off ispisi <OF> pored JEDNOKRATNO
						{
							lcd1602_goto_xy(12,0);
							lcd1602_send_string("<OF>");
						}
					}
					
					lcd1602_goto_xy(1,1);
					pom = (pom==KURSOR_MENU1_MAX) ? -1 : pom;	//if-else, wrap-around ekran; -1 da bi dole krenuo od nule, tj od pocetka
					lcd1602_send_string(menu1_txt[pom + 1]);
					if ( (pom+1) == KURSOR_JEDNOKRAT)			//jednokrat na drugoj liniji, sa wrap-around-om
					{
						if (jednok_on_off == 1)					//ako je on ispisi <ON> pored JEDNOKRATNO
						{
							lcd1602_goto_xy(12,1);
							lcd1602_send_string("<ON>");
						}
						else					//ako je off ispisi <OF> pored JEDNOKRATNO
						{
							lcd1602_goto_xy(12,1);
							lcd1602_send_string("<OF>");
						}
					}

		
					if ( ocitaj_jedan_taster(tasteri, TASTER_DOLE) )		//djira kursor vertikalno ka dole
					{
						kursor++;
						if(kursor > KURSOR_MENU1_MAX)
						kursor = 0;
					}
					else if ( ocitaj_jedan_taster(tasteri, TASTER_GORE) )	//djira kursor vertikalno ka gore
					{
						kursor--;
						if(kursor < 0)		//min je 0, logicno
						kursor = KURSOR_MENU1_MAX;
					}
					else if(kursor == KURSOR_PODSAT && ocitaj_jedan_taster(tasteri, TASTER_ENTER))
					{
						kursor = 0;			//resetujem kursor jer ostane memorisan
						STATE = POD_SAT;	//meni za podesavanje sata
					}
					else if(kursor == KURSOR_PODONOF && ocitaj_jedan_taster(tasteri, TASTER_ENTER))
					{
						kursor = 0;			//resetujem kursor jer ostane memorisan
						STATE = POD_ON_OFF;	//sub_meni za podesavanje on ili off vremena
					}
					else if(kursor == KURSOR_JEDNOKRAT && ocitaj_jedan_taster(tasteri, TASTER_ENTER))
					{
						kursor = 0;			//resetujem kursor jer ostane memorisan
						STATE = JEDNOKRATNO;	//sub_meni za podesavanje on ili off vremena jednokratno
					}
					else if( kursor == KURSOR_JEDNOKRAT && ( ocitaj_jedan_taster(tasteri, TASTER_LEVO) ||  ocitaj_jedan_taster(tasteri, TASTER_DESNO) ) ) //levo ili desno kad je na JEDNOKRATNO da togluje on/of
					{
						jednok_on_off = !jednok_on_off;	//toggle
					}
					else if ( ocitaj_jedan_taster(tasteri, TASTER_NAZAD) )				//taster nazad stisnut
					{
						kursor = 0;			//resetujem kursor jer ostane memorisan
						STATE = DISPL1;		//vraca se na prethodni meni, tj. glavni meni
					}
		break;
		
		case POD_SAT:
		
					/* da ocita trenutno vreme samo prvi puta kada upadne u ovaj case	*/
					if (displ_flag_shot)
					{
						displ_flag_shot = 0;
						snap_shot_vremena = vreme_datum;
						sprintf(bafer, "%02d:%02d:%02d", snap_shot_vremena.hr, snap_shot_vremena.min, snap_shot_vremena.s);
			
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
					else if ( ocitaj_jedan_taster(tasteri, TASTER_LEVO))		//kursor levo
					{
						kursor -= 3;
						if(kursor < 5)
							kursor = 5;
						lcd1602_goto_xy(kursor,1);
					}
					else if ( ocitaj_jedan_taster(tasteri, TASTER_GORE) )
					{
						if (kursor == 5)												//podesava SATE ++
							sati_ispis(&snap_shot_vremena.hr, bafer, &kursor, UVECAJ);
						else if (kursor == 8)											//podesava MINUTE ++
							minuti_ispis(&snap_shot_vremena.min, bafer, &kursor, UVECAJ);
						else if (kursor == 11)											//podesava SEKUNDE ++
							sekundi_ispis(&snap_shot_vremena.s, bafer, &kursor, UVECAJ);
					}
					else if ( ocitaj_jedan_taster(tasteri, TASTER_DOLE) )
					{
						if (kursor == 5)												//podesava SATE --
							sati_ispis(&snap_shot_vremena.hr, bafer, &kursor, UMANJI);
						else if (kursor == 8)											//podesava MINUTE --
							minuti_ispis(&snap_shot_vremena.min, bafer, &kursor, UMANJI);
						else if (kursor == 11)											//podesava SEKUNDE --
							sekundi_ispis(&snap_shot_vremena.s, bafer, &kursor, UMANJI);
					}
					else if ( ocitaj_jedan_taster(tasteri, TASTER_ENTER) )				//Potvrdi vreme koje vidis na ekranu i promeni vreme; izadji iz menija
					{
						/* vreme se upise tek kad se izleti iz ovog menija, tj. ono vreme koje stoji na ekranu ce biti upisano */
						setTime(snap_shot_vremena.hr, snap_shot_vremena.min, snap_shot_vremena.s, am, _24_hour_format);
						displ_flag_shot = 1;	    //dozvolim ponovno citanje tr vremena kada se udje u ovaj case
						kursor = 0;					//reset kursora
						lcd1602_cursor_blink(0);	//isklucim blinking cursor
						STATE = MENU1;				//vraca se u prethodni meni
					}
					else if ( ocitaj_jedan_taster(tasteri, TASTER_NAZAD) )			//IZLAZ iz menija
					{
						/* Vreme setujem samo ako je stisnut ENTER, ako je stisnuto NAZAD, vreme se ne dira, tj. ovde se ne dira */
								//setTime(sanp_shot_vremena.hr, sanp_shot_vremena.min, sanp_shot_vremena.s, am, _24_hour_format);
						displ_flag_shot = 1;	    //dozvolim ponovno citanje tr vremena kada se udje u ovaj case
						kursor = 0;					//reset kursora
						lcd1602_cursor_blink(0);	//isklucim blinking cursor
						STATE = MENU1;				//vraca se u prethodni meni
					}
		
		break;
		
		case POD_ON_OFF:																				
					/* podesavanje perioda paljenja i gasenja */
					/* da ispise samo prvi puta kada upadne u ovaj case da ne djira bezveze	*/
					if (displ_flag_shot)
					{
						displ_flag_shot = 0;		//zabranjujem ponovni ispis sve dok je u ovom case-u. Dozvoljava se kada izleti iz njega
						
						snap_vreme_paljenja = vreme_paljenja;	
						snap_vreme_gasenja = vreme_gasenja;
						
						kursor = 3;			//hh1:mm1 = 3,6 ; hh2:mm2 = 10,13
						
						lcd1602_goto_xy(0,0);
						lcd1602_send_string("  UPALI--UGASI  ");
						
						sprintf(bafer, "%02d:%02d--%02d:%02d", snap_vreme_paljenja.hr, snap_vreme_paljenja.min, snap_vreme_gasenja.hr, snap_vreme_gasenja.min);
						
						lcd1602_goto_xy(0,1);
						lcd1602_send_string("  ");
						lcd1602_send_string(bafer);
						lcd1602_send_string("  ");
						
						lcd1602_goto_xy(kursor,1);
						lcd1602_cursor_blink(1);
						
					}
					if ( ocitaj_jedan_taster(tasteri, TASTER_DESNO) )		//kursor desno
					{
						kursor += 3;
						if(kursor > 6 && kursor <10)	//zbog asimetrije pri prikazu: 23:30--05:04
							kursor = 10;
						else if (kursor > 13)
							kursor = 13;
							
						lcd1602_goto_xy(kursor,1);
					}
					else if ( ocitaj_jedan_taster(tasteri, TASTER_LEVO))		//kursor levo
					{
						kursor -= 3;
						if(kursor < 10 && kursor > 6)	//zbog asimetrije pri prikazu: 23:30--05:04
							kursor = 6;
						else if(kursor < 3)
							kursor = 3;
							
						lcd1602_goto_xy(kursor,1);
					}
					else if ( ocitaj_jedan_taster(tasteri, TASTER_GORE) )
					{
						if (kursor == 3)												//podesava SATE_ON ++
							sati_ispis(&snap_vreme_paljenja.hr, bafer, &kursor, UVECAJ);
						else if (kursor == 6)											//podesava MINUTE_ON ++
							minuti_ispis(&snap_vreme_paljenja.min, bafer, &kursor, UVECAJ);
						else if (kursor == 10)											//podesava SATE_OFF ++
							sati_ispis(&snap_vreme_gasenja.hr, bafer, &kursor, UVECAJ);
						else if (kursor == 13)											//podesava MINUTE_OFF ++
							minuti_ispis(&snap_vreme_gasenja.min, bafer, &kursor, UVECAJ);
					}
					else if ( ocitaj_jedan_taster(tasteri, TASTER_DOLE) )
					{
						if (kursor == 3)												//podesava SATE_ON --
							sati_ispis(&snap_vreme_paljenja.hr, bafer, &kursor, UMANJI);
						else if (kursor == 6)											//podesava MINUTE_ON --
							minuti_ispis(&snap_vreme_paljenja.min, bafer, &kursor, UMANJI);
						else if (kursor == 10)											//podesava SATE_OFF --
							sati_ispis(&snap_vreme_gasenja.hr, bafer, &kursor, UMANJI);
						else if (kursor == 13)											//podesava MINUTE_OFF --
							minuti_ispis(&snap_vreme_gasenja.min, bafer, &kursor, UMANJI);
					}
					else if ( ocitaj_jedan_taster(tasteri, TASTER_ENTER) )				//ENTER vreme i izlaz iz menija
					{
						vreme_paljenja = snap_vreme_paljenja;	//konacno upisem
						vreme_gasenja = snap_vreme_gasenja;
						
						/* upis u eeprom */
						EEPROM_write(UPALI_HR_ADR, vreme_paljenja.hr);
						EEPROM_write(UPALI_MIN_ADR, vreme_paljenja.min);
						EEPROM_write(UGASI_HR_ADR, vreme_gasenja.hr);
						EEPROM_write(UGASI_MIN_ADR, vreme_gasenja.min);
						
						displ_flag_shot = 1;			//dozvolim ponovni ispis
						kursor = 0;					//reset kursora
						lcd1602_cursor_blink(0);	//isklucim blinking cursor
						STATE = MENU1;				//vraca se u prethodni meni
					}
					else if ( ocitaj_jedan_taster(tasteri, TASTER_NAZAD) )			//IZLAZ iz menija
					{
						displ_flag_shot = 1;			//dozvolim ponovni ispis
						kursor = 0;					//reset kursora
						lcd1602_cursor_blink(0);	//isklucim blinking cursor
						STATE = MENU1;				//vraca se u prethodni meni
					}
					
		break;
		
		case JEDNOKRATNO:
					//da opali samo jednom, bez obzira na glavni period
					/* prakticno kopija POD-ON-OFF samo se u drugu promenljivu belezi, cak pozajmljujem i iste tmp promenljive */
					/* podesavanje perioda paljenja i gasenja */
					/* da ispise samo prvi puta kada upadne u ovaj case da ne djira bezveze	*/
					if (displ_flag_shot)
					{
						displ_flag_shot = 0;		//zabranjujem ponovni ispis sve dok je u ovom case-u. Dozvoljava se kada izleti iz njega
						
						snap_vreme_paljenja = jednokratno_paljenje;
						snap_vreme_gasenja = jednokratno_gasenje;
						
						kursor = 3;			//hh1:mm1 = 3,6 ; hh2:mm2 = 10,13
						
						lcd1602_goto_xy(0,0);
						lcd1602_send_string("  JEDNOKRATNO:  ");
						
						sprintf(bafer, "%02d:%02d--%02d:%02d", snap_vreme_paljenja.hr, snap_vreme_paljenja.min, snap_vreme_gasenja.hr, snap_vreme_gasenja.min);
						
						lcd1602_goto_xy(0,1);
						lcd1602_send_string("  ");
						lcd1602_send_string(bafer);
						lcd1602_send_string("  ");
						
						lcd1602_goto_xy(kursor,1);
						lcd1602_cursor_blink(1);
						
					}
					if ( ocitaj_jedan_taster(tasteri, TASTER_DESNO) )		//kursor desno
					{
						kursor += 3;
						if(kursor > 6 && kursor <10)	//zbog asimetrije pri prikazu: 23:30--05:04
						kursor = 10;
						else if (kursor > 13)
						kursor = 13;
						
						lcd1602_goto_xy(kursor,1);
					}
					else if ( ocitaj_jedan_taster(tasteri, TASTER_LEVO))		//kursor levo
					{
						kursor -= 3;
						if(kursor < 10 && kursor > 6)	//zbog asimetrije pri prikazu: 23:30--05:04
						kursor = 6;
						else if(kursor < 3)
						kursor = 3;
						
						lcd1602_goto_xy(kursor,1);
					}
					else if ( ocitaj_jedan_taster(tasteri, TASTER_GORE) )
					{
						if (kursor == 3)												//podesava SATE_ON ++
						sati_ispis(&snap_vreme_paljenja.hr, bafer, &kursor, UVECAJ);
						else if (kursor == 6)											//podesava MINUTE_ON ++
						minuti_ispis(&snap_vreme_paljenja.min, bafer, &kursor, UVECAJ);
						else if (kursor == 10)											//podesava SATE_OFF ++
						sati_ispis(&snap_vreme_gasenja.hr, bafer, &kursor, UVECAJ);
						else if (kursor == 13)											//podesava MINUTE_OFF ++
						minuti_ispis(&snap_vreme_gasenja.min, bafer, &kursor, UVECAJ);
					}
					else if ( ocitaj_jedan_taster(tasteri, TASTER_DOLE) )
					{
						if (kursor == 3)												//podesava SATE_ON --
						sati_ispis(&snap_vreme_paljenja.hr, bafer, &kursor, UMANJI);
						else if (kursor == 6)											//podesava MINUTE_ON --
						minuti_ispis(&snap_vreme_paljenja.min, bafer, &kursor, UMANJI);
						else if (kursor == 10)											//podesava SATE_OFF --
						sati_ispis(&snap_vreme_gasenja.hr, bafer, &kursor, UMANJI);
						else if (kursor == 13)											//podesava MINUTE_OFF --
						minuti_ispis(&snap_vreme_gasenja.min, bafer, &kursor, UMANJI);
					}
					else if ( ocitaj_jedan_taster(tasteri, TASTER_ENTER) )				//ENTER vreme i izlaz iz menija
					{
						jednokratno_paljenje = snap_vreme_paljenja;	//konacno upisem
						jednokratno_gasenje = snap_vreme_gasenja;
						
						/* upis u eeprom */
						EEPROM_write(JEDNOK_UPALI_HR_ADR, jednokratno_paljenje.hr);
						EEPROM_write(JEDNOK_UPALI_MIN_ADR, jednokratno_paljenje.min);
						EEPROM_write(JEDNOK_UGASI_HR_ADR, jednokratno_gasenje.hr);
						EEPROM_write(JEDNOK_UGASI_MIN_ADR, jednokratno_gasenje.min);
						
						displ_flag_shot = 1;			//dozvolim ponovni ispis
						kursor = 0;					//reset kursora
						lcd1602_cursor_blink(0);	//isklucim blinking cursor
						STATE = MENU1;				//vraca se u prethodni meni
					}
					else if ( ocitaj_jedan_taster(tasteri, TASTER_NAZAD) )			//IZLAZ iz menija
					{
						displ_flag_shot = 1;			//dozvolim ponovni ispis
						kursor = 0;					//reset kursora
						lcd1602_cursor_blink(0);	//isklucim blinking cursor
						STATE = MENU1;				//vraca se u prethodni meni
					}
					
		break;
		
		default: {}
		
	}
}