/*
 * common.h
 *
 * Created: 7.12.2016 23:48:12
 *  Author: Tasevski
 */ 


#ifndef COMMON_H_
#define COMMON_H_

#include <avr/io.h>
#include <avr/interrupt.h>
#include <string.h>
#include <math.h>


#include "avr_compiler.h"
#include "port_driver.h"
#include "timer.h"
#include "uart_comm.h"
#include "qdec.h"
#include "control.h"
#include "h_bridge.h"
#include "fsm.h"

#define SBIT(PORTx,PINx) PORTx->OUT|=(1<<PINx)
#define RBIT(PORTx,PINx) PORTx->OUT&=~(1<<PINx)
#define TBIT(PORTx,PINx) PORTx->OUT^=(1<<PINx)
#define ReadBIT(PORTx,PINnum) PORTx->IN&(1<<PINnum)

#define TRUE	1
#define FALSE	!TRUE
#define HIGH	1
#define LOW		!HIGH


#define MAX_DuC 4000.0
#define ObimOdometrijskogTocka 174 //172.783-izracunato, izmereno eksperimentalno 174mm
#define RastojanjeMedjuOdometrijskih 113.0	
#define RezolucijaEnkodera 8192.0

//precnik odometrijskog 55mm >> obim 172.783
//rastojanje izmedju odometrijskih 121(krajnje), 114 srednje

enum IO_pin_enum {
	IO_pin_0 =	(1 << 0),
	IO_pin_1 =	(1 << 1),
	IO_pin_2 =	(1 << 2),
	IO_pin_3 =	(1 << 3),
	IO_pin_4 =	(1 << 4),
	IO_pin_5 =	(1 << 5),
	IO_pin_6 =	(1 << 6),
	IO_pin_7 =	(1 << 7)
};

//void io_init_input(PORT_t *Port, enum IO_pin_enum Pins);
//void io_init_output(PORT_t *Port, enum IO_pin_enum Pins);
//void io_set_pins(PORT_t *Port, enum IO_pin_enum Pins);
//void io_clear_pins(PORT_t *Port, enum IO_pin_enum Pins);
//void io_toggle_pin(PORT_t *Port, enum IO_pin_enum Pin);
//int  io_read_pins(PORT_t *Port, enum IO_pin_enum Pin);


void enable_interrupts();



#endif /* COMMON_H_ */