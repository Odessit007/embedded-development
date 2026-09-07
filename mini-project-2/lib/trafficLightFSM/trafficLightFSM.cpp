#include "buttonFSM.h"
#include "led.h"
#include "trafficLightFSM.h"
#include <Arduino.h>

const char *TrafficLightStateMachine::stateName(TrafficLightState state)
{
    switch (state)
    {
    case GREEN_ON:
        return "GREEN_ON";
    case GREEN_BLINK:
        return "GREEN_BLINK";
    case YELLOW_ON:
        return "YELLOW_ON";
    case RED_ON:
        return "RED_ON";
    case YELLOW_AND_RED_ON:
        return "YELLOW_AND_RED_ON";
    case YELLOW_BLINK:
        return "YELLOW_BLINK";
    }
    return "UNKNOWN";
}

void TrafficLightStateMachine::transitionTo(TrafficLightState next_state)
{
    if (current_state == next_state)
    {
        return;
    }

    Serial.print(stateName(current_state));
    Serial.print(" -> ");
    Serial.println(stateName(next_state));
    current_state = next_state;
}

TrafficLightStateMachine::TrafficLightStateMachine(Parameters parameters)
    : parameters(parameters),
      button_state_machine(parameters.button_pin, parameters.debounce_delay_ms),
      green_led(parameters.green_led_pin, parameters.green_blink_period_ms, 0),
      yellow_led(parameters.yellow_led_pin, parameters.yellow_blink_period_ms, 0),
      red_led(parameters.red_led_pin)
{
}

void TrafficLightStateMachine::init()
{
    button_state_machine.init();
    green_led.init(HIGH);
    yellow_led.init(LOW);
    red_led.init(LOW);
}

void TrafficLightStateMachine::updateState()
{
    uint32_t current_time_ms = millis();

    ButtonState last_button_state = button_state_machine.getState();
    button_state_machine.updateState();
    ButtonState current_button_state = button_state_machine.getState();
    if (current_button_state == PRESSED && last_button_state != PRESSED)
    {
        if (current_state != YELLOW_BLINK)
        {
            transitionTo(YELLOW_BLINK);
            yellow_led.on();
            green_led.off();
            red_led.off();
        }
        else if (current_state == YELLOW_BLINK)
        {
            transitionTo(GREEN_ON);
            last_change_time_ms = current_time_ms;
            yellow_led.off();
            green_led.on();
        }
    }

    switch (current_state)
    {
    case GREEN_ON:
        if (current_time_ms - last_change_time_ms > parameters.green_on_time_ms)
        {
            transitionTo(GREEN_BLINK);
            last_change_time_ms = current_time_ms;
            green_led.off();
        }
        break;
    case GREEN_BLINK:
        if (current_time_ms - last_change_time_ms <= parameters.green_blink_time_ms)
        {
            green_led.blink();
        }
        else
        {
            transitionTo(YELLOW_ON);
            last_change_time_ms = current_time_ms;
            green_led.off();
            yellow_led.on();
        }
        break;
    case YELLOW_ON:
        if (current_time_ms - last_change_time_ms > parameters.yellow_on_time_ms)
        {
            transitionTo(RED_ON);
            last_change_time_ms = current_time_ms;
            yellow_led.off();
            red_led.on();
        }
        break;
    case RED_ON:
        if (current_time_ms - last_change_time_ms > parameters.red_on_time_ms)
        {
            transitionTo(YELLOW_AND_RED_ON);
            last_change_time_ms = current_time_ms;
            yellow_led.on();
            red_led.on();
        }
        break;
    case YELLOW_AND_RED_ON:
        if (current_time_ms - last_change_time_ms > parameters.yellow_red_on_time_ms)
        {
            transitionTo(GREEN_ON);
            last_change_time_ms = current_time_ms;
            red_led.off();
            yellow_led.off();
            green_led.on();
        }
        break;
    case YELLOW_BLINK:
        yellow_led.blink();
        break;
    }
}
