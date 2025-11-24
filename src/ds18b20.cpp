#include <Arduino.h>
#include <OneWire.h>
#include <DallasTemperature.h>
#include "ds18b20.h"

#define ONE_WIRE_BUS 4

OneWire oneWire(ONE_WIRE_BUS);
DallasTemperature sensors(&oneWire);
DeviceAddress sensorAddress;

void ds18_init() {
    Serial.println("\n--- Inicializando DS18B20 ---");

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
}

float ds18_readTemperature() {
    sensors.requestTemperatures();
    return sensors.getTempC(sensorAddress);
}

void ds18_printTemperature() {
    float t = ds18_readTemperature();
    if (t == DEVICE_DISCONNECTED_C) {
        Serial.println("❌ DS18B20 desconectado!");
        return;
    }

    Serial.printf("Temperatura: %.2f °C\n", t);
}
