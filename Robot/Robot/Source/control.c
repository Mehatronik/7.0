/*
 * control.c
 *
 * Created: 14.12.2016 9:01:37
 *  Author: Mirko
 */ 

#include "control.h"



//sense variables
volatile unsigned int last_motor_count_R;
volatile unsigned int motor_count_R;
volatile unsigned int last_motor_count_L;
volatile unsigned int motor_count_L;
volatile signed int motor_angular_velocity_R;
volatile signed long motor_angle_R;
volatile signed int motor_angular_velocity_L;
volatile signed long motor_angle_L;

//velocity control variables
volatile float ref_mav_R=0,des_mav_R=0,des_maa_R=0,err_mav_R=0,int_err_mav_R=0, derr_mav_R=0, prev_err_mav_R=0;
volatile float sat_mav_R=0,pos_mav_R=0, max_mav_R = 0;
volatile long err_ma_R, des_ma_R;
volatile int DuC_R=0;
volatile float Kp_R=0;
volatile float Ki_R=0;
volatile float Kd_R=0;

volatile float ref_mav_L=0,des_mav_L=0,des_maa_L=0,err_mav_L=0,int_err_mav_L=0, derr_mav_L=0, prev_err_mav_L=0;
volatile float sat_mav_L=0,pos_mav_L=0, max_mav_L = 0;
volatile long err_ma_L, des_ma_L;
volatile int DuC_L=0;
volatile float Kp_L=0;
volatile float Ki_L=0;
volatile float Kd_L=0;

//position control variables
volatile int phase_rpc;//robot position control phase 1/2/3/4 variable 
volatile float Kp_or_R, Kp_or_L,KI_or_R, KI_or_L;//Gains
volatile float Kp_pos_R, Kp_pos_L,KI_pos_R, KI_pos_L;
volatile float Yd_mm,Xd_mm,fid_rad, fid_deg;//Desired position and orientation
volatile float xe,ye,fi_e,fi_Ie,d_e,d_Ie;//Error signals

//odometry variables
volatile long fi_inc=0;
volatile float fi_rad=0;
volatile float fi_deg=0;
volatile long X_inc=0;
volatile float X_mm=0;
volatile long Y_inc=0;
volatile float Y_mm=0;
volatile float C_inc_2_rad = ObimOdometrijskogTocka / (RastojanjeMedjuOdometrijskih * RezolucijaEnkodera );		//0.00018492;   //0.0001267;		// 1/C_rad_2_inc,  C_inc_2_rad = ObimOdometrijskog / (RastojanjeMedjuOdometrijskim * RezolucijaEnkodera)
volatile float C_inc_2_mm = ObimOdometrijskogTocka / RezolucijaEnkodera;		//0.021082;		//0.0283786;		// C_inc_2_mm = obim / RezolucijaEnkodera
volatile float C_rad_2_inc = (RastojanjeMedjuOdometrijskih * RezolucijaEnkodera ) / ObimOdometrijskogTocka;		//5407.57;		//7893;
volatile float C_mm_2_inc = RezolucijaEnkodera / ObimOdometrijskogTocka;		//1.0/0.021082;

//precnik odometrijskog 55mm >> obim 172.783
//rastojanje izmedju odometrijskih 121(krajnje), 114 srednje

void set_robot_control_gains()
{
	sat_mav_R = 30;			//30, saturacija? max_mav < sat_mav;   max_mav je dole na dnu
	sat_mav_L = 30;		//30
	
	//velocity control gains
	
	/**** koriguj dejstva spram modifikovanog PWM-a koji sad ide do 4000, umesto 800 ****/
	Kp_R = 50;  //10
	Ki_R = 0.2;  //0.029; 
	Kd_R = 0;

	Kp_L = 50; //10
	Ki_L = 0.2;  //0.025; 
	Kd_L = 0;
	
	//position control gains
	Kp_or_R = 300;
	KI_or_R = 0.2;

	Kp_or_L = -300;
	KI_or_L = -0.2;

	Kp_pos_R = 0;
	KI_pos_R=0;

	Kp_pos_L=0;
	KI_pos_L=0;
	
	max_mav_R = 30;
	max_mav_L = 30;  //30inc / 1ms
	
	des_maa_R = 0.05; // 1inc / 1ms/1ms		//zeljeno ubrzanje
	des_maa_L = 0.05;
}

