#include "main.h"
#include "timer.h"
#include "input_reading.h"
#include "input_processing.h"
#include "output_display.h"

/* All timer state is owned by the TIM2 interrupt after startup. */
static uint32_t elapsed_10ms = 0U;
static uint8_t digit_divider = 0U;
static uint8_t digit_event = 0U;

void timer_reset(void) {
    elapsed_10ms = 0U;
    digit_divider = 0U;
    digit_event = 0U;
}

void timer_tick_10ms(void) {
    ++elapsed_10ms;
    ++digit_divider;
    if (digit_divider >= 25U) {
        digit_divider = 0U;
        digit_event = 1U;
    }
}

uint32_t timer_now_ms(void) {
    return elapsed_10ms * 10U;
}

uint8_t timer_take_digit_event(void) {
    uint8_t result = digit_event;
    digit_event = 0U;
    return result;
}

/* Keep all time-dependent updates in one deterministic TIM2 callback. */
void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim) {
    if (htim->Instance != TIM2) {
        return;
    }

    timer_tick_10ms();
    button_reading_10ms();
    fsm_for_input_processing();
    display_scan_10ms();
}
