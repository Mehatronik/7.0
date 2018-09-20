/*
 * DSPmega.h
 *
 * Created: 2012-10-07 13:04:22
 *  Author: Anders
 */ 
 /********************************************************************
*Copyright (c) 2013 Anders Skoog									*		
*																	*
*Permission is hereby granted, free of charge, to any person		*
*obtaining a copy of this software and associated documentation		*
*files (the "Software"), to deal in the Software without			*
*restriction, including without limitation the rights to use,		*
*copy, modify, merge, publish, distribute, sublicense, and/or sell	*
*copies of the Software, and to permit persons to whom the			*
*Software is furnished to do so, subject to the following			*	
*conditions:														*
*																	*
*The above copyright notice and this permission notice shall be		*	
*included in all copies or substantial portions of the Software.	*
*																	*
*THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,	*
*EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES	*
*OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND			*
*NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT		*
*HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY,		*
*WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING		*
*FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR		*	
*OTHER DEALINGS IN THE SOFTWARE.    							 	*
*********************************************************************/
#ifndef F_CPU 
#define F_CPU 1600000UL 
#endif 

#ifndef DSPMEGA_H_
#define DSPMEGA_H_

#include <avr/interrupt.h> 
#include <avr/io.h>

#define BUFLENGTH 825
#define BUFP 360
#define BUFPH 150


#define LOWPASS 0x01
#define PITCHUP 0x2
#define PITCHDOWN 0x4
#define BITCRUSH 0x10
#define ECHO 0x20
#define PHASOR 0x80

typedef unsigned char uint8_t;
typedef unsigned int uint16_t;
extern volatile uint8_t effects;

extern void ADCSetup();
extern void PWMSetup();
extern uint8_t bitcrush(uint8_t sample);
extern uint8_t echo(uint8_t sample);
extern uint8_t phasor(uint8_t sample);
extern uint8_t pitch_down(uint8_t sample);
extern uint8_t pitch_up(uint8_t sample);
extern uint8_t lowpass(uint8_t sample);
#endif /* DSPMEGA_H_ */