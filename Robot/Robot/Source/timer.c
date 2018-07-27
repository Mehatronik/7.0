/*
 * timer.c
 *
 * Created: 4.11.2016 16:52:15
 *  Author: Tasevski
 */ 

#include "common.h"


volatile unsigned long systime=0;
volatile unsigned char f_20ms=0, flag_1s;

void init_timer()
{
	osc_init_32MHz();

	TCC0.CTRLA |= (1 << 2);		//preskaler 8, 1ms, 32MHz osc.
	TCC0.INTCTRLA |= 0b10;//; //(1 << 0);	// dozvola prekida, interapt level
	TCC0.PER = 4123;			// vrednost broja?a


}

void osc_init_32MHz(void)
{
	OSC.CTRL = OSC_RC32MEN_bm;						// Startujemo interni RC oscilator od 32 MHz.

	while ( !( OSC.STATUS & OSC_RC32MRDY_bm ) );	// Cekamo da se oscilator stabilizuje.
		
	CCP = CCP_IOREG_gc;								/* Zastita slucajne promene sadrzaja registara.
													   Nakon upisa u CCP registar, imamo 4 ciklusa da
													   upisemo vrednost u registar koji zelimo da zastitimo
													   u toku rada. */
	CLK.PSCTRL = 0x08;				/* Podesavanje preskalera za osnovni takt kontrolera i za
													   takt vecine modula na kontroleru. Preskaleri B i C su 2,
													   tako da dobijemo (32 / 2) / 2 = 8 MHz. Ulazi/izlazi, tajmeri
													   i vecina ostale periferije sada radi na 8 MHz. */
	CCP = CCP_IOREG_gc;
	CLK.CTRL = CLK_SCLKSEL_RC32M_gc;				// Postavljamo 32 MHz RC oscilator kao izvor osnovnog takta kontrolera.
}


ISR(TCC0_OVF_vect)//1ms   //ISR(TIMER1_OVF_vect)
{	
	systime++;			//simulator meri da ovaj prekid okida na 314 us, umesto na 1 ms (tj. 1000 us)
	
	PORT_TogglePins(&PORTC, 0xFF);	//toogle PC4 = fet5, za proveru okidanja tajmera na osciloskopu
	
	//upravljanje robotom na diferencijalni pogon sa odometrijom
	//odometrija	
	sens_robot();
	
	timer_event();
	pin_event();
	/*
	DuC_R = 1000;
	DuC_L = 1000;
	robot_direct_drive();
	*/
	
	
	robot_position_control();
	
	
	
	
	
	if(!(systime%100))
		flag_1s=1;		
}