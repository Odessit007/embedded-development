Програма використовує зовнішню кнопку і вбудовану кнопку BOOT для контроля над миготінням двох світлодіодів.
* Спочатку обидва світлодіоди не горять.
* При натисканні будь-якої кнопки починається одночасне миготіння світлодіодів.
* Зовнішня кнопка активує швидший режим миготіння.
* Вбудована кнопка активує повільніший режим миготіння.
* Одночасне затиснення обох кнопок вимикає миготіння.


Код з уроку, в якому натиск/відпускання кнопки призводять до єдиного запису у логу (замість постійного логування):
```c++
#include <Arduino.h>

#define BUTTON_IN  16
#define LED_OUT    15
static int lastButtonState = 2;  // Щоб відрізнялось від 0 та 1

void setup() {
    Serial.begin(115200);
    delay(1000); // Очікування відкриття монітора порту

    pinMode(LED_OUT, OUTPUT);
    pinMode(BUTTON_IN, INPUT_PULLUP);
    digitalWrite(LED_OUT, LOW);
}

void loop() {
    int buttonState = digitalRead(BUTTON_IN);

    if (buttonState == LOW) {
        digitalWrite(LED_OUT, HIGH);
        if (lastButtonState != buttonState) {
            Serial.println("Button pressed");
            lastButtonState = LOW;
        }
    } else {
        digitalWrite(LED_OUT, LOW);
        if (lastButtonState != buttonState) {
            Serial.println("Button released");
            lastButtonState = HIGH;
        }
    }

    delay(100);
}
```