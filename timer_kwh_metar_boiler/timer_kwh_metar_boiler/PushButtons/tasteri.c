/*
 * tasteri.c
 *
 * Created: 30.4.2019. 14.14.55
 *  Author: Kovacevic
 */ 

#include "comm.h"
#include "tasteri.h"
#include "tajmer.h"

void tasteri_init()
{
	/* tasteri input */
	DDRD &= ~( (1<<TASTER_LEVO_pinPort) | (1<<TASTER_DESNO_pinPort) | (1<<TASTER_GORE_pinPort) | (1<<TASTER_DOLE_pinPort) );
	DDRB &= ~( (1<<TASTER_NAZAD_pinPort) | (1<<TASTER_ENTER_pinPort) );
	
	/* internal pull ups ON */
	PORTD |= (1<<TASTER_LEVO_pinPort) | (1<<TASTER_DESNO_pinPort) | (1<<TASTER_GORE_pinPort) | (1<<TASTER_DOLE_pinPort);
	PORTB |= (1<<TASTER_NAZAD_pinPort) | (1<<TASTER_ENTER_pinPort);
}


uint8_t ocitaj_tastere()
{
	/* posto imam 6 tastera spakovacu ih u jedan "registar" tj. 8-bit promenljivu
	   gde ce svaki bit predstavljati stanje tastera, a prva dva MSB su visak     */
	
	/* posto postoji problem bouncing-a, tastere ocitavam sa periodom npr. 50ms, tj.
	   koristim polling sistem. Ako je ocitavanje isto u uzastopna dva citanja, tasteri su
	   debouncovani i stanje u registru predstavlja stvarno stanje tastera		*/
	
	
	/* u tajmeru imam zapravo "2 tajmera" tj. procitam sve tastere prvi put pa posle 25ms (tj. Deb.Perioda/2)
	   opet procitam stanje. Ako se poklapaju konacno upisujem stanje iz dve pomocne promenljive u glavnu promenljivu
	   koja preslikava stvano stanje	*/

	static uint8_t tasteri_reg = 0xFF;
	uint8_t temp_reg1 = 0xFF;
	uint8_t temp_reg2 = 0xFF;
	
	if (flag_prekid_debounce_time_half)		//prvo citanje 25ms; zapravo perioda je 25ms jer je to razlika izmedju prvog i drugog citanja
	{
		flag_prekid_debounce_time_half = 0;	//reset flag
		
		//procitam stanje svih tastera i spakujem u jednu promenljivu
		//prvo citanje
		temp_reg1 = (PINB << 4 )|(PIND >> 4);	//prva 4 LSB su PIND a posle njih su 2 bita iz PINB

	}
	if (flag_prekid_debounce_time)		//drugo citanje 50ms, tj 25ms posle drugog
	{
		flag_prekid_debounce_time = 0;
		
		//drugo citanje
		temp_reg2 = (PINB << 4 )|(PIND >> 4);
		
		if (temp_reg1 == temp_reg2)
			tasteri_reg = temp_reg2;	//nebitno da li je 1 ili 2 jer su ovde isti
		
	}
	
	return tasteri_reg;
}