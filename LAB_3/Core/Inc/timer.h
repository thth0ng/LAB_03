#ifndef LAB3_TIMER_H
#define LAB3_TIMER_H

#include <stdint.h>

/* Timer 2 generates one interrupt every 10 ms. */
void timer_reset(void);
void timer_tick_10ms(void);
uint32_t timer_now_ms(void);
/* One display-digit event per 25 TIM2 interrupts = 250 ms. */
uint8_t timer_take_digit_event(void);

#endif
