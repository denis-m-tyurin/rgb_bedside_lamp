#include <avr/io.h>
#include "leds.h"


/* BLUE: PD5 - OC0B
   RED: PD6 - OC0A
   GREEN: PB3 - OC2A */

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

void leds_set_rgb(uint8_t r, uint8_t g, uint8_t b)
{
	// Set BLUE
	if (LEDS_OFF_LEVEL == b)
	{
		//	disable output for this pin
		TCCR0A &= ~(1 << COM0B1);		
	}
	else
	{
		OCR0B = b;
		
		// Enable this pin if necessary
		if (0 == (TCCR0A & (1 << COM0B1)))
		{
			TCCR0A |= (1 << COM0B1);		
		}
	}
	
	// Set RED
	if (LEDS_OFF_LEVEL == r)
	{
		//	disable output for this pin
		TCCR0A &= ~(1 << COM0A1);
	}
	else
	{
		OCR0A = r;
		
		// Enable this pin if necessary
		if (0 == (TCCR0A & (1 << COM0A1)))
		{
			TCCR0A |= (1 << COM0A1);
		}
	}
	
	// Set GREEN
	if (LEDS_OFF_LEVEL == g)
	{
		//	disable output for this pin
		TCCR2A &= ~(1 << COM2A1);
	}
	else
	{
		OCR2A = g;
		
		// Enable this pin if necessary
		if (0 == (TCCR2A & (1 << COM2A1)))
		{
			TCCR2A |= (1 << COM2A1);
		}
	}
}

