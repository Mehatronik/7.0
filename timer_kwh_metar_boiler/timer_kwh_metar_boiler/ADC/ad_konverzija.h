/*
 * ad_konverzija.h
 *
 * Created: 1.12.2017. 22.08.59
 *  Author: Ko''
 */ 


#ifndef AD_KONVERZIJA_H_
#define AD_KONVERZIJA_H_

extern uint16_t napon;
extern float struja;
extern float snaga;
extern uint16_t energija;


//prototipovi funkcija
extern void ADC_init();		
extern void adc_read();


#endif /* AD_KONVERZIJA_H_ */