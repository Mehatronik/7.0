/*
 * DSPmega.c
 *
 * Created: 2012-10-07 12:28:13
 *  Author: Anders Skoog
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


#include "DSPmega.h"


volatile uint8_t effects= 0;




int main111111(void)
{

	
	ADCSetup();
	PWMSetup();
	buttonSetup();
    while(1)
    {
		if (PINB & (1<<PORTB6)){
			 effects&= ~PITCHUP;
		}else{
			effects|=PITCHUP;
		}			 
		if (PINB & (1<<PORTB7)){
			effects&= ~PITCHDOWN;
		}else{
			effects|=PITCHDOWN;
		}
		if (PIND & (1<<PORTD6)){
			effects&= ~BITCRUSH;
		}else{
			effects|=BITCRUSH;
		}
		 
		if (PIND & (1<<PORTD7)){
			effects&= ~ECHO;
		}else{
			effects|=ECHO;
		} 
		if (PINB & (1<<PORTB0)){
			effects&= ~PHASOR;
		}else{
			effects|=PHASOR;
		} 
		if (PINB & (1<<PORTB1)){
			effects&= ~LOWPASS;
		}else{
			effects|=LOWPASS;
		} 	
    }
}


extern void ADCSetup(){


	ADCSRA |= (1<<ADATE)|(1<<ADIE)|(1<<ADPS2)|(1<<ADPS0); //freerunning ADC, prescale 64, samplerate 19230Hz
	
	ADMUX |= (1<<MUX2)|(1<<MUX0)|(1 << ADLAR)|(1<<REFS0)|(1<<REFS1); //ADC5 (PC5) , 8bit adc, 1.1V ref
	ADCSRA |= (1 << ADEN)|(1 << ADSC); //enable ADC, enable interrupt
	sei();
	
}
extern void PWMSetup(){  
	DDRD |= 1<<PORTD5; //output PD5 
	 TCCR0A |= (1<<WGM00)|(1<<WGM01); // select fast pwm mode 3
	 TCCR0A |= (1<<COM0B1);          //Clear OC0B on Compare Match, set OCR0B at BOTTOM (non-inverting mode)
	 OCR0B = 128;						//duty cycle
	 TCCR0B |= (1<<CS00); 
}

extern void buttonSetup(){
	//pull up resistors
	PORTB |= (1<<PORTB6)|(1<<PORTB7)|(1<<PORTB1)|(1<<PORTB0);
	PORTD |= (1<<PORTD6)|(1<<PORTD7);
}

extern uint8_t bitcrush(uint8_t sample){
		
		sample=sample>>6;
        sample=sample<<6;         

        return sample;
}

extern uint8_t echo(uint8_t sample){
	static uint16_t indexWrite=0;
	static uint16_t indexRead=1;
	uint16_t temp=0;
	static uint8_t Buffer[BUFLENGTH]={0};
	static uint8_t downSample=1; //
		
	temp=sample  +  (Buffer[indexRead]>>1); //Reads sample and adds echo 
	
	Buffer[indexWrite]=(uint8_t)temp;	   

	if (downSample==1)
	{	
		indexRead++;
		if (indexRead==(BUFLENGTH-1)) indexRead=0;

		indexWrite++;
		if (indexWrite==(BUFLENGTH-1)) indexWrite=0;

		downSample=0;


	}else{
		downSample=1;
	}


	return  (uint8_t)temp;
}	



extern uint8_t phasor(uint8_t sample){
	static uint8_t Buffer[BUFPH]={0};	
	static uint16_t indexWrite=0;
	static uint16_t indexRead=1;
	static uint16_t temp=0;
	
	temp=(sample>>1)+(Buffer[indexRead]>>1); //Reads sample and adds flanger 
	
	Buffer[indexWrite]=sample;
	  

	indexRead++;
	if (indexRead>=BUFPH-1) indexRead=0;

	indexWrite++;
	if (indexWrite>=BUFPH) indexWrite=0;
	
	return  (uint8_t)temp;
}

extern uint8_t lowpass(uint8_t sample){
	static uint8_t filter[9]={0};
	
	filter[8]=filter[7];
	filter[7]=filter[6];
	filter[6]=filter[5];
	filter[5]=filter[4];
	filter[4]=filter[3];
	filter[3]=filter[2];
	filter[2]=filter[1];
	filter[1]=filter[0];
	filter[0]=sample;

	sample = (filter[0]*0 +filter[1]*0 + filter[2]*22 + filter[3]*75 +filter[4]*108 +filter[5]*75 + filter[6]*22 + filter[7]*0 + filter[8]*0)>>8; //fir order 7, window Kaiser (beta 2.5, fc 3000Hz)
	
	return  sample;
}


extern uint8_t pitch_up(uint8_t sample){
	static int indexWrite=0;
	static int indexRead=1;
	static uint8_t Buffer[BUFP]={0};
	
	Buffer[indexWrite]=sample;
	   
	indexRead+=2;

	if (indexRead>=(BUFP-1)) indexRead=0;

	indexWrite++;
	if (indexWrite>=(BUFP-1)) indexWrite=0;
	
		
	return  Buffer[indexRead];
}


extern uint8_t pitch_down(uint8_t sample){
	static int indexWrite=0;
	static int indexRead=1;
	static uint8_t half=0;
	static uint8_t Buffer[BUFP]={0};

	

	Buffer[indexWrite]=sample;
	  half++; 
	if(half==2){
		indexRead++;
		half=0;
	}
	

	if (indexRead>=(BUFP-1)) indexRead=0;

	indexWrite++;
	if (indexWrite>=(BUFP-1)) indexWrite=0;
	
	
		
	return  Buffer[indexRead];
}


ISR(ADC_vect) 
{ 
		uint8_t ADCdata=0;

        ADCdata=ADCH;

		if(effects&PITCHUP) ADCdata=pitch_up(ADCdata);
		if(effects&PITCHDOWN) ADCdata=pitch_down(ADCdata);
		if(effects&BITCRUSH) ADCdata=bitcrush(ADCdata);

				
		//only echo or phasor can be used at one time
		if(effects&ECHO)ADCdata=echo(ADCdata);
		if( effects&PHASOR) ADCdata=phasor(ADCdata);
		
		if(effects&LOWPASS) ADCdata=lowpass(ADCdata); //clean up the signal
		
        OCR0B = ADCdata;

} 
