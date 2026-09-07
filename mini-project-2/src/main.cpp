#include "trafficLightFSM.h"
#include <Arduino.h>

Parameters parameters = {
    15,    // button_pin
    50,    // debounce_delay_ms
    16,    // green_led_pin
    5000,  // green_on_time_ms
    3000,  // green_blink_time_ms
    500,   // green_blink_period_ms
    17,    // yellow_led_pin
    2000,  // yellow_on_time_ms
    500,   // yellow_blink_period_ms
    18,    // red_led_pin
    5000,  // red_on_time_ms
    2000   // yellow_red_on_time_ms
};

TrafficLightStateMachine traffic_light_state_machine(parameters);

void setup() {
  Serial.begin(115200);

  traffic_light_state_machine.init();
}

void loop() {
  traffic_light_state_machine.updateState();
}
