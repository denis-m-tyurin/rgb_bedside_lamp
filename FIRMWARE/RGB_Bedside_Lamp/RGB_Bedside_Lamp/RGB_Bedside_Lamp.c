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
#include <stdlib.h>
#include <util/delay.h>
#include <avr/interrupt.h>

#define PWM_TOP 1000
#define HUE_TOP 1536
#define PWM_NIGHT_MODE_REPEAT_CYCLES 100
#define PWM_TRANSITION_MODE_REPEAT_CYCLES 1

#define RED_LED_ON PORTD |= (1 << PD6)
#define RED_LED_OFF PORTD &= ~(1 << PD6)

#define BLUE_LED_ON PORTD |= (1 << PD5)
#define BLUE_LED_OFF PORTD &= ~(1 << PD5)

#define GREEN_LED_ON PORTB |= (1 << PB3)
#define GREEN_LED_OFF PORTB &= ~(1 << PB3)

struct RGB
{
	uint16_t red;
	uint16_t green;
	uint16_t blue;
};

static uint8_t mode_switch = 0;

struct RGB HSV2RGB_Adv1 (uint16_t hue, unsigned char sat, unsigned char val);

int main(void)
{
	struct RGB color;
	uint16_t hue;
	uint16_t pwm_counter;
	
	uint16_t pwm_cycles;

	/* BLUE: PD5
	   RED: PD6
	   GREEN: PB3
   
	   Note that these PINs are connected to output units of Timer0/Timer2
       therefore we could use hardware PWM mode. But since it has low resolution
       we would use software generated PWM. */
	DDRD |= (1<<PD5) | (1<<PD6);
	DDRB |= (1<<PB3);
		
	RED_LED_OFF;
	GREEN_LED_OFF;
	BLUE_LED_OFF;

	// Configure interrupts for buttons	
	// PD3 (INT1) - Power toggle button
	// PD2 (INT0) - Mode switch 
	PORTD |= (1 << PD3) | (1 << PD2); // Enable internal pull-ups for these PINs
	EICRA |= (1 << ISC01); // INT1 - low level generates INT, INT0 - falling edge generates INT
	EIMSK |= (1 << INT1) | (1 << INT0);

	
	// Enable interrupts
	sei();
			
    while(1)
    {
		mode_switch = 0;
		
		// 1st mode - night lamp		
		{
			if (1 == mode_switch) break;
			
			for (hue = 0; hue < HUE_TOP; hue++)
			{
				if (1 == mode_switch) break;
				
				color = HSV2RGB_Adv1(hue, 255, 255);
				for (pwm_cycles = 0; pwm_cycles < PWM_NIGHT_MODE_REPEAT_CYCLES; pwm_cycles++)
				{
					RED_LED_ON;
					GREEN_LED_ON;
					BLUE_LED_ON;
		
					for (pwm_counter = 0; pwm_counter<PWM_TOP; pwm_counter++)
					{
						if (pwm_counter == color.red)	RED_LED_OFF;
						if (pwm_counter == color.green)	GREEN_LED_OFF;
						if (pwm_counter == color.blue)	BLUE_LED_OFF;
					}
				}
			}
		} while (mode_switch != 1);
		
		mode_switch = 0;
	#if 0	
		// This should never happen, but make the level compliant to PWM TOP
		if (color.red > PWM_TOP) color.red = PWM_TOP;
		if (color.green > PWM_TOP) color.green = PWM_TOP;
		if (color.blue > PWM_TOP) color.blue = PWM_TOP;
		
		// Increase LEDs level
		{
			for (pwm_cycles = 0; pwm_cycles < PWM_TRANSITION_MODE_REPEAT_CYCLES; pwm_cycles++)
			{
				RED_LED_ON;
				GREEN_LED_ON;
				BLUE_LED_ON;
				
				for (pwm_counter = 0; pwm_counter<PWM_TOP; pwm_counter++)
				{
					if (pwm_counter == color.red)	RED_LED_OFF;
					if (pwm_counter == color.green)	GREEN_LED_OFF;
					if (pwm_counter == color.blue)	BLUE_LED_OFF;
				}
			}
			
			if (color.red < PWM_TOP) color.red++;
			if (color.green < PWM_TOP) color.green++;
			if (color.blue < PWM_TOP) color.blue++;
						
		} while (color.red < PWM_TOP || color.green < PWM_TOP || color.blue < PWM_TOP);		
#endif		
		// 2nd mode - regular white lamp
		RED_LED_ON;
		GREEN_LED_ON;
		BLUE_LED_ON;
		{
			_delay_ms(5);
		} while (1 != mode_switch);
	}
}


struct RGB HSV2RGB_Adv1 (uint16_t hue, unsigned char sat, unsigned char val)
{
	struct RGB out;
	if (!sat) { // Acromatic color (gray). Hue doesn't mind.
		out.red=out.green=out.blue=val;
		} else  {

		
		//hue %= 1536;
		uint8_t base = ((255 - sat) * val)>>8;
		uint8_t iiii= hue/256;
		switch(iiii) {
			case 0:
			out.red = val;
			out.green = (((val-base)*hue)/256)+base;
			out.blue = base;
			break;

			case 1:
			out.red = (((val-base)*(256-(hue%256)))/256)+base;
			out.green = val;
			out.blue = base;
			break;

			case 2:
			out.red = base;
			out.green = val;
			out.blue = (((val-base)*(hue%256))/256)+base;
			break;

			case 3:
			out.red = base;
			out.green = (((val-base)*(256-(hue%256)))/256)+base;
			out.blue = val;
			break;

			case 4:
			out.red = (((val-base)*(hue%256))/256)+base;
			out.green = base;
			out.blue = val;
			break;

			case 5:
			out.red = val;
			out.green = base;
			out.blue = (((val-base)*(256-(hue%256)))/256)+base;
			break;
		}
	}
	
	return out;
} // HSV 2 RGB Binary

ISR(INT0_vect)
{
	mode_switch = 1;
}

ISR(INT1_vect)
{
	
}
