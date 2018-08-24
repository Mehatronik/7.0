/*
 * ad_konverzija.h
 *
 * Created: 1.12.2017. 22.08.59
 *  Author: Ko''
 */ 


#ifndef AD_KONVERZIJA_H_
#define AD_KONVERZIJA_H_

#include <inttypes.h>

extern volatile int adc_res[4];
extern volatile float ref_napon_sa_pot, mereni_napon, merena_struja;
extern volatile int adc_low, adc_high;
extern volatile int ad_kanal;

void ADC_init();		//prototip funkcije

#endif /* AD_KONVERZIJA_H_ */