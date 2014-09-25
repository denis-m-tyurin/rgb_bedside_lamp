#include <avr/io.h>
#include "leds.h"


/* BLUE: PD5 - OC0B
   RED: PD6 - OC0A
   GREEN: PB3 - OC2A */

// local variables
static uint8_t r_off = 1;
static uint8_t g_off = 1;
static uint8_t b_off = 1;

void leds_init()
{
	// Configure pins
	DDRD |= (1<<PD5) | (1<<PD6);
	DDRB |= (1<<PB3);
		
	PORTB &= ~(1 << PB3);
	PORTD &= ~(1 << PD5 | 1 << PD6);
	
	/* Setup Counter0 for PD5 & PD6 (Blue and Red)
	   Use Fast PWM mode with top at 0xFF
	   No clock pre-scaler */
	TCCR0A |= (1<< WGM01) | (1 << WGM00);
	TCCR0B |= (1 << CS00);
	
	// Setup Counter2 for PB3 (Green). The same conf
	TCCR2A |= (1<< WGM21) | (1 << WGM20);
	TCCR2B |= (1 << CS20);
}

void leds_set_r_level(int level)
{
	if (LEDS_OFF_LEVEL == level)
	{
		//	disable output for this pin
		TCCR0A &= ~(1 << COM0B1);
		r_off = 1;
	} 
	else 
	{
		OCR0B = level;	
		if (1 == r_off)
		{
			TCCR0A |= (1 << COM0B1);
			r_off = 0;
		}
	}	
}

void leds_set_g_level(int level)
{
	if (LEDS_OFF_LEVEL == level)
	{
		//	disable output for this pin
		TCCR2A &= ~(1 << COM2A1);
		g_off = 1;
	}
	else
	{
		OCR2A = level;
		if (1 == g_off)
		{
			TCCR2A |= (1 << COM2A1);
			g_off = 0;
		}
	}
}

void leds_set_b_level(int level)
{
	if (LEDS_OFF_LEVEL == level)
	{
		//	disable output for this pin
		TCCR0A &= ~(1 << COM0A1);
		b_off = 1;
	}
	else
	{
		OCR0A = level;
		if (1 == b_off)
		{
			TCCR0A |= (1 << COM0A1);
			b_off = 0;
		}
	}
}
