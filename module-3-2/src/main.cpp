#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_log.h"
#include "adc.h"

#define ADC_INPUT_CHANNEL ADC_CHANNEL_3
#define MEASUREMENT_PERIOD_MS 100
#define MONITOR_INIT_DELAY_MS 6000

extern "C" void app_main(void) {
    vTaskDelay(pdMS_TO_TICKS(MONITOR_INIT_DELAY_MS));

    adc_drv_channel_ctx_t adc_channel;
    esp_err_t err = adc_drv_channel_init(&adc_channel,
                                         ADC_UNIT_1,
                                         ADC_INPUT_CHANNEL,
                                         ADC_ATTEN_DB_12,
                                         ADC_BITWIDTH_DEFAULT,
                                         true);
    if (err != ESP_OK) {
        printf("ADC initialization failed: %s\n", esp_err_to_name(err));
        return;
    }

    printf("RAW   U_manual(mV)   U_cali(mV)   Error(%%)\n");
    printf("----------------------------------------------\n");

    while (1) {
        // Read raw result
        int raw = adc_drv_channel_read_raw(&adc_channel);
        if (raw < 0) {
            printf("ADC read failed\n");
            continue;
        }

        // Read calibrated voltage
        int calibrated_mv = 0;
        err = adc_drv_cali_raw_to_voltage(&adc_channel.cali_ctx, raw, &calibrated_mv);
        if (err != ESP_OK) {
            printf("ADC calibration failed: %s\n", esp_err_to_name(err));
            continue;
        }

        int manual_mv = adc_drv_cali_estimate_voltage(raw, adc_channel.atten, adc_channel.bitwidth);
        double error_percent = calibrated_mv > 0
            ? abs(manual_mv - calibrated_mv) * 100.0 / calibrated_mv
            : 0.0;

        printf("%4d %14d %12d %10.2f\n",
                raw,
                manual_mv,
                calibrated_mv,
                error_percent);

        vTaskDelay(pdMS_TO_TICKS(MEASUREMENT_PERIOD_MS));
    }
}
