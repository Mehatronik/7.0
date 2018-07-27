/*
 * uart.h
 *
 * Created: 1.12.2017. 23.00.49
 *  Author: Ko''
 */ 


#ifndef UART_H_
#define UART_H_

#include <string.h>


#define INPUT_BUFF_SIZE 8

extern volatile unsigned char poslato;
extern volatile unsigned char input_char;
extern volatile unsigned char input_buffer[INPUT_BUFF_SIZE], buff_size;

extern void uart_init();
extern void send_str(unsigned char *str);



#endif /* UART_H_ */