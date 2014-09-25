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
#include "leds.h"



int main(void)
{
	leds_init();
			
    while(1)
    {
		leds_set_g_level(LEDS_MAX_LEVEL);
		_delay_ms(5000);
		leds_set_g_level(LEDS_OFF_LEVEL);
		leds_set_r_level(LEDS_MAX_LEVEL);
		_delay_ms(5000);
		leds_set_r_level(LEDS_OFF_LEVEL);
		leds_set_b_level(LEDS_MAX_LEVEL);		
		_delay_ms(5000);
		leds_set_b_level(LEDS_OFF_LEVEL);
		_delay_ms(5000);
		
				leds_set_g_level(LEDS_NIGHT_LEVEL);
				_delay_ms(5000);
				leds_set_g_level(LEDS_OFF_LEVEL);
				leds_set_r_level(LEDS_NIGHT_LEVEL);
				_delay_ms(5000);
				leds_set_r_level(LEDS_OFF_LEVEL);
				leds_set_b_level(LEDS_NIGHT_LEVEL);
				_delay_ms(5000);
				leds_set_b_level(LEDS_OFF_LEVEL);
				_delay_ms(5000);
		
    }
}