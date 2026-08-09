#include <Arduino.h>

#define BUTTON_IN  17
#define LED_OUT    16
static int lastButtonState = LOW;

// Кнопка має бути підключена як 3.3V <-- Button --> Pin 17

void setup() {
    Serial.begin(115200);
    delay(1000); // Очікування відкриття монітора порту

    pinMode(LED_OUT, OUTPUT);
    pinMode(BUTTON_IN, INPUT_PULLDOWN);
    digitalWrite(LED_OUT, LOW);
}

void loop() {
    int buttonState = digitalRead(BUTTON_IN);

    if (buttonState == HIGH) {
        digitalWrite(LED_OUT, HIGH);
        if (lastButtonState != buttonState) {
            Serial.println("Button pressed");
            lastButtonState = HIGH;
        }
    } else {
        digitalWrite(LED_OUT, LOW);
        if (lastButtonState != buttonState) {
            Serial.println("Button released");
            lastButtonState = LOW;
        }
    }

    delay(100);
}
