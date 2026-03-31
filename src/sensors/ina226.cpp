//Sensor INA226
//Utilizando divisor de tensão (10k 10k 10k)

#include <Arduino.h>
#include <Wire.h>
#include <INA226.h>
#include "ina226.h"
#include "../config.h"

// Endereço padrão do INA226
static INA226 ina226(0x40);
static bool inaReady = false;

void ina226_init() {
    Serial.println("\n--- Inicializando INA226 (tensão) ---");
    // Inicializa diretamente o driver, configurando os pinos I2C padrão
    if (!ina226.begin(21, 22)) {
        Serial.println("❌ INA226 não encontrado!");
        Serial.println("Verifique as conexões:");
        Serial.println("  VCC -> 3.3V");
        Serial.println("  GND -> GND");
        Serial.println("  SCL -> GPIO 22");
        Serial.println("  SDA -> GPIO 21");
        Serial.println("  ADDR -> GND (0x40) ou conforme configuração");
        inaReady = false;
        return;
    }

    // Configura para leituras estáveis somente de tensão de barramento
    ina226.setAverage(2);                 // 16 amostras
    ina226.setBusVoltageConversionTime(4); // ~1.1 ms (padrão)
    ina226.setModeBusContinuous();         // modo contínuo BV

    inaReady = true;
    Serial.println("✓ INA226 pronto para medir tensão do barramento");
}

float ina226_readBusVoltage() {
    if (!inaReady) {
        return NAN;
    }
    float v = ina226.getBusVoltage();
    if (isnan(v)) {
        return NAN;
    }
    return v * INA226_BUS_DIVIDER;
}

void ina226_printData() {
    float v = ina226_readBusVoltage();
    Serial.println("\n--- Dados INA226 ---");
    if (isnan(v)) {
        Serial.println("Tensão indisponível");
    } else {
        Serial.printf("Tensão real (ajustada x%.2f): %.3f V\n", INA226_BUS_DIVIDER, v);
        Serial.printf("Leitura direta (dividida): %.3f V\n", v / INA226_BUS_DIVIDER);
    }
}
