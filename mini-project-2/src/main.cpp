#include "buttonFSM.h"
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
  uint32_t last_change_time_ms = 0;
  uint32_t last_blink_time_ms = 0;
  TrafficLightState current_state = GREEN_ON;

  public:
  TrafficLightStateMachine(Parameters parameters)
    : parameters(parameters),
      button_state_machine(parameters.button_pin, parameters.debounce_delay_ms) {
  }

  void updateState() {
    button_state_machine.updateState();
    ButtonState button_state = button_state_machine.getState();
    if (button_state == PRESSED && current_state != YELLOW_BLINK) {
      current_state = YELLOW_BLINK;
    } else if (button_state == IDLE && current_state == YELLOW_BLINK) {
      current_state = GREEN_ON;
    }

    switch (current_state) {
      uint32_t current_time_ms = millis();
      case GREEN_ON:
        // <= 5s since the last change, still green
        if (current_time_ms - last_change_time_ms <= parameters.green_on_time_ms) {
          digitalWrite(parameters.green_led_pin, HIGH);
        // > 5s --> switching to BLINK mode, turn off the green LED, update timers.
        } else {
          current_state = GREEN_BLINK;
          last_change_time_ms = current_time_ms;
          last_blink_time_ms = current_time_ms;
          digitalWrite(parameters.green_led_pin, LOW);
        }
        break;
      case GREEN_BLINK:
        if (current_time_ms - last_change_time_ms <= parameters.green_blink_time_ms) {
          if (current_time_ms - last_blink_time_ms >= parameters.green_blink_period_ms) {
            digitalWrite(parameters.green_led_pin, !digitalRead(parameters.green_led_pin));
            last_blink_time_ms = current_time_ms;
          }
        } else {
          current_state = YELLOW_ON;
          last_change_time_ms = current_time_ms;
          digitalWrite(parameters.green_led_pin, LOW);
          digitalWrite(parameters.yellow_led_pin, HIGH);
        }
        break;
      case YELLOW_ON:
        if (current_time_ms - last_change_time_ms <= parameters.yellow_on_time_ms) {
          digitalWrite(parameters.yellow_led_pin, HIGH);
        } else {
          current_state = YELLOW_AND_RED_ON;
          last_change_time_ms = current_time_ms;
          digitalWrite(parameters.red_led_pin, HIGH);
        }
        break;
      case RED_ON:
        if (current_time_ms - last_change_time_ms <= parameters.red_on_time_ms) {
          digitalWrite(parameters.red_led_pin, HIGH);
        } else {
          current_state = YELLOW_AND_RED_ON;
          last_change_time_ms = current_time_ms;
          digitalWrite(parameters.yellow_led_pin, HIGH);
          digitalWrite(parameters.red_led_pin, HIGH);
        }
        break;
      case YELLOW_AND_RED_ON:
        if (current_time_ms - last_change_time_ms <= parameters.yellow_red_on_time_ms) {
          digitalWrite(parameters.yellow_led_pin, HIGH);
          digitalWrite(parameters.red_led_pin, HIGH);
        } else {
          current_state = GREEN_ON;
          last_change_time_ms = current_time_ms;
          digitalWrite(parameters.yellow_led_pin, LOW);
          digitalWrite(parameters.red_led_pin, LOW);
          digitalWrite(parameters.green_led_pin, HIGH);
        }
        break;
      case YELLOW_BLINK:
        if (current_time_ms - last_blink_time_ms >= parameters.yellow_blink_period_ms) {
          digitalWrite(parameters.yellow_led_pin, !digitalRead(parameters.yellow_led_pin));
          last_blink_time_ms = current_time_ms;
        }
        break;
    }
  }
};

TrafficLightStateMachine traffic_light_state_machine(parameters);

void setup() {
  Serial.begin(115200);

  // pinMode(parameters.button_pin, INPUT_PULLUP);
  pinMode(parameters.green_led_pin, OUTPUT);
  pinMode(parameters.yellow_led_pin, OUTPUT);
  pinMode(parameters.red_led_pin, OUTPUT);

  digitalWrite(parameters.green_led_pin, LOW);
  digitalWrite(parameters.yellow_led_pin, LOW);
  digitalWrite(parameters.red_led_pin, LOW);
}

void loop() {
  traffic_light_state_machine.updateState();
  // digitalWrite(parameters.green_led_pin, HIGH);
  // Serial.println("Green LED ON");
  // delay(1000);
  // digitalWrite(parameters.green_led_pin, LOW);
  // Serial.println("Green LED OFF");
  // delay(1000);

  // digitalWrite(parameters.yellow_led_pin, HIGH);
  // Serial.println("Yellow LED ON");
  // delay(1000);
  // digitalWrite(parameters.yellow_led_pin, LOW);
  // Serial.println("Yellow LED OFF");
  // delay(1000);

  // digitalWrite(parameters.red_led_pin, HIGH);
  // Serial.println("Red LED ON");
  // delay(1000);
  // digitalWrite(parameters.red_led_pin, LOW);
  // Serial.println("Red LED OFF");
  // delay(1000);
}
