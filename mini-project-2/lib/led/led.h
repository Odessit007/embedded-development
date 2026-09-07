#pragma once

#include <Arduino.h>

class Led {
public:
  Led(uint8_t pin, uint32_t blink_period_ms, uint32_t last_blink_time_ms);
  Led(uint8_t pin);

  void init(uint8_t mode);
  void on();
  void off();
  void blink();

private:
  uint8_t pin;
  uint32_t blink_period_ms = 0;
  uint32_t last_blink_time_ms = 0;
};