void sens_robot()
{
	float d;
	motor_count_R = TCD0.CNT;
	
	//desna brzina
	if(MOTOR_QDEC_OWF_R == 0)
											//motor_sample_R16 = (motor_count_R - last_motor_count_R);
											//motor_angular_velocity_R = (motor_count_R - last_motor_count_R);
	motor_angular_velocity_R =	(signed int)last_motor_count_R - (signed int)motor_count_R;
	else
	{
		if(MOTOR_QDEC_OWF_R == 1)//ovf
		{
			MOTOR_QDEC_OWF_R = 0;
											//motor_sample_R16 = motor_count_R + ~(last_motor_count_R);

											//motor_count_R = 65000 //last_motor_count = 1000
											//-(1000 + 65535 - 65000)
			motor_angular_velocity_R = - (last_motor_count_R + (65535 - motor_count_R));
		}
		if(MOTOR_QDEC_OWF_R == 2)//uvf
		{
			MOTOR_QDEC_OWF_R = 0;
											//motor_sample_R16 = -(motor_count_R + ~(last_motor_count_R));
			
											//motor_count_R = 1000 //last_motor_count = 65000
											// (1000 + (65535 - 65000)
			motor_angular_velocity_R = (motor_count_R + (65535 - last_motor_count_R));
		}
	}
	
	motor_angle_R += motor_angular_velocity_R;
	//osvezavanje last_ promenjive
	last_motor_count_R = motor_count_R;
	
	motor_count_L = TCD1.CNT;
	//leva brzina
	if(MOTOR_QDEC_OWF_L == 0)
											//motor_sample_R16 = (motor_count_R - last_motor_count_R);
	motor_angular_velocity_L =	(signed int)last_motor_count_L - (signed int)motor_count_L;
											//motor_angular_velocity_L = (motor_count_L - last_motor_count_L);
	else
	{
		if(MOTOR_QDEC_OWF_L == 1)//ovf
		{
			MOTOR_QDEC_OWF_L = 0;
											//motor_sample_R16 = motor_count_R + ~(last_motor_count_R);

											//motor_count_R = 65000 //last_motor_count = 1000
											//-(1000 + 65535 - 65000)
			motor_angular_velocity_L = - (last_motor_count_L + (65535 - motor_count_L));
		}
		if(MOTOR_QDEC_OWF_L == 2)//uvf
		{
			MOTOR_QDEC_OWF_L = 0;
											//motor_sample_R16 = -(motor_count_R + ~(last_motor_count_R));
			
											//motor_count_R = 1000 //last_motor_count = 65000
											// (1000 + (65535 - 65000)
			motor_angular_velocity_L = (motor_count_L + (65535 - last_motor_count_L));
		}
	}
	motor_angle_L += motor_angular_velocity_L;
	//osvezavanje last_ promenjive
	last_motor_count_L = motor_count_L;
	
	
	//rastojanje izmedju tockova 224mm
	//precnik tocka 74mm
	
	
	
	//KOCE KONTROLER OVI COSINUSI I SINUSI ko sam crn djavo. Funkcije sin i cos su iz math.h biblioteke, znaci vrv preko Tejlorovih redova se racunaju,
	//sto ubija kontroler. Bolje bi bilo napraviti look-up tabelu za sin i cos.
	//f cpu povecana na 32Mhz pa nema problema sa usporavanjem vise

	//fi_inc += motor_angular_velocity_R - motor_angular_velocity_L;
	fi_inc = -(int)(motor_count_R - motor_count_L);
	fi_rad = C_inc_2_rad * fi_inc;
	fi_deg = 180*fi_rad/M_PI;
	
	//d = (motor_angular_velocity_R + motor_angular_velocity_L)/2;
	d = -(int)(motor_count_R + motor_count_L) / 2;
	//X_inc += d * cos(fi_rad);
	//Y_inc += d * sin(fi_rad);
	X_inc = d * cos(fi_rad);
	Y_inc = d * sin(fi_rad);
	
	X_mm = C_inc_2_mm * X_inc;
	Y_mm = C_inc_2_mm * Y_inc;
	
	//****problem: kada se jako sporo pomera, uopste ne uvecava x. Dok ugao fino uvecava. A daaaa. Skontao sam. Za racunanje X-a koristi d, koje racuna na 
	//osnovu  brzine (motor_angular_velocity_R) pa mu stigne vrlo mali broj impulsa da bi skontao....
	//***resenje: prebaciti u direktno merenje inkremenata, umesto ovih gluposti
	
}

