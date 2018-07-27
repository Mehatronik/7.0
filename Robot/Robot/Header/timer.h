/*
 * timer.h
 *
 * Created: 4.11.2016 16:50:23
 *  Author: Tasevski
 */ 


#ifndef TIMER_H_
#define TIMER_H_

extern volatile unsigned long systime;
extern volatile unsigned char f_20ms, flag_1s;

void init_timer();
void osc_init_32MHz(void);

#endif /* TIMER_H_ */