/*
 * EEPROM.h
 *
 * Created: 1/12/2021 11:24:10 AM
 *  Author: Jack2bs
 */ 


#ifndef EEPROM_H_
#define EEPROM_H_

#include <avr/eeprom.h>
#include "Main.h"

void saveProfile();
void useProfileFromEEprom();


#endif /* EEPROM_H_ */