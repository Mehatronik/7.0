/*
 * tajmer.h
 *
 * Created: 2.12.2017. 22.09.10
 *  Author: Ko''
 */ 


#ifndef TAJMER_H_
#define TAJMER_H_

void tajmer0_init();	//prototip funkcije


extern volatile unsigned char flag_tajmer0_prekid, flag_prekid_10ms;
extern volatile unsigned int brojac_prekida_tajmera0;
extern volatile long int Upravljanje;
extern volatile double ugaona_brzina,Kp, Ki;
extern volatile double greska, suma_greske;
extern const volatile double stepeniPOms_to_rpm;
extern volatile double relativni_ugao;

#endif /* TAJMER_H_ */