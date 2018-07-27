/*
 * timer.h
 *
 * Created: 20.7.2018. 20.53.59
 *  Author: Kovacevic
 */ 


#ifndef TIMER_H_
#define TIMER_H_

extern const uint_least8_t TOP_timer; //deklaracija = postoji. Definicija u timer.c
extern volatile uint_fast8_t flag_100ms;
extern volatile uint_fast32_t sys_time;
extern volatile uint_fast8_t flag_pritisnut;
extern const uint_least16_t interval;
extern volatile uint_fast8_t PIND2_old;


extern void timer_1ms_init();	//deklaracija a ne definicija. Znaci da postoji ovo. Prototip.
extern volatile uint_fast8_t pritisnut;



#endif /* TIMER_H_ */