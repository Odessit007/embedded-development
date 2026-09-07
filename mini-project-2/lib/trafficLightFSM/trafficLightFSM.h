#pragma once

#include "buttonFSM.h"
#include "led.h"
#include <Arduino.h>

enum TrafficLightState {
  GREEN_ON,
  GREEN_BLINK,
  YELLOW_ON,
  RED_ON,
  YELLOW_AND_RED_ON,
  YELLOW_BLINK
};

struct Parameters {
  // Button
  uint8_t button_pin;
  uint32_t debounce_delay_ms;
  // Green LED
  uint8_t green_led_pin;
  uint32_t green_on_time_ms;
  uint32_t green_blink_time_ms;
  uint32_t green_blink_period_ms;
  // Yellow LED
  uint8_t yellow_led_pin;
  uint32_t yellow_on_time_ms;      // Normal mode
  uint32_t yellow_blink_period_ms; // Emergency mode triggered by button click
  // Red LED
  uint8_t red_led_pin;
  uint32_t red_on_time_ms;
  uint32_t yellow_red_on_time_ms;
};


class TrafficLightStateMachine {
  private:
  Parameters parameters;
  ButtonStateMachine button_state_machine;
  Led green_led;
  Led yellow_led;
  Led red_led;
  uint32_t last_change_time_ms = 0;
  TrafficLightState current_state = GREEN_ON;

  const char* stateName(TrafficLightState state);
  void transitionTo(TrafficLightState next_state);

  public:
  TrafficLightStateMachine(Parameters parameters);
  void init();
  void updateState();
};
 