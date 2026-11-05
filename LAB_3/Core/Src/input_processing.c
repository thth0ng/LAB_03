#include "main.h"
#include "input_reading.h"
#include "output_display.h"
#include "input_processing.h"
#include "timer.h"
/* Lab 3: mode 1 normal, mode 2 red, mode 3 amber, mode 4 green. */
static uint8_t mode=1, red_duration=7, yellow_duration=3, green_duration=4;
static uint8_t edit_value=0;
static uint8_t phase=0;
static uint32_t phase_start_ms=0;
/* Four phases: road1 green, road1 yellow, road2 green, road2 yellow.
   Red duration equals green+yellow of opposing road to avoid conflicting greens. */
static uint8_t phase_color(uint8_t ph,uint8_t road) {
  if(ph==0) return road==0?2:0;
  if(ph==1) return road==0?1:0;
  if(ph==2) return road==0?0:2;
  return road==0?0:1;
}
static uint8_t phase_seconds(uint8_t ph) {return (ph==0 || ph==2)?green_duration:yellow_duration;}
void application_init(void) {mode=1;red_duration=7;yellow_duration=3;green_duration=4;phase=0;phase_start_ms=timer_now_ms();}
static uint8_t remaining(uint32_t now,uint8_t ph,uint8_t road) {
  uint32_t elapsed=(now-phase_start_ms)/1000;
  uint8_t dur=phase_seconds(ph);
  uint8_t rem=elapsed<dur?(uint8_t)(dur-elapsed):0;
  if(phase_color(ph,road)!=0) return rem;
  /* When red: show countdown across current and next opposite phases. */
  if((ph==0 || ph==2)) return (uint8_t)(rem+yellow_duration);
  return rem;
}
void fsm_for_input_processing(void) {
  uint32_t now=timer_now_ms();
  if(button_pressed_event(BUTTON_MODE)) {
    mode=(mode%4)+1;
    if(mode==2) edit_value=red_duration;
    if(mode==3) edit_value=yellow_duration;
    if(mode==4) edit_value=green_duration;
    if(mode==1) {phase=0;phase_start_ms=now;}
  }
  if(mode==1) {
    while ((uint32_t)(now-phase_start_ms)>=((uint32_t)phase_seconds(phase)*1000U)) {
      phase_start_ms+=(uint32_t)phase_seconds(phase)*1000U;
      phase=(phase+1)%4;
    }
    traffic_set_leds(phase_color(phase,0),phase_color(phase,1));
    display_set_numbers(remaining(now,phase,0),remaining(now,phase,1));
  } else {
    if(button_pressed_event(BUTTON_INCREASE)) edit_value=(edit_value>=99)?1:(edit_value+1);
    if(button_pressed_event(BUTTON_VALUE)) {
      if(mode==2) {
        /* maintain red = yellow + green; require at least one second of green. */
        if(edit_value>yellow_duration) {red_duration=edit_value;green_duration=red_duration-yellow_duration;}
      }
      if(mode==3) {
        if(edit_value<red_duration) {yellow_duration=edit_value;green_duration=red_duration-yellow_duration;}
      }
      if(mode==4) {
        green_duration=edit_value;
        red_duration=green_duration+yellow_duration;
        if(red_duration>99) {red_duration=99;green_duration=red_duration-yellow_duration;}
      }
      /* refresh shown edit value to actual accepted duration */
      edit_value=(mode==2)?red_duration:((mode==3)?yellow_duration:green_duration);
    }
    /* 2Hz blink = 250ms on + 250ms off */
    traffic_blink_edit(mode-2,(now%500U)<250U);
    display_set_numbers(edit_value,mode);
  }
}
