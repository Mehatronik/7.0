/*
 * eksterni_interapt.h
 *
 * Created: 2.12.2017. 00.27.43
 *  Author: Ko''
 */ 


#ifndef EKSTERNI_INTERAPT_H_
#define EKSTERNI_INTERAPT_H_

void eksternInt_init();


extern volatile unsigned int impulsi;
extern volatile unsigned char smer_obrtanja;
extern volatile double ugao;
extern volatile unsigned int brojac_ext_interaptova;

#endif /* EKSTERNI_INTERAPT_H_ */