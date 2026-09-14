#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/gpio.h"
#include "driver/gptimer.h"

#define LED_OUT    GPIO_NUM_16
#define BUTTON_IN  GPIO_NUM_15

static bool IRAM_ATTR timer_alarm_callback(gptimer_handle_t timer, const gptimer_alarm_event_data_t *edata, void *user_data)
{
    gpio_set_level(LED_OUT, !gpio_get_level(LED_OUT));
    return true;
}

void app_main(void) {
    gpio_config_t gpio_led_conf = {
        .pin_bit_mask = 1ULL << LED_OUT,
        .mode = GPIO_MODE_OUTPUT,
        .pull_up_en = GPIO_PULLUP_DISABLE,
        .pull_down_en = GPIO_PULLDOWN_DISABLE,
        .intr_type = GPIO_INTR_DISABLE,
    };

    gpio_config_t gpio_button_conf = {
        .pin_bit_mask = 1ULL << BUTTON_IN,
        .mode = GPIO_MODE_INPUT,
        .pull_up_en = GPIO_PULLUP_ENABLE,
        .pull_down_en = GPIO_PULLDOWN_DISABLE,
        .intr_type = GPIO_INTR_DISABLE,
    };

    gptimer_handle_t timer = NULL;

    gptimer_config_t timer_conf = {
        .clk_src = GPTIMER_CLK_SRC_DEFAULT,
        .direction = GPTIMER_COUNT_UP,
        .resolution_hz = 1000, // 1 kHz
    };

    gptimer_new_timer(&timer_conf, &timer);

    gptimer_alarm_config_t alarm_conf = {
        .alarm_count = 1000, // 1 second
        .reload_count = 0,
        .flags.auto_reload_on_alarm = true,
    };
    gptimer_event_callbacks_t cbs = {
        .on_alarm = timer_alarm_callback,
    };
    gptimer_register_event_callbacks(timer, &cbs, NULL);
    gptimer_set_alarm_action(timer, &alarm_conf);

    gpio_set_level(LED_OUT, 0);

    gpio_config(&gpio_led_conf);
    gpio_config(&gpio_button_conf);

    while (1) {
        bool btn_state = gpio_get_level(BUTTON_IN);

        gpio_set_level(LED_OUT, !btn_state);

        // gpio_set_level(LED_OUT, 1);
        // vTaskDelay(200 / portTICK_PERIOD_MS);
        // gpio_set_level(LED_OUT, 0);
        vTaskDelay(100 / portTICK_PERIOD_MS);
    }
}
