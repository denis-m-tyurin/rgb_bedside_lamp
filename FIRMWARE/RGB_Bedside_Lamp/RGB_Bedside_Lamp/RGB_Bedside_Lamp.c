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
#include "leds.h"

struct RGB
{
	uint8_t red;
	uint8_t green;
	uint8_t blue;
};

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


int main(void)
{
	struct RGB color;
	uint16_t hue;
	
	leds_init();
			
    while(1)
    {
		for (hue = 0; hue < 1536; hue++)
		{
			color = HSV2RGB_Adv1(hue, 255, LEDS_MAX_LEVEL);
			leds_set_rgb(color.red, color.green, color.blue);
			_delay_ms(15);
		}
		
		for (hue = 0; hue < 1536; hue++)
		{
			color = HSV2RGB_Adv1(hue, 255, LEDS_NIGHT_LEVEL);
			leds_set_rgb(color.red, color.green, color.blue);
			_delay_ms(15);
		}
	
		for (hue = 0; hue < 1536; hue++)
		{
			color = HSV2RGB_Adv1(hue, 255, LEDS_NIGHT_LEVEL_5);
			leds_set_rgb(color.red, color.green, color.blue);
			_delay_ms(15);
		}
		
		for (hue = 0; hue < 1536; hue++)
		{
			color = HSV2RGB_Adv1(hue, 255, LEDS_NIGHT_LEVEL_1);
			leds_set_rgb(color.red, color.green, color.blue);
			_delay_ms(15);
		}
		
    }
}