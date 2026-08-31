#include <Arduino.h>

class Parameters {
public:
  static constexpr uint8_t pin_in = 4;
  static constexpr uint8_t pin_out = 15;
  static constexpr uint8_t n_iter = 10;
};

uint32_t trigger_time = 0;
volatile uint32_t response_time = 0;
volatile bool relay_triggered = false;

void pin4Interrupt() {
  if (!relay_triggered) {
    response_time = millis() - trigger_time;
    relay_triggered = true;
  }
}

void setup() {
  Serial.begin(115200);
  pinMode(Parameters::pin_in, INPUT_PULLUP);
  pinMode(Parameters::pin_out, OUTPUT);
  digitalWrite(Parameters::pin_out, LOW); // Transistor OFF, Relay OFF
  attachInterrupt(digitalPinToInterrupt(Parameters::pin_in), pin4Interrupt, FALLING);
}

void loop() {
  static uint8_t iter_count = 0;
  static uint32_t total_time = 0;

  // Pin = HIGH -> BC547 ON -> relay IN = LOW -> Relay ON
  relay_triggered = false;
  trigger_time = millis();
  digitalWrite(Parameters::pin_out, HIGH);

  uint32_t timeout_start = millis();
  while (!relay_triggered && (millis() - timeout_start < 200)) {
    // Wait for interrupt
  }

  // Turn OFF relay
  digitalWrite(Parameters::pin_out, LOW);

  if (relay_triggered) {
    uint32_t elapsed = response_time;
    total_time += elapsed;
    iter_count++;

    Serial.print("Iteration ");
    Serial.print(iter_count);
    Serial.print(": Response time = ");
    Serial.print(elapsed);
    Serial.println(" ms");
  } else {
    Serial.println("Error: Relay activation timeout!");
  }

  // Print overall average once batch finishes
  if (iter_count >= Parameters::n_iter) {
    Serial.print("\n--> Mean activation time for ");
    Serial.print(Parameters::n_iter);
    Serial.print(" iterations: ");
    Serial.print((float)total_time / Parameters::n_iter);
    Serial.println(" ms\n");

    total_time = 0;
    iter_count = 0;
  }

  delay(1000); 
}
