//Sensor de Corrente
//Apesar dos testes ele não ficou 100% estável.


#include <Arduino.h>
#include <math.h>
#include "acs758.h"
#include "../config.h"

static float readSensorVoltage() {
    uint32_t sum = 0;
    for (int i = 0; i < ACS758_SAMPLES; i++) {
        sum += analogRead(ACS758_PIN);
    }

    const float avg = static_cast<float>(sum) / static_cast<float>(ACS758_SAMPLES);
    return (avg / ACS758_ADC_MAX) * ACS758_VREF;
}

void acs758_init() {
    Serial.println("\n--- Inicializando ACS758 (corrente) ---");
    pinMode(ACS758_PIN, INPUT);
    analogReadResolution(12);
    analogSetPinAttenuation(ACS758_PIN, ADC_11db);

    Serial.printf("✓ ACS758 pronto no pino ADC %d\n", ACS758_PIN);
    Serial.printf("Calibracao: offset=%.3fV sensibilidade=%.4fV/A\n",
                  ACS758_ZERO_CURRENT_V, ACS758_SENSITIVITY_V_PER_A);
}

float acs758_readCurrentA() {
    const float voltage = readSensorVoltage();
    float current = (voltage - ACS758_ZERO_CURRENT_V) / ACS758_SENSITIVITY_V_PER_A;

    if (fabsf(current) < ACS758_NOISE_THRESHOLD_A) {
        current = 0.0f;
    }

    return current;
}

void acs758_printData() {
    const float voltage = readSensorVoltage();
    const float current = acs758_readCurrentA();

    Serial.println("\n--- Dados ACS758 ---");
    Serial.printf("Tensao sensor: %.3f V\n", voltage);
    Serial.printf("Corrente: %.3f A\n", current);
}
