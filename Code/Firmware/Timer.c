/*
 * Timer.c
 *
 * Created: 1/29/2021 7:52:44 PM
 *  Author: Jack2bs
 */ 

#include "Timer.h"

volatile int g_oneSecond = 0;
volatile int16_t g_countoTo15656 = 0;
volatile int g_zeroRpmTest = 0;

static volatile uint16_t g_firstTime = 0;
static volatile uint16_t g_secondTime = 0;
volatile uint16_t g_thirdTime = 0;

ISR(TIMER3_OVF_vect)
{
	PORTD |= g_timer3FanMask;
	PORTB |= g_timer3FanMask;
	
	g_countoTo15656++;
	if(g_countoTo15656 >= 15656)
	{
		g_countoTo15656 = 0;
		g_oneSecond = 1;
	}
}

ISR(TIMER1_OVF_vect)
{
	//wdt_reset();
	PORTD |= g_timer1FanMask;
	PORTB |= g_timer1FanMask;
}

ISR(TIMER1_CAPT_vect)
{
	if(!g_zeroRpmTest)
	{
		g_zeroRpmTest = 1;
	}
	else if (!g_secondTime)
	{
		g_secondTime = g_countoTo15656;
	}
	else if (!g_thirdTime)
	{
		TIMSK1 &= ~(1<<ICIE1);
		g_thirdTime = g_countoTo15656;
	}
}



#ifdef ALL_PWM


ISR(TIMER3_COMPA_vect)
{
	PORTD &= ~(1<<PORTD2);
}
ISR(TIMER3_COMPB_vect)
{
	PORTD &= ~(1<<PORTD3);
}
ISR(TIMER3_COMPC_vect)
{
	PORTD &= ~(1<<PORTD4);
}
ISR(TIMER1_COMPA_vect)
{
	PORTD &= ~(1<<PORTD5);
}
ISR(TIMER1_COMPB_vect)
{
	PORTD &= ~(1<<PORTD6);
}
ISR(TIMER1_COMPC_vect)
{
	PORTD &= ~(1<<PORTD7);
}

#endif

#ifndef ALL_PWM

ISR(TIMER3_COMPA_vect)
{
	if(g_fanTypes[0])
	{
		PORTD &= ~(1<<PORTD2);
	}
	else
	{
		PORTB &= ~(1<<PORTB2);
	}
}
ISR(TIMER3_COMPB_vect)
{
	if(g_fanTypes[1])
	{
		PORTD &= ~(1<<PORTD3);
	}
	else
	{
		PORTB &= ~(1<<PORTB3);
	}
}
ISR(TIMER3_COMPC_vect)
{
	if(g_fanTypes[2])
	{
		PORTD &= ~(1<<PORTD4);
	}
	else
	{
		PORTB &= ~(1<<PORTB4);
	}
}
ISR(TIMER1_COMPA_vect)
{
	if(g_fanTypes[3])
	{
		PORTD &= ~(1<<PORTD5);
	}
	else
	{
		PORTB &= ~(1<<PORTB5);
	}
}
ISR(TIMER1_COMPB_vect)
{
	if(g_fanTypes[4])
	{
		PORTD &= ~(1<<PORTD6);
	}
	else
	{
		PORTB &= ~(1<<PORTB6);
	}
}
ISR(TIMER1_COMPC_vect)
{
	if(g_fanTypes[5])
	{
		PORTD &= ~(1<<PORTD7);
	}
	else
	{
		PORTB &= ~(1<<PORTB7);
	}
}

#endif


int handleTach(void)
{
	uint64_t rpm = (469680/(g_thirdTime - g_secondTime));
	int8_t fan = ADMUX;
	if(fan > 1)
	{
		fan -= 2;
	}
	g_fanSpeeds[fan] = rpm;
	
	g_firstTime = 0;
	g_secondTime = 0;
	g_thirdTime = 0;
	
	return rpm;
}

void prepareForTach(void)
{
	if(!g_zeroRpmTest)
	{
		TIMSK1 &= ~(1<<ICIE1);
		int8_t fan = ADMUX;
		if(fan > 1)
		{
			fan -= 2;
		}
		g_fanSpeeds[fan] = 0;
	}
	g_zeroRpmTest = 0;
	
	incrementAdmux();
	
	g_firstTime = 0;
	g_secondTime = 0;
	g_thirdTime = 0;
	
	TIFR1 |= (1<<ICF1);
	TIMSK1 |= (1<<ICIE1);
	
}

void analogCompInit(void)
{
		DDRF = 0;
		PORTF |= 0xff;
		ADCSRA &= ~(1<<ADEN);
		ADCSRB |= (1<<ACME);
		ADMUX |= (1<<MUX0);
		
		ACSR |= (1<<ACIS1) | (1<<ACIS0) | (1<<ACIC) | (1<<ACBG);
		DIDR1 |= (1<<AIN0D);
}

void pwmInit()
{
	//Enable the waveformgeneration mode for fast pwm with 10bit top
	TCCR1A |= (1<<WGM11) | (1<<WGM10);
	TCCR3A |= (1<<WGM31) | (1<<WGM30);
	
	TCCR1B |= (1<<CS10) | (1<<WGM12);
	
	_delay_us(30);
	
	TCCR3B |= (1<<CS30) | (1<<WGM32);
	
	TIMSK1 |= (1<<OCIE1A) | (1<<OCIE1B) | (1<<OCIE1C) | (1<<TOIE1);
	TIMSK3 |= (1<<OCIE3A) | (1<<OCIE3B) | (1<<OCIE3C) | (1<<TOIE3);
	
	OCR1A = 500;
	OCR1B = 500;
	OCR1C = 500;
	
	OCR3A = 500;
	OCR3B = 500;
	OCR3C = 500;
}