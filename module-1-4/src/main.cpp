#include <Arduino.h>

#define BOOT_BUTTON_IN      0
#define EXTERNAL_BUTTON_IN  17
#define RED_LED_OUT         16
#define BLUE_LED_OUT        15

#define DEBOUNCE_DELAY       50
#define FAST_BLINK_INTERVAL  100
#define SLOW_BLINK_INTERVAL  400

/*
  Стан LED.
  * MODE_OFF - обидва світлoдіоди вимкнені (це початковий стан; також активується, коли натиснуті обидві кнопки)
    * Ставимо обидва світлoдіоди у LOW, щоб вимкнути їх.
  * MODE_FAST_BLINK - обидва світлoдіоди блимають швидко (активується, коли натиснута лише зовнішня кнопка)
    * Якщо з останнього миготіння пройшло >= FAST_BLINK_INTERVAL мс, перемикаємо стан світлoдіодів (HIGH <-> LOW).
  * MODE_SLOW_BLINK - обидва світлoдіоди блимають повільно (активується, коли натиснута лише кнопка BOOT)
    * Якщо з останнього миготіння пройшло >= SLOW_BLINK_INTERVAL мс, перемикаємо стан світлoдіодів (HIGH <-> LOW).
*/
enum LedState {
  MODE_OFF,
  MODE_FAST_BLINK,
  MODE_SLOW_BLINK,
};
LedState currentLedMode = MODE_OFF;       // Поточний режим миготіння світлодіодів; регулюється комбінацією двох кнопок
unsigned long previousLedUpdateTime = 0;  // Час останнього оновлення стану світлодіодів
int blueLedState = LOW;                   // Поточний стан синього світлодіода: горить (HIGH) або вимкнений (LOW)
int redLedState = LOW;                    // Поточний стан червоного світлодіода: горить (HIGH) або вимкнений (LOW)

// Комбінація кнопок.
enum ButtonCombination {
  COMBO_NONE,           // обидві кнопки не натиснуті (режим світлодіодів не змінюється)
  COMBO_BOOT_ONLY,      // натиснута лише кнопка BOOT (світлoдіоди переходять в повільний режим миготіння)
  COMBO_EXTERNAL_ONLY,  // натиснута лише зовнішня кнопка (світлодіоди переходять в швидкий режим миготіння)
  COMBO_BOTH,           // обидві кнопки натиснуті (світлодіоди вимикаються)
};
ButtonCombination lastRawButtonCombo = COMBO_NONE;  // Остання зафіксована комбінація кнопок
ButtonCombination stableButtonCombo = COMBO_NONE;   // Поточна стабільна комбінація кнопок (з урахуванням брязкоту контактів)
unsigned long lastButtonComboChangeTime = 0;        // Час останньої зміни комбінації кнопок (для реалізації debounce)


void printWithTimestamp(const char* message) {
  Serial.print(millis());
  Serial.print(" ms | ");
  Serial.println(message);
}

/*
  debounce реалізується шляхом оновлення стабільної комбінації кнопок лише тоді, коли з останньої зміни "сирої" комбінації пройшло >= DEBOUNCE_DELAY мс.
  Контроль миготіння реалізується на основі лише стабільної комбінації, а не сирої, щоб уникнути змін через брязкіт контактів кнопок.
*/
ButtonCombination readDebouncedButtonCombination() {
  const bool bootPressedRaw = (digitalRead(BOOT_BUTTON_IN) == LOW);
  const bool externalPressedRaw = (digitalRead(EXTERNAL_BUTTON_IN) == LOW);

  const ButtonCombination rawButtonCombo =
    bootPressedRaw ? (externalPressedRaw ? COMBO_BOTH : COMBO_BOOT_ONLY)
                  : (externalPressedRaw ? COMBO_EXTERNAL_ONLY : COMBO_NONE);

  const unsigned long now = millis();

  if (rawButtonCombo != lastRawButtonCombo) {
    lastRawButtonCombo = rawButtonCombo;
    lastButtonComboChangeTime = now;
  }

  if ((now - lastButtonComboChangeTime) >= DEBOUNCE_DELAY) {
    stableButtonCombo = rawButtonCombo;
  }

  return stableButtonCombo;
}

void setup() {
    Serial.begin(115200);
    delay(1000); // Очікування відкриття монітора порту

    // pull up --> коли кнопка не натиснута, на вході буде HIGH, коли натиснута - LOW
    pinMode(BOOT_BUTTON_IN, INPUT_PULLUP);
    pinMode(EXTERNAL_BUTTON_IN, INPUT_PULLUP);

    pinMode(BLUE_LED_OUT, OUTPUT);
    pinMode(RED_LED_OUT, OUTPUT);

    digitalWrite(BLUE_LED_OUT, LOW);
    digitalWrite(RED_LED_OUT, LOW); 

    printWithTimestamp("Setup complete.");
}

void blink(int blink_interval) {
  const unsigned long now = millis();
  if (now - previousLedUpdateTime < blink_interval) {
    return;
  }
  previousLedUpdateTime = now;
  if (blueLedState == LOW && redLedState == LOW) {
    // Якщо обидва світлодіоди вимкнені, то щоб вони чергувались треба спочатку ввімкнути один з них і я обрав синій.
    blueLedState = HIGH;
  }
  blueLedState = blueLedState == LOW ? HIGH : LOW;
  redLedState = redLedState == LOW ? HIGH : LOW;
  digitalWrite(BLUE_LED_OUT, blueLedState);
  digitalWrite(RED_LED_OUT, redLedState);
}

void setLedMode(const ButtonCombination currentCombo) {
  switch (currentCombo) {
    case COMBO_BOTH:
      if (currentLedMode != MODE_OFF) {
        printWithTimestamp("Boot: ✅ External: ✅. Off mode");
      }
      currentLedMode = MODE_OFF;
      return;

    case COMBO_BOOT_ONLY:
      if (currentLedMode != MODE_SLOW_BLINK) {
        printWithTimestamp("Boot: ✅ External: ❌. Slow blink mode");
      }
      currentLedMode = MODE_SLOW_BLINK;
      return;

    case COMBO_EXTERNAL_ONLY:
      if (currentLedMode != MODE_FAST_BLINK) {
        printWithTimestamp("Boot: ❌ External: ✅. Fast blink mode");
      }
      currentLedMode = MODE_FAST_BLINK;
      return;

    case COMBO_NONE:
      return;
  }
}

void applyLedMode() {
  switch (currentLedMode) {
    case MODE_OFF:
      digitalWrite(RED_LED_OUT, LOW);
      digitalWrite(BLUE_LED_OUT, LOW);
      break;
    case MODE_FAST_BLINK:
      blink(FAST_BLINK_INTERVAL);
      break;
    case MODE_SLOW_BLINK:
      blink(SLOW_BLINK_INTERVAL);
      break;
  }
}

void loop() {
  const ButtonCombination currentCombo = readDebouncedButtonCombination();  // Зчитуємо стабільну комбінацію кнопок з урахуванням брязкоту контактів
  setLedMode(currentCombo);  // Визначаємо режим миготіння за комбінацією кнопок
  applyLedMode();            // Миготимо світлодіодами відповідно до поточного режиму
}
