#include <Arduino.h>

#define LDR_IN 4        // Фоторезистор (ADC)
#define RELAY_OUT 15    // База транзистора (Реле)

// Порогові значення ADC (0...4095 для 12-бітного ADC ESP32-S3)
constexpr uint32_t THRESHOLD_DARK = 2000;   // Нижче цього рівня вважаємо, що темно
constexpr uint32_t THRESHOLD_LIGHT = 2800;  // Вище цього рівня вважаємо, що світло
constexpr uint8_t SAMPLES = 10;  // Кількість вимірювань для усереднення


uint32_t readMilliVoltsAverage() {
    uint32_t sum = 0;
    for (uint8_t i = 0; i < SAMPLES; ++i) {
        sum += analogReadMilliVolts(LDR_IN);
    }
    return sum / SAMPLES;
}


void setup() {
  Serial.begin(115200);
  delay(1000);
  analogReadResolution(12);

  pinMode(LDR_IN, INPUT);
  pinMode(RELAY_OUT, OUTPUT);

  // Початковий стан: реле вимкнено
  digitalWrite(RELAY_OUT, LOW);

  Serial.println("System Ready");
  Serial.flush();
}

void loop() {
  static bool relayOn = false;
  const uint32_t voltageMilliVolts = readMilliVoltsAverage();

  if (voltageMilliVolts < THRESHOLD_DARK) {
    relayOn = true;
    digitalWrite(RELAY_OUT, HIGH);  // Реле активне
  }
  else if (voltageMilliVolts > THRESHOLD_LIGHT) {
    relayOn = false;
    digitalWrite(RELAY_OUT, LOW);  // Реле вимкнене
  }
  // Зона між порогами ігнорується, стан релє залишається незмінним

  Serial.print("Voltage (mV): ");
  Serial.print(voltageMilliVolts);
  Serial.print(" | Relay: ");
  Serial.println(relayOn ? "on" : "off");

  delay(500);
}
