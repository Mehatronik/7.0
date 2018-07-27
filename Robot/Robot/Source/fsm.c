/*
 * fsm.c
 *
 * Created: 10.1.2018 10:01:14
 *  Author: Tasevski
 */ 

#include "common.h"

unsigned long int timer[TIMER_NUM]; // u milisekundama
int event_queue[MAX_EVENTS];
unsigned int queue_start;
unsigned int queue_end;

unsigned char port_b_old;
unsigned char port_b;
unsigned int  state;

//char message[30];

void add_event(int event_id){
	event_queue[queue_end] = event_id;
	queue_end = (queue_end + 1) % MAX_EVENTS;
}

int get_event(){
	if (queue_end == queue_start) return EVENT_NONE;
	int ev=event_queue[queue_start];
	queue_start = (queue_start + 1) % MAX_EVENTS;
	return ev;
}

void set_timer(unsigned char timer_id, unsigned long int duration)
{
	timer[timer_id] = duration;
}

void stop_timer(unsigned int timer_id)
{
	timer[timer_id] = 0;
}

void timer_event()
{
	for (int i=0; i<TIMER_NUM; i++){
		if (timer[i]){
			timer[i]--;
			if (!timer[i])
				add_event(TIMER_EVENT + i);
		}
	}
	
}

void pin_event()
{
	unsigned char promena = (port_b_old ^ port_b) & (~(PORTB.IN ^ port_b));		
	if (promena & 0x01 & port_b) add_event(EVENT_CINC);
	
	if (promena & 0x02 & port_b) add_event(EVENT_LEVI_TASTER);
	if (promena & 0x04 & port_b) add_event(EVENT_DESNI_TASTER);
	
	port_b_old = port_b;
	port_b = PORTB.IN;
}

void init_FSM(){
	for (int i=0; i<TIMER_NUM; i++) 
		stop_timer(i);
	queue_end = 0;
	queue_start = 0;
	
	port_b = 0xFF;
	port_b_old = 0xFF;
	
	state =  STATE_START;
}

void cycle_FSM(){
	int ev = get_event();
	
	switch (state){
		case STATE_START:
			start_proc(ev);
			break;
		case STATE_NAPRED:
			napred_proc(ev);
			break;
	/*	case STATE_LEVI_PRITISNUT:
		    levi_pritisnut(ev);
			break;
		case STATE_OKRET_DESNO:
			okret_desno(ev);
			break;
		case STATE_UNAZAD:
			unazad(ev);
			break;
		case STATE_OKRET_180:
			okret_180(ev);
			break; */
	}
	
}

void levi_pritisnut(int ev)
{
	switch (ev){
		case EVENT_DESNI_TASTER:
			des_mav_L = -10;
			des_mav_R = -10;
			state = STATE_UNAZAD;
			set_timer(TIMER_POKRET, 2000);
			break;
		case TIMER_300MS_EVENT:
			des_mav_L = 0;
			des_mav_R = -10;
			state = STATE_OKRET_DESNO;
			set_timer(TIMER_POKRET, 1666);
		    //90
			break;	
		case TIMER_90S_EVENT:
			des_mav_L = 0;
			des_mav_R = 0;
			state = STATE_KRAJ;
			break;	
	}
} 


void start_proc(int ev)
{
	switch (ev){
		case EVENT_CINC:
			set_timer(TIMER_90S, 90000); //ms
			
			Xd_mm = 100;			//zeljeno
			Yd_mm = 0;
			fid_deg = 0;	//zeljeni ugao
			//des_mav_L = 0;	//10
			//des_mav_R = 0;
			state = STATE_NAPRED;
			break;
	}
}

void napred_proc(int ev)
{
	switch(ev){
		case TIMER_90S_EVENT:
			des_mav_L = 0;
			des_mav_R = 0;
			
			state = STATE_KRAJ;
			break;
		case EVENT_LEVI_TASTER:
		case EVENT_DESNI_TASTER:
			set_timer(TIMER_300MS, 100);
			state =  STATE_LEVI_PRITISNUT;
			break;
	}
}

void okret_desno(int ev)
{
	switch (ev){
		case TIMER_90S_EVENT:
			des_mav_L = 0;
			des_mav_R = 0;
			
			
			
			state = STATE_KRAJ;
			break;
			
		case TIMER_POKRET_EVENT:
			des_mav_L = 10;
			des_mav_R = 10;
			state = STATE_NAPRED;
			break;
	}
}

void unazad(int ev){
	switch (ev){
		case TIMER_90S_EVENT:
			des_mav_L = 0;
			des_mav_R = 0;
			state = STATE_KRAJ;
			break;
		
		case TIMER_POKRET_EVENT:
			des_mav_L = 10;
			des_mav_R = -10;
			state = STATE_OKRET_180;
			set_timer(TIMER_POKRET, 1666);
			break;
	}
}

void okret_180(int ev){
	switch (ev){
		case TIMER_90S_EVENT:
			des_mav_L = 0;
			des_mav_R = 0;
			state = STATE_KRAJ;
			break;
		
		case TIMER_POKRET_EVENT:
			des_mav_L = 10;
			des_mav_R = 10;
			state = STATE_NAPRED;
			break;
	}
}
