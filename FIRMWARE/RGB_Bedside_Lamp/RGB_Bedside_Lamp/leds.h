#ifndef LEDS_H
#define LEDS_H

#define LEDS_NIGHT_LEVEL 10
#define LEDS_NIGHT_LEVEL_5 5
#define LEDS_NIGHT_LEVEL_1 1
#define LEDS_MAX_LEVEL 255
#define LEDS_OFF_LEVEL 0

void leds_init();

void leds_set_rgb(uint8_t r, uint8_t g, uint8_t b);


#endif