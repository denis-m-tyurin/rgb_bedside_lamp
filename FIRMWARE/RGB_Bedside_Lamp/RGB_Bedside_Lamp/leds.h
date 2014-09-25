#ifndef LEDS_H
#define LEDS_H

#define LEDS_NIGHT_LEVEL 10
#define LEDS_MAX_LEVEL 255
#define LEDS_OFF_LEVEL 0

void leds_init();

void leds_set_r_level(int level);
void leds_set_g_level(int level);
void leds_set_b_level(int level);


#endif