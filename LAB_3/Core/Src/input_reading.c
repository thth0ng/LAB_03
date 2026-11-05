#include "main.h"
#include "input_reading.h"
/* Active-low push buttons; sample every 10ms, 3 identical samples = 30ms debounce. */
static const uint16_t pins[3] = {MODE_Pin, INCREASE_Pin, VALUE_Pin};
static uint8_t stable[3], previous[3], count[3], events[3];
static uint16_t held[3];
void button_reading_10ms(void) {
  for (uint8_t i=0;i<3;i++) {
    uint8_t raw = HAL_GPIO_ReadPin(GPIOB,pins[i]) == GPIO_PIN_RESET;
    if(raw == previous[i]) {if(count[i]<3) count[i]++;}
    else {previous[i]=raw; count[i]=1;}
    if(count[i]>=3 && stable[i]!=raw) {
      stable[i]=raw;
      if(raw) events[i]=1;
      else held[i]=0;
    }
    if(stable[i] && held[i]<65535) held[i]++;
  }
}
uint8_t is_button_pressed(uint8_t i) {return i<3 ? stable[i] : 0;}
uint8_t button_pressed_event(uint8_t i) {
  if(i>=3) return 0;
  /* Button events are read and cleared in the same TIM2 ISR. */
  uint8_t result=events[i]; events[i]=0;
  return result;
}
uint8_t is_button_pressed_1s(uint8_t i) {return i<3 && held[i]>=100;}
