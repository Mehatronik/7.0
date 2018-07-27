/*
 * timer.c
 *
 * Created: 20.7.2018. 20.53.38
 *  Author: Kovacevic
 */ 

#include "common.h"
#include "external_interrupt.h"
#include "pwm.h"

const uint_least8_t TOP_timer = 249;	//definicija se radi jednom. Posto je extern u timer.h vide je i drugi fajlovi
const uint_least16_t interval = 500; //u ms
volatile uint_fast8_t flag_100ms = 0;
volatile uint_fast32_t sys_time = 0;  //64 bita nikada nece otici u overflow. teoretski to ce se desiti za oko 600 miliona godina
volatile uint_fast8_t flag_pritisnut = 0;

volatile uint_fast8_t PIND2_old = 1;
volatile uint_fast8_t pritisnut = 0;


void timer_1ms_init()	//sistemski tajmer, timer/counter0
{
	DDRD |= 1<<DDD6;	//PD6 izlaz = timer debug check on o-scope
	DDRD |= 1<<DDD7;
	DDRB |= 1<DDB5;     //led
	PORTB &= ~(1<<PORTB5); //led OFF
	
	DDRD &= ~(1<<DDD2);  //PD2 ulaz za taster
	PORTD |= 1<<PORTD2;  //PD2 pull up
	
	TCCR0A |= 1<<WGM01;				//CTC mode
	TCCR0B |= (1<<CS01)|(1<<CS00);	//prescaler = 64
	OCR0A = TOP_timer;				//top = 249;  1kHz
	TIMSK0 |= 1<<OCIE0A;			//compare match A (OCR0A) interrupt enable
	TCNT0 = 0;						//resetujem tc brojac da ovde krene od nule. Kada udari u OCR0A(249) resetuje se
	
}

ISR(TIMER0_COMPA_vect)	//1ms prekid
{
	
	/*
	
	if ((opadajuca==1) && (sys_time==1) && (!(PIND&0b100)) )
	{
		OCR1A = set_servo_angle(90, 0, 180);
		flag_pritisnut = 1;
		PORTB |= 1<<PORTB5; //led ON
	}
	else if(sys_time==1)	//ako nije ispunje gore treci uslov, u prvoj milisekundi
		opadajuca = 0; //dozvoli opet taster

	
	if (sys_time == interval  && flag_pritisnut)
		OCR1A = set_servo_angle(0, 0, 180);
		 

	
	if (sys_time == 2*interval && flag_pritisnut)
	{
		OCR1A = TOP_PWM; //servo off, Duc = 100%
		opadajuca = 0; //dozvoli opet taster
		flag_pritisnut = 0;
		
		PORTB &= ~(1<<PORTB5); //led OFF
	}
	*/
	
	if( ReadPin(PIND, 2)==0  &&  PIND2_old == ReadPin(PIND, 2) )
	{
		//taster pritisnut i debounce-ovan, jer je prosla 1ms od citanja sa PIND2_old
		//takoreci, stanje je stabilno posle 1ms
		pritisnut = 1;
		
		
	}
	else if ( ReadPin(PIND, 2)!=0  &&  PIND2_old == ReadPin(PIND, 2) )
	{
		//taster otpusten i debounce-ovan
		pritisnut = 0;
		
		
	}
	
	
	
	PIND2_old = ReadPin(PIND, 2);	//citanje sa maskom sa PD2,  PIND & 0b100;
	
	
	
	
	if (!(sys_time % 100))
	{
		flag_100ms = 1;
		PIND |= 1<<PIND7;
	}
	
	
	
	sys_time++;
	
	PIND |= 1<<PIND6;	//toggle
}