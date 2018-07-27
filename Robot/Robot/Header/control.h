/*
 * control.h
 *
 * Created: 14.12.2016 9:01:23
 *  Author: Mirko
 */ 


#ifndef CONTROL_H_
#define CONTROL_H_

#include "common.h"


//void sens_motor_angular_velocity();
//void PID();
void set_robot_control_gains();
void sens_robot();
void robot_direct_drive();
void robot_velocity_control();
void robot_position_control();

//sense variables
extern volatile unsigned int last_motor_count_R;
extern volatile unsigned int motor_count_R;
extern volatile unsigned int last_motor_count_L;
extern volatile unsigned int motor_count_L;
extern volatile signed int motor_angular_velocity_R;
extern volatile signed long motor_angle_R;
extern volatile signed int motor_angular_velocity_L;
extern volatile signed long motor_angle_L;

//velocity control variables
extern volatile float ref_mav_R,des_mav_R,des_maa_R,err_mav_R,int_err_mav_R,  derr_mav_R, prev_err_mav_R;
extern volatile float sat_mav_R,pos_mav_R,max_mav_R;
extern volatile long err_ma_R, des_ma_R;
extern volatile int DuC_R;
extern volatile float Kp_R;
extern volatile float Ki_R;
extern volatile float Kd_R;

extern volatile float ref_mav_L,des_mav_L,des_maa_L,err_mav_L,int_err_mav_L,  derr_mav_L, prev_err_mav_L;
extern volatile float sat_mav_L,pos_mav_L,max_mav_L;
extern volatile long err_ma_L, des_ma_L;
extern volatile int DuC_L;
extern volatile float Kp_L;
extern volatile float Ki_L;
extern volatile float Kd_L;

//position control variables
extern volatile int phase_rpc;//robot position control phase 1/2/3/4 variable
extern volatile float Kp_or_R, Kp_or_L,KI_or_R, KI_or_L;//Gains
extern volatile float Kp_pos_R, Kp_pos_L,KI_pos_R, KI_pos_L;
extern volatile float Yd_mm,Xd_mm,fid_rad,fid_deg;//Desired position and orientation
extern volatile float xe,ye,fi_e,fi_Ie,d_e,d_Ie;//Error signals


//odometry variables
extern volatile long fi_inc;
extern volatile float fi_rad;
extern volatile float fi_deg;
extern volatile long X_inc;
extern volatile float X_mm;
extern volatile long Y_inc;
extern volatile float Y_mm;
extern volatile float C_inc_2_rad;//1/C_rad_2_inc
extern volatile float C_inc_2_mm;
extern volatile float C_rad_2_inc;
extern volatile float C_mm_2_inc;

#endif /* CONTROL_H_ */