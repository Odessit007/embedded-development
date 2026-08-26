#include <Arduino.h>
#include "led.h"


static Led_t red_led;
static Led_t green_led;
static Led_t blue_led;

void setup() {
  if (init_led(&red_led, 15, 200) != 0) {
    Serial.println("Failed to initialize the red LED");
  }
  if (init_led(&green_led, 16, 500) != 0) {
    Serial.println("Failed to initialize the green LED");
  }
  if (init_led(&blue_led, 17, 1000) != 0) {
    Serial.println("Failed to initialize the blue LED");
  }
}

void loop() {
  blink_led(&red_led);
  blink_led(&green_led);
  blink_led(&blue_led);
}
