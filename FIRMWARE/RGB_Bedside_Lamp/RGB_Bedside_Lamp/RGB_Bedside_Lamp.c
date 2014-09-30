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
#include <avr/eeprom.h>

#define PWM_TOP 1000
#define HUE_TOP PWM_TOP*6
#define PWM_NIGHT_MODE_REPEAT_CYCLES 50
#define PWM_TRANSITION_MODE_REPEAT_CYCLES 1
#define LEVEL_STEP 5

#define HUE_CYCLES_BACK_OFFSET 300
#define HUE_CYCLES_FORTH_OFFSET 300
#define HUE_CYCLES_BACK_FORTH_COUNT_CYCLES 10

#define RED_LED_ON PORTD |= (1 << PD6)
#define RED_LED_OFF PORTD &= ~(1 << PD6)

#define BLUE_LED_ON PORTD |= (1 << PD5)
#define BLUE_LED_OFF PORTD &= ~(1 << PD5)

#define GREEN_LED_ON PORTB |= (1 << PB3)
#define GREEN_LED_OFF PORTB &= ~(1 << PB3)

// EEPROM ADDRESS LIST
#define EEPROM_LEVEL_ADDR 2
#define EEPROM_SRAND_ADDR 4

struct RGB
{
	uint16_t red;
	uint16_t green;
	uint16_t blue;
};

static volatile uint8_t mode_switch = 0;

// Make the following vars global to reduce stack usage in nested loops
static struct RGB color = {0, 0, 0};
static uint16_t base_hue;
static uint16_t hue;
static volatile uint16_t level = PWM_TOP;
static volatile uint8_t level_changed = 0;
static uint16_t pwm_cycles;
static uint16_t pwm_counter;
static uint16_t iiii = 0;
static uint8_t hue_cycles_back_count = HUE_CYCLES_BACK_FORTH_COUNT_CYCLES;
static uint8_t hue_cycles_forth_count = HUE_CYCLES_BACK_FORTH_COUNT_CYCLES;

inline static void HSV2RGB_Adv1 (uint16_t hue, uint16_t val);
inline static void PWM_Cycle();
inline static void generate_BaseHue();
inline static void init_rand();

int main(void)
{
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
	
	// Read data from EEPROM
	level = eeprom_read_word((void *)EEPROM_LEVEL_ADDR);
	if (level > PWM_TOP) level = PWM_TOP;
	
	generate_BaseHue();
	hue = base_hue;	
	
	// Seed a pseudo rand value to Rand
	init_rand();

	// Configure interrupts for buttons	
	// PD2 (INT0) - Mode switch 
	PORTD |= (1 << PD2) | (1 << PD1) | (1 << PD0); // Enable internal pull-ups for these PINs
	EICRA |= (1 << ISC01); // INT0 - falling edge generates INT
	EIMSK |= (1 << INT0);
	
	// PD1 (PCINT17) - level down
	// PD0 (PCINT16) - level up
	PCICR |= (1 << PCIE2); // Enable interrupts for PCIE23..16 pins
	PCMSK2 |= (1 << PCINT16) | (1 << PCINT17);
	
	// Enable interrupts
	sei();
			
    while(1)
    {
		/* 1st mode - night lamp		
		 *
		 * HUE algorithm:
		 *	1. Generate random HUE value
		 *  2. Play close colors for some time
		 *	3. Generate new random HUE and transit to it
		 *		 
		 */
		for (;mode_switch != 1;)
		{
			if (1 == mode_switch) break;
			
 			generate_BaseHue();
			
			for (; hue != base_hue; )
			{
				if (1 == mode_switch) break;
				
				// This function will change the color global variable to reduce stack usage
				HSV2RGB_Adv1(hue, level);
				PWM_Cycle();
				
				if (hue < base_hue) hue ++;
				if (hue > base_hue) hue--;
			}
			
			if (1 == level_changed)
			{
				// Save new level value to EEPROM
				eeprom_write_word((void *) EEPROM_LEVEL_ADDR, level);
				level_changed = 0;
			}
		}
		
		// This should never happen, but make the level compliant to PWM TOP
		if (color.red > PWM_TOP) color.red = PWM_TOP;
		if (color.green > PWM_TOP) color.green = PWM_TOP;
		if (color.blue > PWM_TOP) color.blue = PWM_TOP;
		
		// Increase LEDs level
		for (;color.blue < PWM_TOP || color.red < PWM_TOP || color.green < PWM_TOP;)
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
		}

		mode_switch = 0;
		
		// 2nd mode - regular white lamp
		RED_LED_ON;
		GREEN_LED_ON;
		BLUE_LED_ON;
		
		for (;1 != mode_switch;)
		{
			_delay_ms(5);
		}
		
		// debouncing
		_delay_ms(1000);
		mode_switch = 0;
	}
}

