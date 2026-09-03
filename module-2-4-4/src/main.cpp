#include <Arduino.h>

enum ButtonState {
	IDLE,
	MAYBE_PRESSED,
	PRESSED,
	MAYBE_RELEASED
};

uint16_t click_counter = 0;

class StateMachine {
public:
  StateMachine(uint8_t button_pin, uint32_t debounce_delay)
    : button_pin(button_pin),
      debounce_delay_ms(debounce_delay),
      last_trigger_time_ms(0),
      current_state(IDLE) {}

  void initPins() {
    pinMode(button_pin, INPUT_PULLUP);

    Serial.println("Pins initialized");
  }

  void updateState() {
    /*
    Transitions:
    IDLE -- (button pin = LOW) --> MAYBE_PRESSED
    MAYBE_PRESSED -- (debounce delay elapsed, button pin = LOW ~ press confirmed) --> PRESSED
    MAYBE_PRESSED -- (debounce delay elapsed, button pin = HIGH ~ press not confirmed) --> IDLE
    PRESSED -- (button pin = HIGH) --> MAYBE_RELEASED
    MAYBE_RELEASED -- (debounce delay elapsed, button pin = HIGH ~ release confirmed) --> IDLE
    MAYBE_RELEASED -- (debounce delay elapsed, button pin = LOW ~ release not confirmed) --> PRESSED
    */
    uint32_t current_time_ms = millis();
    uint8_t button_pin_value = digitalRead(button_pin);

    switch (current_state) {
      case IDLE:
        if (button_pin_value == LOW) {
          current_state = MAYBE_PRESSED;
          last_trigger_time_ms = current_time_ms;
          Serial.println("IDLE --> MAYBE_PRESSED");
        }
        break;
      case MAYBE_PRESSED:
        if (current_time_ms - last_trigger_time_ms >= debounce_delay_ms) {
          if (button_pin_value == LOW) {
            current_state = PRESSED;
            ++click_counter;
            Serial.print("MAYBE_PRESSED --> PRESSED. n_clicks = ");
            Serial.println(click_counter);
          } else {
            current_state = IDLE;
            Serial.println("MAYBE_PRESSED --> IDLE");
          }
        }
        break;
      case PRESSED:
        if (button_pin_value == HIGH) {
          current_state = MAYBE_RELEASED;
          last_trigger_time_ms = current_time_ms;
          Serial.println("PRESSED --> MAYBE_RELEASED");
        }
        break;
      case MAYBE_RELEASED:
        if (current_time_ms - last_trigger_time_ms >= debounce_delay_ms) {
          if (button_pin_value == HIGH) {
            current_state = IDLE;
            Serial.println("MAYBE_RELEASED --> IDLE");
          } else {
            current_state = PRESSED;
            ++click_counter;
            Serial.println("MAYBE_RELEASED --> PRESSED. n_clicks = ");
            Serial.println(click_counter);
          }
        }
        break;
    }
  }

private:
  uint8_t button_pin;
  uint32_t debounce_delay_ms;
  uint32_t last_trigger_time_ms;
  ButtonState current_state;
};

StateMachine state_machine(15, 50);

void setup() {
	Serial.begin(115200);

  state_machine.initPins();
}

void loop() {
  state_machine.updateState();
  delay(10); // питувати кнопку кожні 5–10 мс
}
