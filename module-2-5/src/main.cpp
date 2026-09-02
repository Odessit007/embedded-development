#include <Arduino.h>

namespace Parameters {
  constexpr uint8_t out_pin = 16;  // Assuming an active-high relay is connected here that controls a fan
  
  constexpr uint32_t on_duration_s = 15 * 60;                           // 15 minutes ON
  constexpr uint32_t off_duration_s = 45 * 60;                          // 45 minutes OFF
  constexpr uint32_t cycle_duration_s = on_duration_s + off_duration_s; // 1 full cycle = 60 minutes

  // Debug parameters
  // constexpr uint32_t on_duration_s = 1;                                 // debug: 1 second ON
  // constexpr uint32_t off_duration_s = 3;                                // debug: 3 seconds OFF
  // constexpr uint32_t cycle_duration_s = on_duration_s + off_duration_s; // debug: 1 full cycle = 12 seconds

  constexpr uint32_t tick_period_us = 1000000;   // 1M microseconds = 1 second
}

volatile uint32_t cycle_counter = 0;
volatile bool is_on = false;
volatile bool timer_fired = false;
hw_timer_t * timer = nullptr;

void IRAM_ATTR onTimer() {
  ++cycle_counter;  // The interrupts are rare (once per second so should be fine to not use atomic
  if (cycle_counter == Parameters::cycle_duration_s) {
    cycle_counter = 0;
  }

  // 0..14 --> ON; 15..49 --> OFF
  const bool should_be_on = (cycle_counter < Parameters::on_duration_s);

  if (should_be_on != is_on) { // State change
    digitalWrite(Parameters::out_pin, should_be_on ? HIGH : LOW);
    is_on = should_be_on;
    timer_fired = true;
  }
}

void setup() {
  Serial.begin(115200);
  pinMode(Parameters::out_pin, OUTPUT);
  digitalWrite(Parameters::out_pin, LOW);

  timer = timerBegin(0, 80, true);  // 80 MHz / 80 = 1 MHz tick
  timerAttachInterrupt(timer, &onTimer, true);
  timerAlarmWrite(timer, Parameters::tick_period_us, true); // Every 1 second
  timerAlarmEnable(timer);

  Serial.println("Fan control started: 15 minutes ON, 45 minutes OFF");
}

void loop() {
  if (timer_fired) {
    timer_fired = false;
    if (is_on) {
      Serial.println("Turned ON");
    } else {
      Serial.println("Turned OFF");
    }
  }
}
