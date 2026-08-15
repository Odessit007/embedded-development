#include <Arduino.h>

#define ADC_IN 4
#define ADC_MAX 4095.0f
#define U_REF 3100.0f


void setup() {
    Serial.begin(115200);
    analogReadResolution(12);
    pinMode(ADC_IN, INPUT);

    Serial.println();
    Serial.println("ADP_RAW | U_calc | U_ADP | Похибка, %");
}

void loop() {
    const float adp_raw = analogRead(ADC_IN) / 1000.0f;
    const float volatgeCalculated = (adp_raw / ADC_MAX) * U_REF;
    const float voltageReadMilliVolts = analogReadMilliVolts(ADC_IN) / 1000.0f;
    const float delta = ((volatgeCalculated - voltageReadMilliVolts) / voltageReadMilliVolts) * 100.0f;

    Serial.printf("%3.2fV | %3.2fV | %3.2fV | %3.2f%%\n", adp_raw, volatgeCalculated, voltageReadMilliVolts, delta);

    delay(100);
}