void robot_direct_drive(){
	if (DuC_R > 0) //direction 1
	{
		PORT_ClearPins(&PORTH, 0b00001000);	//IN_B1=0 //dir1
		PORT_SetPins(&PORTH, 0b00000001);	//IN_A1=1 //dir1
		TCF1.CCABUF = DuC_R;//PWM duty cicle
	}
	else if (DuC_R < 0)	//direction 2
	{
		PORT_ClearPins(&PORTH, 0b00000001);	//IN_A1=0 //dir2
		PORT_SetPins(&PORTH, 0b00001000);	//IN_B1=1 //dir2
		TCF1.CCABUF = - DuC_R;
	}
	else //break
	PORT_ClearPins(&PORTH, 0b00001001);	//IN_A1=0, IN_B1=0
	
	//left wheel
	if (DuC_L < 0)	//direction 1
	{
		PORT_ClearPins(&PORTH, 0b00010000);	//IN_A2=0
		PORT_SetPins(&PORTH, 0b10000000);	//IN_B2=1
		TCF1.CCBBUF = -DuC_L;
	}
	else if (DuC_L > 0)	//direction 2
	{
		PORT_ClearPins(&PORTH, 0b10000000);	//IN_B2=0
		PORT_SetPins(&PORTH, 0b00010000);	//IN_A2=1,
		TCF1.CCBBUF =  DuC_L;
	}
	else	//break
	PORT_ClearPins(&PORTH, 0b10010000);	//IN_A2=0, IN_B2=0
}


float saturacija(float a, float max)
{
	if (a>max)
		return max;
	else if (a < -max)
		return -max;
	else
		return a;	
}

float trapez(float v, float v_des, float a)
{
	if (v < v_des - a)
		return v+a;
	else if (v > v_des + a)
		return v-a;
	else
		return v_des;
}

