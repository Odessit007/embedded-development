#include "buttonFSM.h"
#include <Arduino.h>

namespace Parameters {
  // Button
  constexpr uint8_t button_pin = 15;
  constexpr uint32_t debounce_delay_ms = 50;
  // LEDs
  constexpr uint8_t green_led_pin = 16;
  constexpr uint8_t yellow_led_pin = 17;
  constexpr uint8_t red_led_pin = 18;
}

ButtonStateMachine button_state_machine(Parameters::button_pin, Parameters::debounce_delay_ms);

void setup() {
  Serial.begin(115200);

  // pinMode(Parameters::button_pin, INPUT_PULLUP);
  pinMode(Parameters::green_led_pin, OUTPUT);
  pinMode(Parameters::yellow_led_pin, OUTPUT);
  pinMode(Parameters::red_led_pin, OUTPUT);

  digitalWrite(Parameters::green_led_pin, LOW);
  digitalWrite(Parameters::yellow_led_pin, LOW);
  digitalWrite(Parameters::red_led_pin, LOW);
}

void loop() {
  digitalWrite(Parameters::green_led_pin, HIGH);
  Serial.println("Green LED ON");
  delay(1000);
  digitalWrite(Parameters::green_led_pin, LOW);
  Serial.println("Green LED OFF");
  delay(1000);

  digitalWrite(Parameters::yellow_led_pin, HIGH);
  Serial.println("Yellow LED ON");
  delay(1000);
  digitalWrite(Parameters::yellow_led_pin, LOW);
  Serial.println("Yellow LED OFF");
  delay(1000);

  digitalWrite(Parameters::red_led_pin, HIGH);
  Serial.println("Red LED ON");
  delay(1000);
  digitalWrite(Parameters::red_led_pin, LOW);
  Serial.println("Red LED OFF");
  delay(1000);
}
