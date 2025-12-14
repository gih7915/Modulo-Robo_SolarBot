/*#include <Arduino.h>
#include <OneWire.h>
#include <DallasTemperature.h>
#include "ds18b20.h"
#include "config.h"

#define ONE_WIRE_BUS 4

OneWire oneWire(ONE_WIRE_BUS);
DallasTemperature sensors(&oneWire);
DeviceAddress sensorAddress;

void ds18_init() {
    Serial.println("\n--- Inicializando DS18B20 ---");

#if SIMULATE_SENSORS
    Serial.println("⚠️  MODO SIMULAÇÃO ATIVO - Dados fictícios");
    Serial.println("Sensor simulado: DS18B20 Virtual");
    Serial.println("Temperatura: 20-30°C (variável)");
#else
    sensors.begin();

    int count = sensors.getDeviceCount();
    Serial.printf("Sensores encontrados: %d\n", count);

    if (count == 0) {
        Serial.println("❌ Nenhum sensor encontrado!");
        return;
    }

    sensors.getAddress(sensorAddress, 0);
    sensors.setResolution(sensorAddress, 12);

    Serial.print("Endereço: ");
    for (int i = 0; i < 8; i++) {
        Serial.printf("%02X ", sensorAddress[i]);
    }
    Serial.println();
#endif
}

float ds18_readTemperature() {
#if SIMULATE_SENSORS
    // Simula temperatura variando entre 20-30°C com variação senoidal
    static unsigned long lastChange = 0;
    static float baseTemp = 25.0;
    
    if (millis() - lastChange > 5000) {
        baseTemp = 20.0 + (random(0, 1000) / 100.0); // 20.00 - 30.00°C
        lastChange = millis();
    }
    
    float variation = sin(millis() / 1000.0) * 0.5; // ±0.5°C
    return baseTemp + variation;
#else
    sensors.requestTemperatures();
    return sensors.getTempC(sensorAddress);
#endif
}

void ds18_printTemperature() {
    float t = ds18_readTemperature();
#if !SIMULATE_SENSORS
    if (t == DEVICE_DISCONNECTED_C) {
        Serial.println("❌ DS18B20 desconectado!");
        return;
    }
#endif
    Serial.printf("🌡️  Temperatura: %.2f °C%s\n", t, SIMULATE_SENSORS ? " (simulado)" : "");
}*/