void robot_velocity_control()
{
	//input to the control function
	//des_mav_R - desired motor angular velocity for right wheel
	//des_mav_L - desired motor angular velocity for left wheel
	//des_maa_R (>0) - acceleration/deceleration for trapezoidal velocity profile for R wheel
	//des_maa_L (>0) - acceleration/deceleration for trapezoidal velocity profile for R wheel 
	//max_mav_R (>0 <sat_mav_R) - desired cruising motor angular velocity for right wheel
	//max_mav_L (>0 <sat_mav_L) - desired cruising motor angular velocity for left wheel
	
	
	
	des_mav_R = saturacija(des_mav_R, max_mav_R);
	ref_mav_R = trapez(ref_mav_R, des_mav_R, des_maa_R);	
	err_mav_R = ref_mav_R - motor_angular_velocity_R;
	int_err_mav_R += err_mav_R;
	derr_mav_R = err_mav_R - prev_err_mav_R;
	prev_err_mav_R = err_mav_R;
	//int_err_mav_R = saturacija(int_err_mav_R, 2000*sat_mav_R);	//seljacki pokusaj zastite od wind up-a
	
	
	DuC_R = (int)(Kp_R*err_mav_R + Kd_R * derr_mav_R + Ki_R*int_err_mav_R);// + 7*( fi_e + 0.5 * fi_Ie ); //0.01 * fi_Ie
	
	DuC_R = saturacija(DuC_R, MAX_DuC);
	if(DuC_R == MAX_DuC)
		int_err_mav_R -= err_mav_R;		//moj pokusaj anti wind up-a, za sad radi savrseno.
										//Opis: ako je vec maksimalno upravljanje(Max Duc Pwm 4000 = preopterecenje), prestani sa uvecavanjem integralne greske 
	
	
	
	des_mav_L = saturacija(des_mav_L, max_mav_L);		
	ref_mav_L = trapez(ref_mav_L, des_mav_L, des_maa_L);	
	err_mav_L = ref_mav_L - motor_angular_velocity_L;
	int_err_mav_L += err_mav_L;
	derr_mav_L = err_mav_L - prev_err_mav_L;
	prev_err_mav_L = err_mav_L;	
	//int_err_mav_L = saturacija(int_err_mav_L, 2000*sat_mav_L); //seljacki pokusaj zastite od wind up-a	
		
	DuC_L = (int)(Kp_L*err_mav_L + Kd_L * derr_mav_L + Ki_L*int_err_mav_L);// - 7*( fi_e + 0.5 * fi_Ie );		//prvoeri znakove za peglanje ugla; prvereno, u redu su
	if(DuC_L == MAX_DuC)
		int_err_mav_L -= err_mav_L;		//moj pokusaj anti wind up-a, za sad radi savrseno.
										//Opis: ako je vec maksimalno upravljanje(Max Duc Pwm 4000 = preopterecenje), prestani sa uvecavanjem integralne greske
	
	if (DuC_R > 0) //direction 1
	{
		PORT_ClearPins(&PORTH, 0b00001000);	//IN_B1=0 //dir1
		PORT_SetPins(&PORTH, 0b00000001);	//IN_A1=1 //dir1
		TCF1.CCABUF = DuC_R;//PWM duty cicle
	}
	else if (DuC_R < 0)	//direction 2
	{
		PORT_ClearPins(&PORTH, 0b00000001);	//IN_A1=0 //dir2
		PORT_SetPins(&PORTH, 0b00001000);	//IN_B1=1 //dir2
		TCF1.CCABUF = - DuC_R;
	}
	else //break
	PORT_ClearPins(&PORTH, 0b00001001);	//IN_A1=0, IN_B1=0
	
	//left wheel
	if (DuC_L < 0)	//direction 1
	{
		PORT_ClearPins(&PORTH, 0b00010000);	//IN_A2=0
		PORT_SetPins(&PORTH, 0b10000000);	//IN_B2=1
		TCF1.CCBBUF = -DuC_L;
	}
	else if (DuC_L > 0)	//direction 2
	{
		PORT_ClearPins(&PORTH, 0b10000000);	//IN_B2=0
		PORT_SetPins(&PORTH, 0b00010000);	//IN_A2=1,
		TCF1.CCBBUF =  DuC_L;
	}
	else	//break
	PORT_ClearPins(&PORTH, 0b10010000);	//IN_A2=0, IN_B2=0
	
	
}
	
void robot_position_control()
{
	//moja funkcija
	
	xe = Xd_mm - X_mm;  //zeljeno - mereno
	
	
	
	fi_e = fid_deg - fi_deg;	//zadato - mereno; greska u uglu
	
	fi_Ie += fi_e;		//integralna greska ugla
	fi_Ie = saturacija(fi_Ie, 45);  //recimo ovako da ogranicim integralnu gresku na 45 stepeni
	
	//za sada samo poziciju da regulise
	//max 30-ak
	//pozicioni regulator zadaje brzinu. Brzinski zadaje Duc
	des_mav_L = (0.05*xe) - (0.2 * fi_e);
	des_mav_R = (0.05*xe) + 0.2 * fi_e;
	
	
	robot_velocity_control();	
	
}
		
	
	
