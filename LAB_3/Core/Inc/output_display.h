#ifndef OUTPUT_DISPLAY_H
#define OUTPUT_DISPLAY_H
#include <stdint.h>
void display_set_numbers(uint8_t left, uint8_t right);
void display_scan_10ms(void);
void traffic_set_leds(uint8_t road1, uint8_t road2);
void traffic_blink_edit(uint8_t color, uint8_t visible);
#endif
