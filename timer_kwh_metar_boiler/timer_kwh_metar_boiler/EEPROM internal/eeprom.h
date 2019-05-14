/*
 * eeprom.h
 *
 * Created: 14.5.2019. 08.12.01
 *  Author: Kovacevic
 */ 


#ifndef EEPROM_H_
#define EEPROM_H_

#include "comm.h"

#define UPALI_HR_ADR   0
#define UPALI_MIN_ADR  1
#define UGASI_HR_ADR   2
#define UGASI_MIN_ADR  3

extern uint8_t EEPROM_read(uint16_t uiAddress);
extern void EEPROM_write(uint16_t uiAddress, uint8_t ucData);


#endif /* EEPROM_H_ */