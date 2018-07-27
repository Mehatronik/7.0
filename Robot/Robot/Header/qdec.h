/*
 * qdec.h
 *
 * Created: 14.12.2016 8:46:52
 *  Author: Mirko
 */ 


#ifndef QDEC_H_
#define QDEC_H_

#include "common.h"

extern volatile unsigned char MOTOR_QDEC_OWF_R;
extern volatile unsigned char MOTOR_QDEC_OWF_L;

void init_qdec(void);

#endif /* QDEC_H_ */