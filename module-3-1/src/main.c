#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_adc/adc_cali.h"
#include "esp_adc/adc_cali_scheme.h"
#include "esp_adc/adc_oneshot.h"
#include "driver/gpio.h"
#include "driver/gptimer.h"

#define LED_OUT		        GPIO_NUM_16
#define ADC_CHANNEL	        ADC_CHANNEL_3  // ADC1_CH3: GPIO4
#define SMA_N               5              // Number of samples for averaging

#define DARK_THRESHOLD      2200           // < 2200 mV - enough darkness to turn the LED ON
#define LIGHT_THRESHOLD     2800           // > 2800 mV - enough brightness to turn the LED OFF
// 2200..2800 mV - hysteresis band, no change in LED state

void app_main() {
    esp_err_t err = ESP_OK;

    adc_oneshot_unit_handle_t adc_handle;
    adc_oneshot_unit_init_cfg_t adc_unit_config = {};
    adc_unit_config.unit_id = ADC_UNIT_1;
    err = adc_oneshot_new_unit(&adc_unit_config, &adc_handle);
    if (err != ESP_OK) {
        printf("Failed to initialize ADC, err = %d\n", err);
        return;
    }

    adc_oneshot_chan_cfg_t adc_channel_config = {};
    adc_channel_config.bitwidth = ADC_BITWIDTH_DEFAULT;
    adc_channel_config.atten = ADC_ATTEN_DB_12;
    err = adc_oneshot_config_channel(adc_handle, ADC_CHANNEL, &adc_channel_config);
    if (err != ESP_OK) {
        printf("Failed to configure ADC channel, err = %d\n", err);
        return;
    }

    adc_cali_handle_t calibration_handle = nullptr;
    adc_cali_curve_fitting_config_t calibration_config = {};
    calibration_config.unit_id = ADC_UNIT_1;
    calibration_config.chan = ADC_CHANNEL;
    calibration_config.atten = ADC_ATTEN_DB_12;
    calibration_config.bitwidth = ADC_BITWIDTH_DEFAULT;
    adc_cali_create_scheme_curve_fitting(&calibration_config, &calibration_handle);

    // Налаштування структури GPIO для LED
    gpio_config_t gpio_led_conf = {};
    gpio_led_conf.pin_bit_mask = 1ULL << LED_OUT;
    gpio_led_conf.mode = GPIO_MODE_OUTPUT;
    gpio_led_conf.pull_up_en = GPIO_PULLUP_DISABLE;
    gpio_led_conf.pull_down_en = GPIO_PULLDOWN_DISABLE;
    gpio_led_conf.intr_type = GPIO_INTR_DISABLE;

    // Конфігурація GPIO
    gpio_config(&gpio_led_conf);

    // Встановлення початкового стану
    gpio_set_level(LED_OUT, 0);

    uint32_t samples[SMA_N] = {0};  // Circular buffer to hold the last SMA_N samples
    uint8_t sample_index = 0;        // Current position in the circular buffer
    uint64_t sma_sum = 0;            // Running sum of the last SMA_N samples for averaging
    bool buffer_filled = false;      // Tracks whether the buffer has reached SMA_N samples

    while (1) {
        static bool led_state = 0;
        int adc_raw = 0;
        int voltage_mv = 0;

        err = adc_oneshot_read(adc_handle, ADC_CHANNEL, &adc_raw);
        if (err == ESP_OK) {
            adc_cali_raw_to_voltage(calibration_handle, adc_raw, &voltage_mv);
            printf("ADC GPIO4: raw=%d, voltage=%d mV\n", adc_raw, voltage_mv);
        } else {
            printf("Failed to read ADC, err = %d\n", err);
        }

        if (buffer_filled) {
            sma_sum -= samples[sample_index];
            samples[sample_index] = voltage_mv;
            sma_sum += voltage_mv;
            sample_index = (sample_index + 1) % SMA_N;
        } else {
            samples[sample_index] = voltage_mv;
            sma_sum += voltage_mv;
            sample_index++;

            if (sample_index == SMA_N) {
                buffer_filled = true;
                sample_index = 0;
            }
        }

        if (buffer_filled) {
            uint64_t sma = sma_sum / SMA_N;
            printf("SMA: %llu mV\n", sma);

            if (sma < DARK_THRESHOLD && !led_state) {
                led_state = true;
                gpio_set_level(LED_OUT, led_state);
                printf("Darkness detected, LED turned ON\n");
            } else if (sma > LIGHT_THRESHOLD && led_state) {
                led_state = false;
                gpio_set_level(LED_OUT, led_state);
                printf("Light detected, LED turned OFF\n");
            }
        }

        vTaskDelay(pdMS_TO_TICKS(200));
    }
}
