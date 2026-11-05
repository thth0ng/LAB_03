#ifndef INPUT_READING_H
#define INPUT_READING_H
#include <stdint.h>
#define BUTTON_MODE 0
#define BUTTON_INCREASE 1
#define BUTTON_VALUE 2
void button_reading_10ms(void);
uint8_t is_button_pressed(uint8_t index);
uint8_t button_pressed_event(uint8_t index);
uint8_t is_button_pressed_1s(uint8_t index);
#endif
