/*
 * Main.h
 *
 * Created: 1/29/2021 4:35:55 PM
 *  Author: Jack2bs
 */ 


#ifndef MAIN_H_
#define MAIN_H_

#define f_cpu 16000000ul

#include <avr/io.h>
#include <avr/interrupt.h>
#include "util/delay.h"
#include "jtUSB.h"
#include "Descriptors.h"
#include "Timer.h"
#include "Ticket.h"
#include "EEPROM.h"
#include "TWI.h"
#include <util/atomic.h>

volatile int g_fanTypes[6];
volatile uint16_t g_fanSpeeds[6];

volatile int8_t g_fanPercentages[6];

int g_temp1;
int g_temp2;
int g_cpuTemp;

uint8_t g_timer3FanMask;
uint8_t g_timer1FanMask;

volatile uint8_t g_resetFlags;

volatile uint8_t g_currentFanProfile[6][21];

int incrementAdmux(void);
void getFanPercent(void);

#endif /* MAIN_H_ */