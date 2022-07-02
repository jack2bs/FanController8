/*
 * Timer.h
 *
 * Created: 1/29/2021 7:52:55 PM
 *  Author: Jack2bs
 */ 


#ifndef TIMER_H_
#define TIMER_H_

#define ALL_PWM

#include "Main.h"
#include <avr/wdt.h>

volatile int g_oneSecond;
volatile int16_t g_countoTo15656;

static volatile uint16_t g_firstTime;
static volatile uint16_t g_secondTime;
volatile uint16_t g_thirdTime;

int handleTach(void);
void prepareForTach(void);

void analogCompInit(void);
void pwmInit(void);


#endif /* TIMER_H_ */