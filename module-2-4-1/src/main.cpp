#include <Arduino.h>
#include <atomic>

namespace Parameters {
  static constexpr uint8_t button_pin_in = 15;
};

volatile bool button_clicked = false;
std::atomic<uint16_t> click_counter{0};

void buttonPressedInterrupt() {
  button_clicked = true;
  click_counter.fetch_add(1, std::memory_order_relaxed);
}

void setup() {
	Serial.begin(115200);

  pinMode(Parameters::button_pin_in, INPUT_PULLUP);

  attachInterrupt(digitalPinToInterrupt(Parameters::button_pin_in), buttonPressedInterrupt, FALLING);
}

void loop() {
  if (button_clicked) {
    Serial.print("Button clicked. n_clicks = ");
    Serial.println(click_counter.load(std::memory_order_relaxed));
    button_clicked = false;
    Serial.println("***");
  }
}
