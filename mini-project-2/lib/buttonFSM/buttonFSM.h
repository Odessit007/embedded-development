#pragma once

#include <Arduino.h>

enum ButtonState {
	IDLE,
	MAYBE_PRESSED,
	PRESSED,
	MAYBE_RELEASED
};

class ButtonStateMachine {
public:
	ButtonStateMachine(uint8_t button_pin, uint32_t debounce_delay);

	void init();
	void updateState();
	ButtonState getState();

private:
	uint8_t button_pin;
	uint32_t debounce_delay_ms;
	uint32_t last_trigger_time_ms;
	ButtonState current_state;
};
