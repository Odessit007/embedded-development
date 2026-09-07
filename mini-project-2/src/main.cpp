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
      button_state_machine(parameters.button_pin, parameters.debounce_delay_ms) {
  }

  void init() {
    button_state_machine.init();

    pinMode(parameters.green_led_pin, OUTPUT);
    pinMode(parameters.yellow_led_pin, OUTPUT);
    pinMode(parameters.red_led_pin, OUTPUT);

    digitalWrite(parameters.green_led_pin, HIGH);  // Starting in GREEN_ON state
    digitalWrite(parameters.yellow_led_pin, LOW);
    digitalWrite(parameters.red_led_pin, LOW);
  }

  void updateState() {
      uint32_t current_time_ms = millis();

    ButtonState last_button_state = button_state_machine.getState();
    button_state_machine.updateState();
    ButtonState current_button_state = button_state_machine.getState();
    if (current_button_state == PRESSED && last_button_state != PRESSED) {
      if (current_state != YELLOW_BLINK) {
        transitionTo(YELLOW_BLINK);
        digitalWrite(parameters.yellow_led_pin, HIGH);
        digitalWrite(parameters.green_led_pin, LOW);
        digitalWrite(parameters.red_led_pin, LOW);
      } else if (current_state == YELLOW_BLINK) {
        transitionTo(GREEN_ON);
        last_change_time_ms = current_time_ms;
        digitalWrite(parameters.yellow_led_pin, LOW);
        digitalWrite(parameters.green_led_pin, HIGH);
      }
    }

    switch (current_state) {
      case GREEN_ON:
        if (current_time_ms - last_change_time_ms > parameters.green_on_time_ms) {
          Serial.print("curren_time_ms: ");
          Serial.print(current_time_ms);
          Serial.print(", last_change_time_ms: ");
          Serial.println(last_change_time_ms);
          transitionTo(GREEN_BLINK);
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
          transitionTo(YELLOW_ON);
          last_change_time_ms = current_time_ms;
          digitalWrite(parameters.green_led_pin, LOW);
          digitalWrite(parameters.yellow_led_pin, HIGH);
        }
        break;
      case YELLOW_ON:
        if (current_time_ms - last_change_time_ms > parameters.yellow_on_time_ms) {
          transitionTo(RED_ON);
          last_change_time_ms = current_time_ms;
          digitalWrite(parameters.yellow_led_pin, LOW);
          digitalWrite(parameters.red_led_pin, HIGH);
        }
        break;
      case RED_ON:
        if (current_time_ms - last_change_time_ms > parameters.red_on_time_ms) {
          transitionTo(YELLOW_AND_RED_ON);
          last_change_time_ms = current_time_ms;
          digitalWrite(parameters.yellow_led_pin, HIGH);
          digitalWrite(parameters.red_led_pin, HIGH);
        }
        break;
      case YELLOW_AND_RED_ON:
        if (current_time_ms - last_change_time_ms > parameters.yellow_red_on_time_ms) {
          transitionTo(GREEN_ON);
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

  traffic_light_state_machine.init();
}

void loop() {
  traffic_light_state_machine.updateState();
}
