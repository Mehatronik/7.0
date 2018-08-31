/*
 * ad_konverzija.h
 *
 * Created: 1.12.2017. 22.08.59
 *  Author: Ko''
 */ 


#ifndef AD_KONVERZIJA_H_
#define AD_KONVERZIJA_H_

#include <inttypes.h>

#define  BR_SEMPLOVA  20


extern volatile unsigned adc_res[4][BR_SEMPLOVA];

extern volatile unsigned ref_napon_sa_pot, mereni_napon, merena_struja;
extern volatile uint8_t adc_low, adc_high;
extern volatile uint8_t ad_kanal;

void ADC_init();		//prototip funkcije

#endif /* AD_KONVERZIJA_H_ */