inline static void init_rand()
{
	uint16_t srand_seed_value;
	
	srand_seed_value = eeprom_read_word((void*) EEPROM_SRAND_ADDR);
	srand(srand_seed_value);
	eeprom_write_word((void*)EEPROM_SRAND_ADDR, rand());
}

inline static void PWM_Cycle()
{
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

inline static void generate_BaseHue()
{
	if ((hue_cycles_back_count == HUE_CYCLES_BACK_FORTH_COUNT_CYCLES) && (hue_cycles_forth_count == HUE_CYCLES_BACK_FORTH_COUNT_CYCLES))
	{
		// Time to generate new random HUE
		base_hue = rand() % HUE_TOP;
		hue_cycles_back_count = 0;
		hue_cycles_forth_count = 0;
	}
	else
	{
		// going back and forth
		if (hue_cycles_back_count <= hue_cycles_forth_count)
		{
			// When will back at first we just need subtract the offset value. 
			// In all other cases would need to subtract both of them
			base_hue = (base_hue > HUE_CYCLES_BACK_OFFSET ? base_hue - (hue_cycles_back_count==0 ? HUE_CYCLES_BACK_OFFSET : (HUE_CYCLES_BACK_OFFSET + HUE_CYCLES_FORTH_OFFSET)) : 0);			
			hue_cycles_back_count++;
		} 
		else
		{
			// Before we come here we went back, thus now we need to add both offsets
			base_hue = base_hue + HUE_CYCLES_BACK_OFFSET + HUE_CYCLES_FORTH_OFFSET;
			if (base_hue > HUE_TOP) base_hue = HUE_TOP;
			
			hue_cycles_forth_count++;			
		}
		
	}	
}

inline static void HSV2RGB_Adv1 (uint16_t hue, uint16_t val)
{
		iiii= hue/(PWM_TOP+1);
		switch(iiii) {
			case 0:
			color.red = val;
			color.green = (uint16_t) (((uint32_t) val * (uint32_t) hue)/(PWM_TOP+1));
			color.blue = 0;
			break;

			case 1:
			color.red = (uint16_t) (((uint32_t) val*((PWM_TOP+1) - ((uint32_t) hue%(PWM_TOP+1))))/(PWM_TOP+1));
			color.green = val;
			color.blue = 0;
			break;

			case 2:
			color.red = 0;
			color.green = val;
			color.blue = (uint16_t) (((uint32_t) val*((uint32_t) hue%(PWM_TOP+1)))/(PWM_TOP+1));
			break;

			case 3:
			color.red = 0;
			color.green = (uint16_t) (((uint32_t) val*((PWM_TOP+1)-((uint32_t) hue%(PWM_TOP+1))))/(PWM_TOP+1));
			color.blue = val;
			break;

			case 4:
			color.red = (uint16_t) (((uint32_t) val*((uint32_t) hue%(PWM_TOP+1)))/(PWM_TOP+1));
			color.green = 0;
			color.blue = val;
			break;

			case 5:
			color.red = val;
			color.green = 0;
			color.blue = (uint16_t) (((uint32_t) val*((PWM_TOP+1)-((uint32_t) hue%(PWM_TOP+1))))/(PWM_TOP+1));
			break;
		}
} // HSV 2 RGB Binary

ISR(INT0_vect)
{
	mode_switch = 1;
}

ISR(PCINT2_vect)
{
	// Check what PIN is low and update level accordingly
		
	if ((1 << PD1) == (PIND & (1 << PD1)))
	{
		// level down
		if (level >= LEVEL_STEP)
		{
			level = level - LEVEL_STEP;
			level_changed = 1;
		}
	}
	
	if ((1 << PD0) == (PIND & (1 << PD0)))
	{
		// level up
		if (level <= (PWM_TOP - LEVEL_STEP))
		{
			level = level + LEVEL_STEP;
			level_changed = 1;
		}
	}

}
