#include <Arduino.h>
#include <atomic>

namespace Parameters {
  static constexpr uint8_t button_pin_in = 15;
  static constexpr uint8_t debounce_delay_ms = 50;
};

volatile bool button_clicked = false;
uint16_t click_counter = 0;

void buttonPressedInterrupt() {
  button_clicked = true;
}

void setup() {
	Serial.begin(115200);

  pinMode(Parameters::button_pin_in, INPUT);

  attachInterrupt(digitalPinToInterrupt(Parameters::button_pin_in), buttonPressedInterrupt, FALLING);
}

void loop() {
  // (*) приймати подію лише якщо кнопка досі натиснута
  // (*) ігнорувати події при відпусканні
  bool button_pin_state = digitalRead(Parameters::button_pin_in);
  if (button_clicked && (button_pin_state == LOW)) {
    ++click_counter;
    Serial.print("Button clicked. n_clicks = ");
    Serial.println(click_counter);
    Serial.println("***");
    button_clicked = false;
  }
}
