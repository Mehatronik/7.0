/*
 * uart_comm.h
 *
 * Created: 17.11.2016 18:07:35
 *  Author: Tasevski
 */ 


#ifndef UART_COMM_H_
#define UART_COMM_H_

#include "common.h"

#define INPUT_BUFF_SIZE 30

extern volatile unsigned char poslato;
extern volatile unsigned char input_char;
extern volatile unsigned char input_buffer[INPUT_BUFF_SIZE],buff_size;

void init_uart();
void send_str(char *str);
void send_chr(char *str);

#endif /* UART_COMM_H_ */