#include <Arduino.h>

namespace Parameters {
  constexpr uint8_t pin_in = 4;
  constexpr uint8_t pin_out = 15;
  constexpr uint8_t n_iter = 10;
};

volatile uint32_t trigger_time = 0;
volatile uint32_t on_time_us = 0;
volatile uint32_t off_time_us = 0;

volatile bool waiting_for_on = false;
volatile bool waiting_for_off = false;

void IRAM_ATTR pin4Interrupt() {
  uint32_t now = micros();
  
  // Verify pin state directly to prevent contact bounce from triggering the wrong state
  if (waiting_for_on && digitalRead(Parameters::pin_in) == LOW) {
    on_time_us = now - trigger_time;
    waiting_for_on = false; 
  } 
  else if (waiting_for_off && digitalRead(Parameters::pin_in) == HIGH) {
    off_time_us = now - trigger_time;
    waiting_for_off = false;
  }
}

void setup() {
  Serial.begin(115200);
  pinMode(Parameters::pin_in, INPUT_PULLUP);
  pinMode(Parameters::pin_out, OUTPUT);
  digitalWrite(Parameters::pin_out, LOW); // Transistor OFF -> Relay OFF

  attachInterrupt(digitalPinToInterrupt(Parameters::pin_in), pin4Interrupt, CHANGE);
}

void loop() {
  static uint8_t iter_count = 0;
  static uint64_t total_on_time_us = 0;
  static uint64_t total_off_time_us = 0;

  // --- 1. Measure TURN ON Time ---
  waiting_for_off = false;
  waiting_for_on = true;
  trigger_time = micros();
  digitalWrite(Parameters::pin_out, HIGH); // Drive Transistor ON -> Relay ON

  uint32_t timeout_start = millis();
  while (waiting_for_on && (millis() - timeout_start < 500)) {
    // Wait for ON interrupt
  }

  // Mandatory pause to let contacts settle after initial impact bounce
  delay(100); 

  // --- 2. Measure TURN OFF Time ---
  waiting_for_on = false;
  waiting_for_off = true;
  trigger_time = micros();
  digitalWrite(Parameters::pin_out, LOW); // Drive Transistor OFF -> Relay OFF

  timeout_start = millis();
  while (waiting_for_off && (millis() - timeout_start < 500)) {
    // Wait for OFF interrupt
  }

  // --- 3. Process & Display Results ---
  if (!waiting_for_on && !waiting_for_off) {
    total_on_time_us += on_time_us;
    total_off_time_us += off_time_us;
    iter_count++;

    float on_ms = on_time_us / 1000.0f;
    float off_ms = off_time_us / 1000.0f;

    Serial.print("Iteration ");
    Serial.print(iter_count);
    Serial.print(" | ON Time: ");
    Serial.print(on_ms, 2);
    Serial.print(" ms | OFF Time: ");
    Serial.print(off_ms, 2);
    Serial.println(" ms");
  } else {
    Serial.print("Timeout Error - ON state pending: ");
    Serial.print(waiting_for_on);
    Serial.print(" | OFF state pending: ");
    Serial.println(waiting_for_off);
  }

  // Print overall averages once batch finishes
  if (iter_count >= Parameters::n_iter) {
    float avg_on_ms = (float)total_on_time_us / (Parameters::n_iter * 1000.0f);
    float avg_off_ms = (float)total_off_time_us / (Parameters::n_iter * 1000.0f);

    Serial.println("\n**********");
    Serial.print("Mean ON time (");
    Serial.print(Parameters::n_iter);
    Serial.print(" iterations): ");
    Serial.print(avg_on_ms, 2);
    Serial.println(" ms");

    Serial.print("Mean OFF time (");
    Serial.print(Parameters::n_iter);
    Serial.print(" iterations): ");
    Serial.print(avg_off_ms, 2);
    Serial.println(" ms");
    Serial.println("**********\n");

    total_on_time_us = 0;
    total_off_time_us = 0;
    iter_count = 0;
  }

  delay(1000);
}
