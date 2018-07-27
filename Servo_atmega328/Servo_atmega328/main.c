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
	uint8_t dozvola = 1, prethodno;
  
	timer_1ms_init();
	pwm_init();
	uart_init();
	//ext_int_init();
	
	sei();		//NEMOJ ZABORAVITI, global interrupt enable
	

	
    while (1) 
    {
		
		
		if (pritisnut && dozvola && !prethodno)
		{
			dozvola = 0;
			sys_time=0;   //startuj brojac
		}	
			
			
		if (sys_time == 0)
		{
			OCR1A = set_servo_angle(90, 0, 180);
		}
		else if (sys_time == 800)
		{
			OCR1A = set_servo_angle(0, 0, 180);
		}
		else if (sys_time == 1600)
		{
			OCR1A = TOP_PWM;
			
			
			   dozvola = 1;
		}
		prethodno=pritisnut;	
			
		
				
		
		
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
			
			send_str("  pritisnut:");
			utoa(pritisnut, bafer, 10);
			send_str(bafer);
			
	
			
			
			send_str("\n");
		}
		
		
		
    }
	
	return 0;
}



