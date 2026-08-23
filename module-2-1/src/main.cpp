#include <Arduino.h>

enum LedState {
  ON,
  OFF
};

enum LedMode {
  MODE_BLINK,
  MODE_ON,
  MODE_OFF
};

class Parameters {
public:
  static constexpr uint8_t button_pin_in = 4;
  static constexpr uint8_t led_pin_out = 15;
  static constexpr uint16_t blink_delay_ms = 250;
  static constexpr uint16_t button_debounce_ms = 200;
  static constexpr uint32_t loop_report_iterations = 1000000;
};

class Led {
public:
  void init() {
    pinMode(Parameters::led_pin_out, OUTPUT);
    set(OFF);
    this->last_blink_ms = millis();
  }

  void set(LedState state) {
    this->state = state;
    digitalWrite(Parameters::led_pin_out, state == ON ? HIGH : LOW);
  }

  void blink() {
    const uint32_t now = millis();
    if (now - this->last_blink_ms < Parameters::blink_delay_ms) {
      return;
    }
    this->last_blink_ms = now;
    set(state == ON ? OFF : ON);
  }

  void update_state() {
    switch (this->mode) {
      case MODE_BLINK:
        blink();
        break;
      case MODE_ON:
        set(ON);
        break;
      case MODE_OFF:
        set(OFF);
        break;
    }
  }

  void update_mode() {
    switch (this->mode) {
      case MODE_BLINK:
        this->mode = MODE_ON;
        Serial.print("LED mode changed from BLINK to ON at ");
        Serial.print(millis());
        Serial.println(" ms");
        break;
      case MODE_ON:
        this->mode = MODE_OFF;
        Serial.print("LED mode changed from ON to OFF at ");
        Serial.print(millis());
        Serial.println(" ms");
        break;
      case MODE_OFF:
        this->mode = MODE_BLINK;
        Serial.print("LED mode changed from OFF to BLINK at ");
        Serial.print(millis());
        Serial.println(" ms");
        break;
    }
  }

private:
  LedState state = OFF;
  LedMode mode = MODE_BLINK;
  uint32_t last_blink_ms = 0;
};

class LoopTracker {
public:
  void start() {
    loop_start_us = micros();
  }

  void track() {
    const uint32_t iteration_time_us = micros() - loop_start_us;
    ++loop_iterations;
    total_loop_time_us += iteration_time_us;
    max_loop_time_us = max(max_loop_time_us, iteration_time_us);

    if (loop_iterations % Parameters::loop_report_iterations == 0) {
      Serial.print("Iterations: ");
      Serial.print(loop_iterations);
      Serial.print(", mean loop time so far: ");
      Serial.print(total_loop_time_us / loop_iterations);
      Serial.print(" us, max loop time so far: ");
      Serial.print(max_loop_time_us);
      Serial.println(" us");
    }
  }

private:
  uint32_t loop_start_us = 0;
  uint32_t loop_iterations = 0;
  uint64_t total_loop_time_us = 0;
  uint32_t max_loop_time_us = 0;
};

Led led;

volatile bool button_pressed = false;

void button_interrupt() {
  button_pressed = true;
}

void setup() {
  Serial.begin(115200);

  led.init();

  pinMode(Parameters::button_pin_in, INPUT_PULLUP);
  attachInterrupt(
    digitalPinToInterrupt(Parameters::button_pin_in),
    button_interrupt,
    FALLING
  );
}

void loop() {
  static LoopTracker loop_tracker;
  static uint32_t last_button_press_ms = 0;
  loop_tracker.start();

  if (button_pressed) {
    button_pressed = false;
    const uint32_t now = millis();
    if (now - last_button_press_ms >= Parameters::button_debounce_ms) {
      last_button_press_ms = now;
      Serial.println("!!!  Button clicked  !!!");
      led.update_mode();
    }
  }

  led.update_state();

  loop_tracker.track();
}
