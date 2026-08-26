#include <Arduino.h>

enum LedState {
  ON,
  OFF
};

class Parameters {
public:
  static constexpr uint8_t red_pin_out = 15;
  static constexpr uint8_t green_pin_out = 16;
  static constexpr uint8_t blue_pin_out = 17;

  static constexpr uint16_t red_pin_delay_ms = 200;
  static constexpr uint16_t green_pin_delay_ms = 500;
  static constexpr uint16_t blue_pin_delay_ms = 1000;
};

class Led {
public:
  explicit Led(uint8_t pin_out, uint16_t delay_ms): pin_out(pin_out), delay_ms(delay_ms) {}

  void init() {
    pinMode(this->pin_out, OUTPUT);
    set(OFF);
    this->last_blink_ms = millis();
  }

  void set(LedState state) {
    this->state = state;
    digitalWrite(this->pin_out, state == ON ? HIGH : LOW);
  }

  void blink() {
    const uint32_t now = millis();
    if (now - this->last_blink_ms < this->delay_ms) {
      return;
    }
    this->last_blink_ms = now;
    set(state == ON ? OFF : ON);
  }
private:
  uint8_t pin_out;
  uint16_t delay_ms;
  LedState state = OFF;
  uint32_t last_blink_ms = 0;
};


Led red_led(Parameters::red_pin_out, Parameters::red_pin_delay_ms);
Led green_led(Parameters::green_pin_out, Parameters::green_pin_delay_ms);
Led blue_led(Parameters::blue_pin_out, Parameters::blue_pin_delay_ms);

void setup() {
  red_led.init();
  green_led.init();
  blue_led.init();
}

void loop() {
  red_led.blink();
  green_led.blink();
  blue_led.blink();
}
