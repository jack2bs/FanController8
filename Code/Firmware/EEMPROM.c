/*
 * CFile1.c
 *
 * Created: 1/12/2021 11:23:48 AM
 *  Author: Jack2bs
 */ 

#include "EEPROM.h"
/*Profile structure

uint8_t index //a number which indicates which profile it is
uint8_t number of fans
uint8_t numberOfDataPoints //Indicates the number of data points
uint8_t temperature
uint8_t fanPercentage

*/

/*

150 bytes

0-10
uint8_t profileNum
uint8_t number of fans

10-130: 20 for each of 6 fans
uint8_t fanIndex
uint8_t numOfDataPoints
uint8_t temp
uint8_t fs


*/


void saveProfile()
{	
	//int fanProfileEnd = sizeof(currentFanProfile);
	
	eeprom_update_block((void*)g_currentFanProfile, (void*)5, sizeof(g_currentFanProfile));
	return;
}

void useProfileFromEEprom()
{
	//int fanProfileEnd = sizeof(currentFanProfile);
	
	eeprom_read_block((void*)g_currentFanProfile, (void*)5, sizeof(g_currentFanProfile));
	getFanPercent();
	return;
}