#include <Arduino.h>
#include <atomic>

namespace Parameters {
  static constexpr uint8_t button_pin_in = 15;
  static constexpr uint8_t debounce_delay_ms = 50;
};

volatile bool button_clicked = false;
volatile uint32_t triggered_click_time_ms = 0;
uint32_t accepted_click_time_ms = 0;
uint16_t click_counter = 0;

void buttonPressedInterrupt() {
  button_clicked = true;
  triggered_click_time_ms = millis();
}

void setup() {
	Serial.begin(115200);

  pinMode(Parameters::button_pin_in, INPUT_PULLUP);

  attachInterrupt(digitalPinToInterrupt(Parameters::button_pin_in), buttonPressedInterrupt, FALLING);
}

void loop() {
  // За вимогою завдання, debounce має бути реалізований поза ISR
  if (button_clicked && (triggered_click_time_ms - accepted_click_time_ms) >= Parameters::debounce_delay_ms) {
    ++click_counter;
    accepted_click_time_ms = triggered_click_time_ms;
    Serial.print("Button clicked. n_clicks = ");
    Serial.println(click_counter);
    Serial.println("***");
    button_clicked = false;
  }
}
