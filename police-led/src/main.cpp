#include <Arduino.h>

#define BLUE_LED_OUT 16
#define RED_LED_OUT 17

void blink(uint8_t pin, uint16_t times, uint16_t delayTime);

void setup() {
    Serial.begin(115200);
    delay(1000);

    pinMode(BLUE_LED_OUT, OUTPUT);
    pinMode(RED_LED_OUT, OUTPUT);

    digitalWrite(BLUE_LED_OUT, LOW);
    digitalWrite(RED_LED_OUT, LOW);
}

void blink(uint8_t pin, uint16_t times, uint16_t delayTime) {
    for (uint16_t i = 0; i < times; i++) {
        digitalWrite(pin, HIGH);
        delay(delayTime);
        digitalWrite(pin, LOW);
        delay(delayTime);
    }
}

void loop() {
  uint16_t start_gap = 1000;
  uint16_t current_gap = start_gap;
  int16_t gap_delta = 0;
  while (1 == 1) {
    if (current_gap >= start_gap) {
      Serial.println("Increasing speed");
      gap_delta = -200;
    }
    if (current_gap <= 200) {
      Serial.println("Decreasing speed");
      gap_delta = 200;
    }
    Serial.println("Blinking with the blue LED");
    blink(BLUE_LED_OUT, 2, current_gap);
    Serial.println("Blinking with the red LED");
    blink(RED_LED_OUT, 2, current_gap);
    current_gap += gap_delta;
    Serial.print("Gap updated. Current gap = ");
    Serial.println(current_gap);
  }
}