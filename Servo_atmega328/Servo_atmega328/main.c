/*
 * Servo_atmega328.c
 *
 * Created: 19.7.2018. 18.29.15
 * Author : Kovacevic
 */ 

#include "common.h"
#include "timer.h"
#include "pwm.h"
#include "servo.h"
#include "uart.h"
#include "external_interrupt.h"

char bafer[30];

int main(void)
{
	//int i = 0;
  
	timer_1ms_init();
	pwm_init();
	uart_init();
	ext_int_init();
	
	sei();		//NEMOJ ZABORAVITI, global interrupt enable
	

	
    while (1) 
    {
		/*
		for(i=0; i<=180; i+=10)
		{
			OCR1A = set_servo_angle(i, 50, 100);
			_delay_ms(100);
		}
		
		for(i=180; i>=0; i-=10)
		{
			OCR1A = set_servo_angle(i, 50, 100);
			_delay_ms(20);
		}
		*/
		
		
		if(flag_100ms)
		{
			flag_100ms = 0; //resetuj flag koji se setuje u ISR-u
			
			/*
			send_str("  sys_time:");
			ultoa(sys_time, bafer, 10);
			send_str(bafer);
			
			send_str("  flag_pritisnut:");
			utoa(flag_pritisnut, bafer, 10);
			send_str(bafer);
			
			send_str("  opadajuca:");
			utoa(opadajuca, bafer, 10);
			send_str(bafer);
			
			send_str("\n");
			
			*/
			
		
		}
		
		
		
    }
	
	return 0;
}



