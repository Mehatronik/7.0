/*
 * ad_konverzija.h
 *
 * Created: 1.12.2017. 22.08.59
 *  Author: Ko''
 */ 


#ifndef AD_KONVERZIJA_H_
#define AD_KONVERZIJA_H_



extern volatile int adc_res[4], adc_low, adc_high;
extern volatile int ref_brzina_sa_pot;
extern volatile int ad_kanal;

void ADC_init();		//prototip funkcije

#endif /* AD_KONVERZIJA_H_ */