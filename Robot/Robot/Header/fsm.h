/*
 * h_bridge.h
 *
 * Created: 14.12.2016 9:17:45
 *  Author: Mirko
 */ 


#ifndef FSM_H_
#define FSM_H_

#include "common.h"

#define  TIMER_NUM		5
#define  TIMER_90S		0
#define  TIMER_300MS	1
#define  TIMER_POKRET	2
#define  MAX_EVENTS		10

#define EVENT_NONE -1
#define EVENT_CINC  0
#define EVENT_LEVI_TASTER 3
#define EVENT_DESNI_TASTER 2

#define TIMER_EVENT 100
#define TIMER_90S_EVENT (TIMER_EVENT + TIMER_90S)
#define TIMER_300MS_EVENT (TIMER_EVENT + TIMER_300MS)
#define TIMER_POKRET_EVENT (TIMER_EVENT + TIMER_POKRET)

#define  STATE_START			0
#define  STATE_NAPRED			1 
#define  STATE_LEVI_PRITISNUT	2
#define  STATE_OKRET_DESNO		3
#define  STATE_UNAZAD			4
#define  STATE_OKRET_180		5
#define  STATE_KRAJ    1000


void init_FSM();
void cycle_FSM();

void timer_event();
void pin_event();

#endif /* H_BRIDGE_H_ */