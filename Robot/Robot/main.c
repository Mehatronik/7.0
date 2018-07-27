/*
 * primer2.c
 *
 * Created: 10.1.2018 9:50:43
 *  Author: Tasevski
 */ 

#include "common.h"
#include "timer.h"
#include "uart_comm.h"
#include "qdec.h"
#include "control.h"


void enable_interrupts()
{
	//XMega128A
	PMIC.CTRL |= PMIC_HILVLEN_bm;	// Omoguci HIGH level prekida
	PMIC.CTRL |= PMIC_MEDLVLEN_bm;	// Omoguci MEDIUM level prekida
	PMIC.CTRL |= PMIC_LOLVLEN_bm;	// Omoguci LOW level prekida
	PMIC.CTRL |= PMIC_RREN_bm;		// Pokreni Round Robin algoritam
}

int main(void)
{
	char message[30];
	unsigned sekunde = 0;
	init_timer();
	init_uart();
	init_qdec();
	init_PWM();
	init_FSM();
	set_robot_control_gains();
	enable_interrupts();
	
	PORT_SetPinsAsInput(&PORTB, 0xFF);
	PORT_ConfigurePins(&PORTB, 0xFF, false, false,  PORT_OPC_PULLDOWN_gc, PORT_ISC_RISING_gc);
													//totem pull down zbog SICK senzora; kod milutina je bilo pull up zbog nagaznih senzora-tastera
													//rastuca ivica  PORT_OPC_PULLDOWN_gc
													
	PORT_SetPinsAsOutput(&PORTC, 0xFF);	//za proveru okidanja tajmera-1ms na osciloskopu
													
													
	sei();

    while(1)
    {
	   add_event(EVENT_CINC);		//zeznem ga
       cycle_FSM();		//
	   
	   
	  
	   
	   if(flag_1s == 1)
	   {
		   flag_1s = 0;			
		   //sekunde++;
		   

		   
		   /*
		   send_str("des_mav_L: ");
		   itoa(des_mav_L, message, 10);
		   send_str(message);
		   send_str("    ");
		   
		
		   send_str("mavL: ");
		   itoa(motor_angular_velocity_L, message, 10);
		   send_str(message);
		   send_str("    ");
		   
		   send_str("DuC_L: ");
		   itoa(DuC_L/4, message, 10);	// /4 da bi bilo max 200, zbog prikaza na ploteru
		   send_str(message);
		   send_str("    ");
		   
			send_str("des_mav_R: ");
			itoa(des_mav_R, message, 10);
			send_str(message);
			send_str("    ");
			
			
			send_str("mavR: ");
			itoa(motor_angular_velocity_R, message, 10);
			send_str(message);
			send_str("    ");
		   
		   
		  
		      
		   send_str("DuC_R: ");
		   itoa(DuC_R/4, message, 10);	// /4 da bi bilo max 200, zbog prikaza na ploteru
		   send_str(message);
		   send_str("    ");
		   */
		   /*
		   send_str("int_err_mav_L:");
		   itoa(int_err_mav_L, message, 10);
		   send_str(message);
		   send_str("  ");
		   
		   
		   	send_str("fi_deg:");
		   	itoa(fi_deg, message, 10);
		   	send_str(message);
		   	send_str("  ");
		   
		   send_str("fi_eror_deg:");
		   itoa(fi_e, message, 10);
		   send_str(message);
		   send_str("    ");
		   
		    send_str("fi_inc: ");
		    itoa(fi_inc, message, 10);
		    send_str(message);
		    send_str("    ");
	
	
		   send_str("mot_L:");
		   itoa(motor_count_L, message, 10);
		   send_str(message);
		   send_str("  ");
		   
		   send_str("mot_R: ");
		   itoa(motor_count_R, message, 10);
		   send_str(message);
		   send_str("\n");
		   */
		   
		   send_str("Xd_mm: ");
		   itoa(Xd_mm, message, 10);
		   send_str(message);
		   send_str("     ");
		   
		   
		   send_str("X_mm:");
		   itoa(X_mm, message, 10);
		   send_str(message);
		   send_str("     ");
		   
		   send_str("xe: ");
		   itoa(xe, message, 10);
		   send_str(message);
		   send_str("    ");
		  
		   
		   
		   send_str("fi_deg: ");
		   itoa(fi_deg, message, 10);
		   send_str(message);
		   send_str("    ");
		   
		   send_str("fi_inc: ");
		   utoa(fi_inc, message, 10);
		   send_str(message);
		   send_str("    ");
		   
		   send_str("motor_count_L: ");
		   utoa(motor_count_L, message, 10);	//utoa
		   send_str(message);
		   send_str("    ");
		   
		   
		   send_str("motor_count_R: ");
		   utoa(motor_count_R, message, 10);	//utoa
		   send_str(message);
		   send_str("\n");
		   
		   
	   } 
	   
	   
    }
}