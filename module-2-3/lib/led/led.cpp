#include <Arduino.h>
#include "led.h"

int set_led_state(Led_t *led, LedState_t state) {
    if (led == NULL) {
        Serial.println("Error: Led_t pointer is NULL");
        return -1;
    }

    led->state = state;
    digitalWrite(led->pin_out, state == ON ? HIGH : LOW);

    return 0;
}

int init_led(Led_t *led, uint8_t pin_out, uint16_t delay_ms) {
    if (led == NULL) {
        Serial.println("Error: Led_t pointer is NULL");
        return -1;
    }

    led->pin_out = pin_out;
    led->delay_ms = delay_ms;
    led->state = OFF;
    led->last_blink_ms = 0;

    pinMode(led->pin_out, OUTPUT);
    set_led_state(led, OFF);

    return 0;
}

int blink_led(Led_t *led) {
    if (led == NULL) {
        Serial.println("Error: Led_t pointer is NULL");
        return -1;
    }

    const uint32_t now = millis();
    if (now - led->last_blink_ms < led->delay_ms) {
        return 0;
    }
    led->last_blink_ms = now;
    set_led_state(led, led->state == ON ? OFF : ON);

    return 0;
}
