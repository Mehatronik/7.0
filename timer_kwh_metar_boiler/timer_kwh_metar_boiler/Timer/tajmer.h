/*
 * tajmer.h
 *
 * Created: 2.12.2017. 22.09.10
 *  Author: Ko''
 */ 


#ifndef TAJMER_H_
#define TAJMER_H_

#include <inttypes.h>

#define DEBOUNCE_TIME 50					//50*1ms vezano je za osnovni tajmer prekid, tj za brojanje prekida
#define DEBOUNCE_TIME_half DEBOUNCE_TIME/2	//25ms

extern volatile uint8_t flag_tajmer0_prekid;
extern volatile uint8_t flag_prekid_10ms;
//extern volatile uint8_t brojac_prekida_tajmera0;
extern volatile uint8_t flag_prekid_debounce_time;
extern volatile uint8_t flag_prekid_debounce_time_half;


extern void tajmer0_init();	//prototip funkcije



#endif /* TAJMER_H_ */