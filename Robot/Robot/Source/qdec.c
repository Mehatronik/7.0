/*
 * qdec.c
 *
 * Created: 14.12.2016 8:47:02
 *  Author: Mirko
 */ 

#include "qdec.h"

volatile unsigned char MOTOR_QDEC_OWF_R;
volatile unsigned char MOTOR_QDEC_OWF_L;

void init_qdec(void)
{
	//enkoderski ulazi
	PORT_ConfigurePins( &PORTD,
						0xFF,							//bilo 0xFD tj. 1111 1101
						0,
						0,
						PORT_OPC_PULLUP_gc,				//zasto sugavi pull up na enkodere kad sami daju aktivan signal? !!??
						PORT_ISC_FALLING_gc);			//wtf, zasto samo falling ako radi kvadraturno dekodiranje, trebalo bi obe ivice da hvata???
	
	PORT_SetPinsAsInput( &PORTD, 0xFF );		//bilo 0xfd iz nekog metafizickog razloga
	
			// Motor_desni PORTD6,7 - Event system 0 - TCD0
	PORTD.PIN6CTRL |= 0b01000000;		//invertuje se pin, da enkoder broji u drugom smeru
	EVSYS.CH0MUX = 0b01101110;			// PORTD6 mux input -> MOTOR_QDEC.LEFT
	EVSYS.CH0CTRL = 0b00001001;			//enable QDEC, filtering 2x
			//Timer/Counter TCD1
	TCD1.CTRLA = 0b00000001;			//clock source
	TCD1.CTRLD = 0b01101000;  			//event action=QDEC, event source=ch0 [01101000]
	TCD1.INTCTRLA = ( TCD1.INTCTRLA & ~TC1_OVFINTLVL_gm ) | 1;		//interapt na owerflov

				//Motor_levi	PORTD4,5 - Event system 2 - TCD1
				//PORTD.PIN4CTRL |= 0b01000000;	//invertuje se pin, da enkoder broji u drugom smeru
	EVSYS.CH2MUX = 0b01101100; // PORTD4 mux input -> MOTOR_QDEC.RIGHT
	EVSYS.CH2CTRL = 0b00001001;	//enable QDEC, filtering 2x
				//Timer/Counter TCD0 -> QDEC.RIGHT
	TCD0.CTRLA = 0b00000001;	//clock source
	TCD0.CTRLD = 0b01101010;  	//event action=QDEC, event source=ch2
	TCD0.INTCTRLA = ( TCD0.INTCTRLA & ~TC0_OVFINTLVL_gm ) | 1;
	
				//Pasivni_levi	PORTD2,3 - Event system 4 - TCE0
				//PORTD.PIN2CTRL |= 0b01000000;	//invertuje se pin, da enkoder broji u drugom smeru
	EVSYS.CH4MUX = 0b01101010; // PORTD2 mux input -> PASIVNI_LEVI
	EVSYS.CH4CTRL = 0b00001001;	//enable QDEC, filtering 2x
				//Timer/Counter TCE0 -> QDEC.RIGHT
	TCE0.CTRLA = 0b00000001;				//clock source
	TCE0.CTRLD = 0b01101100;  				//event action=QDEC, event source=ch4
	TCE0.INTCTRLA = ( TCE0.INTCTRLA & ~TC0_OVFINTLVL_gm ) | 1;
	
				//Pasivni_desni	PORTD0,1 - Pasivni_desni_CNT
	PORT_ConfigurePins( &PORTD,
						0b00000011,
						0,
						0,
						PORT_OPC_PULLUP_gc,
						PORT_ISC_BOTHEDGES_gc);
	
	PORT_SetPinsAsInput( &PORTD, 0b00000011 );
				//PORTD.PIN0CTRL |= 0b01000000;	//invertuje se pin, da enkoder broji u drugom smeru
				//Configure Interrupt0 to have HI interrupt level on PD0
	PORT_ConfigureInterrupt0( &PORTD, PORT_INT0LVL_HI_gc, 0b00000001 );
				//Configure Interrupt1 to have HI interrupt level on PD1
	PORT_ConfigureInterrupt1( &PORTD, PORT_INT1LVL_HI_gc, 0b00000010 );
}

////LEVI MOTOR QDEC
ISR(TCD1_OVF_vect)
{
	if((TCD1.CTRLFSET & TC1_DIR_bm) == 0)
	MOTOR_QDEC_OWF_L = 1;
	
	if((TCD1.CTRLFSET & TC1_DIR_bm) == 1)
	MOTOR_QDEC_OWF_L = 2;
}

//DESNI MOTOR QDEC
ISR(TCD0_OVF_vect)
{
	if((TCD0.CTRLFSET & TC0_DIR_bm) == 0)
	MOTOR_QDEC_OWF_R = 1;
	
	if((TCD0.CTRLFSET & TC0_DIR_bm) == 1)
	MOTOR_QDEC_OWF_R = 2;
}
