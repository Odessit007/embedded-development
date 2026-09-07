#include "led.h"

Led::Led(uint8_t pin, uint32_t blink_period_ms, uint32_t last_blink_time_ms)
  : pin(pin),
    blink_period_ms(blink_period_ms),
    last_blink_time_ms(last_blink_time_ms) {
}

Led::Led(uint8_t pin)
  : pin(pin) {
}

void Led::init(uint8_t mode) {
  pinMode(pin, OUTPUT);
  digitalWrite(pin, mode);
}

void Led::on() {
  digitalWrite(pin, HIGH);
}

void Led::off() {
  digitalWrite(pin, LOW);
}

void Led::blink() {
  uint32_t current_time_ms = millis();
  if (current_time_ms - last_blink_time_ms >= blink_period_ms) {
    digitalWrite(pin, !digitalRead(pin));
    last_blink_time_ms = current_time_ms;
  }
}