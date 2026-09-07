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
  uint8_t button_pin = 15;
  uint32_t debounce_delay_ms = 50;
  // Green LED
  uint8_t green_led_pin = 16;
  uint32_t green_on_time_ms = 5000;
  uint32_t green_blink_time_ms = 3000;
  uint32_t green_blink_period_ms = 500;
  // Yellow LED
  uint8_t yellow_led_pin = 17;
  uint32_t yellow_on_time_ms = 2000;      // Normal mode
  uint32_t yellow_blink_period_ms = 500;  // Emergency mode triggered by button click
  // Red LED
  uint8_t red_led_pin = 18;
  uint32_t red_on_time_ms = 5000;
  uint32_t yellow_red_on_time_ms = 2000;
};

Parameters parameters;

class TrafficLightStateMachine {
  private:
  Parameters parameters;
  ButtonStateMachine button_state_machine;
  Led green_led;
  Led yellow_led;
  Led red_led;
  uint32_t last_change_time_ms = 0;
  TrafficLightState current_state = GREEN_ON;

  const char* stateName(TrafficLightState state) {
    switch (state) {
      case GREEN_ON: return "GREEN_ON";
      case GREEN_BLINK: return "GREEN_BLINK";
      case YELLOW_ON: return "YELLOW_ON";
      case RED_ON: return "RED_ON";
      case YELLOW_AND_RED_ON: return "YELLOW_AND_RED_ON";
      case YELLOW_BLINK: return "YELLOW_BLINK";
    }
    return "UNKNOWN";
  }

  void transitionTo(TrafficLightState next_state) {
    if (current_state == next_state) {
      return;
    }

    Serial.print(stateName(current_state));
    Serial.print(" -> ");
    Serial.println(stateName(next_state));
    current_state = next_state;
  }

  public:
  TrafficLightStateMachine(Parameters parameters)
    : parameters(parameters),
      button_state_machine(parameters.button_pin, parameters.debounce_delay_ms),
      green_led(parameters.green_led_pin, parameters.green_blink_period_ms, 0),
      yellow_led(parameters.yellow_led_pin, parameters.yellow_blink_period_ms, 0),
      red_led(parameters.red_led_pin) {
  }

  void init() {
    button_state_machine.init();
    green_led.init(HIGH);
    yellow_led.init(LOW);
    red_led.init(LOW);
  }

  void updateState() {
      uint32_t current_time_ms = millis();

    ButtonState last_button_state = button_state_machine.getState();
    button_state_machine.updateState();
    ButtonState current_button_state = button_state_machine.getState();
    if (current_button_state == PRESSED && last_button_state != PRESSED) {
      if (current_state != YELLOW_BLINK) {
        transitionTo(YELLOW_BLINK);
        yellow_led.on();
        green_led.off();
        red_led.off();
      } else if (current_state == YELLOW_BLINK) {
        transitionTo(GREEN_ON);
        last_change_time_ms = current_time_ms;
        yellow_led.off();
        green_led.on();
      }
    }

    switch (current_state) {
      case GREEN_ON:
        if (current_time_ms - last_change_time_ms > parameters.green_on_time_ms) {
          transitionTo(GREEN_BLINK);
          last_change_time_ms = current_time_ms;
          green_led.off();
        }
        break;
      case GREEN_BLINK:
        if (current_time_ms - last_change_time_ms <= parameters.green_blink_time_ms) {
          green_led.blink();
        } else {
          transitionTo(YELLOW_ON);
          last_change_time_ms = current_time_ms;
          green_led.off();
          yellow_led.on();
        }
        break;
      case YELLOW_ON:
        if (current_time_ms - last_change_time_ms > parameters.yellow_on_time_ms) {
          transitionTo(RED_ON);
          last_change_time_ms = current_time_ms;
          yellow_led.off();
          red_led.on();
        }
        break;
      case RED_ON:
        if (current_time_ms - last_change_time_ms > parameters.red_on_time_ms) {
          transitionTo(YELLOW_AND_RED_ON);
          last_change_time_ms = current_time_ms;
          yellow_led.on();
          red_led.on();
        }
        break;
      case YELLOW_AND_RED_ON:
        if (current_time_ms - last_change_time_ms > parameters.yellow_red_on_time_ms) {
          transitionTo(GREEN_ON);
          last_change_time_ms = current_time_ms;
          red_led.off();
          yellow_led.off();
          green_led.on();
        }
        break;
      case YELLOW_BLINK:
        yellow_led.blink();
        break;
    }
  }
};

TrafficLightStateMachine traffic_light_state_machine(parameters);

void setup() {
  Serial.begin(115200);

  traffic_light_state_machine.init();
}

void loop() {
  traffic_light_state_machine.updateState();
}
