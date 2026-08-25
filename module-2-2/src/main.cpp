#include <Arduino.h>

class Parameters {
public:
  static constexpr uint8_t pin_in = 4;
  static constexpr uint8_t pin_out = 15;
  static constexpr uint8_t n_iter = 10;
};

volatile uint32_t last_activation_time = 0;
volatile uint32_t activation_interval = 0;
volatile bool relay_activated = false;
volatile bool waiting_for_relay = false;

void pin4Interrupt() {
  const uint32_t activation_time = millis();

  if (last_activation_time != 0) {
    
    activation_interval = activation_time - last_activation_time;
    relay_activated = true;
  }
  last_activation_time = activation_time;
}

void setup() {
  Serial.begin(115200);
  pinMode(Parameters::pin_in, INPUT_PULLUP);
  pinMode(Parameters::pin_out, OUTPUT);
  digitalWrite(Parameters::pin_out, LOW);
  attachInterrupt(digitalPinToInterrupt(Parameters::pin_in), pin4Interrupt, FALLING);
}


void loop() {
  static uint8_t n_iter = 0;
  static uint32_t total_time = 0;

  if (!waiting_for_relay) {
    Serial.println("Setting pin_out to HIGH");
    digitalWrite(Parameters::pin_out, HIGH);
    waiting_for_relay = true;
  }

  if (!relay_activated) {
    return;
  }

  noInterrupts();
  const uint32_t iteration_time = activation_interval;
  waiting_for_relay = false;
  relay_activated = false;
  digitalWrite(Parameters::pin_out, LOW);
  interrupts();

  Serial.print("Current time: ");
  Serial.print(iteration_time);
  Serial.println("ms");
  total_time += iteration_time;
  ++n_iter;

  if (n_iter == Parameters::n_iter) {
    Serial.print("Mean time for the last ");
    Serial.print(Parameters::n_iter);
    Serial.print(" iterations: ");
    Serial.print(total_time / Parameters::n_iter);
    Serial.println("ms");
    total_time = 0;
    n_iter = 0;
  }
}
