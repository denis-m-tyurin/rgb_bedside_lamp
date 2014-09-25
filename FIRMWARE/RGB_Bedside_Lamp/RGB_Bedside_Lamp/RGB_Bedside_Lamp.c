/*
 * RGB_Bedside_Lamp.c
 *
 * Created: 24.09.2014 22:10:39
 *  Author: denis
 */ 
#ifndef F_CPU
#define F_CPU 8000000UL // 8 MHz
#endif

#include <avr/io.h>
#include <util/delay.h>



int main(void)
{
	// Configure pins as outputs
	DDRD = (1<<PD5) | (1<<PD6);
	DDRB = (1<<PB3);
	
	PORTB = 0;
	PORTD = 0;
			
    while(1)
    {
		PORTB |= (1 << PB3);
		_delay_ms(5000);
		PORTB &= ~(1 << PB3);		
		PORTD |= (1 << PD5);
		_delay_ms(5000);
		PORTD &= ~(1 << PD5);
		PORTD |= (1 << PD6);
		_delay_ms(5000);
		PORTD &= ~(1 << PD6);		
    }
}