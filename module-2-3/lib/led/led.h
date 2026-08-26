#ifndef LED_H
#define LED_H

#include <stdint.h>

typedef enum {
  ON,
  OFF
} LedState_t;

typedef struct {
  uint8_t pin_out;
  uint16_t delay_ms;
  LedState_t state;
  uint32_t last_blink_ms;
} Led_t;

int init_led(Led_t *led, uint8_t pin_out, uint16_t delay_ms);

int set_led_state(Led_t *led, LedState_t state);

int blink_led(Led_t *led);

#endif // LED_H
