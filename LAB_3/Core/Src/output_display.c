
#include "main.h"
#include "output_display.h"
#include "timer.h"

// PNP: EN LOW = ON, EN HIGH = OFF
#define DIGIT_ON  GPIO_PIN_RESET
#define DIGIT_OFF GPIO_PIN_SET

static const uint16_t seg_pins[7] = {
    SEG_A_Pin, SEG_B_Pin, SEG_C_Pin, SEG_D_Pin,
    SEG_E_Pin, SEG_F_Pin, SEG_G_Pin
};

static const uint16_t enable_pins[4] = {
    EN0_Pin, EN1_Pin, EN2_Pin, EN3_Pin
};

// Bang ma 7-segment: bit 0=A, ..., bit 6=G
static const uint8_t glyphs[11] = {
    0x3F, 0x06, 0x5B, 0x4F, 0x66,
    0x6D, 0x7D, 0x07, 0x7F, 0x6F, 0x00
};

static volatile uint8_t digits[4] = {0, 0, 0, 0};

// left: duong doc, right: duong ngang
void display_set_numbers(uint8_t left, uint8_t right) {
    if (left > 99) left = 99;
    if (right > 99) right = 99;

    digits[0] = left / 10;
    digits[1] = left % 10;
    digits[2] = right / 10;
    digits[3] = right % 10;
}

// Goi moi 10ms tu TIM2
void display_scan_10ms(void) {
    static uint8_t digit = 0;
    static uint8_t frame[4] = {0, 0, 0, 0};

    // The common timer raises this event every 250ms.
    if (!timer_take_digit_event()) {
        return;
    }

    // Turn off all 7-segment displays
    for (uint8_t i = 0; i < 4; i++) {
        HAL_GPIO_WritePin(
            GPIOA, enable_pins[i], DIGIT_OFF
        );
    }

    // Capture all 4 digits at the start of each scan
    if (digit == 0) {
        for (uint8_t i = 0; i < 4; i++) {
            frame[i] = digits[i];
        }
    }

    // Display the captured digit
    uint8_t mask = glyphs[frame[digit]];

    for (uint8_t i = 0; i < 7; i++) {
        HAL_GPIO_WritePin(
            GPIOB,
            seg_pins[i],
            (mask & (1U << i))
                ? GPIO_PIN_RESET
                : GPIO_PIN_SET
        );
    }

    // Enable current digit (Active LOW)
    HAL_GPIO_WritePin(
        GPIOA, enable_pins[digit], DIGIT_ON
    );

    // Next digit
    digit = (digit + 1U) % 4U;
}

// Phan dieu khien den giao thong giu nguyen
static const uint16_t light_pins[2][3] = {
    {RED_1_Pin, YELLOW_1_Pin, GREEN_1_Pin},
    {RED_2_Pin, YELLOW_2_Pin, GREEN_2_Pin}
};

void traffic_set_leds(uint8_t road1, uint8_t road2) {
    for (uint8_t road = 0; road < 2; road++) {
        for (uint8_t c = 0; c < 3; c++) {
            HAL_GPIO_WritePin(
                GPIOA,
                light_pins[road][c],
                ((road ? road2 : road1) == c)
                    ? GPIO_PIN_SET
                    : GPIO_PIN_RESET
            );
        }
    }
}

void traffic_blink_edit(uint8_t color, uint8_t visible) {
    for (uint8_t road = 0; road < 2; road++) {
        for (uint8_t c = 0; c < 3; c++) {
            HAL_GPIO_WritePin(
                GPIOA,
                light_pins[road][c],
                (visible && c == color)
                    ? GPIO_PIN_SET
                    : GPIO_PIN_RESET
            );
        }
    }
}
