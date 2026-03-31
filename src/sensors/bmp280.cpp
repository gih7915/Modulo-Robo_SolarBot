//Sensor de Temperatura e Pressao

#include <Arduino.h>
#include <Wire.h>
#include <Adafruit_BMP280.h>
#include "bmp280.h"

Adafruit_BMP280 bmp; // I2C

void bmp280_init() {
    Serial.println("\n--- Inicializando BMP280 ---");
    if (!bmp.begin(0x76)) {  // Endereço I2C padrão do BMP280
        Serial.println("❌ BMP280 não encontrado!");
        Serial.println("Verifique as conexões:");
        Serial.println("  VCC -> 3.3V");
        Serial.println("  GND -> GND");
        Serial.println("  SCL -> GPIO 22");
        Serial.println("  SDA -> GPIO 21");
        return;
    }

    // Configurações do sensor
    bmp.setSampling(Adafruit_BMP280::MODE_NORMAL,     // Operating Mode
                    Adafruit_BMP280::SAMPLING_X2,     // Temp. oversampling
                    Adafruit_BMP280::SAMPLING_X16,    // Pressure oversampling
                    Adafruit_BMP280::FILTER_X16,      // Filtering
                    Adafruit_BMP280::STANDBY_MS_500); // Standby time

    Serial.println("✓ BMP280 inicializado com sucesso!");
    Serial.print("ID do sensor: 0x");
    Serial.println(bmp.sensorID(), HEX);
}

float bmp280_readTemperature() {
    return bmp.readTemperature();
}

float bmp280_readPressure() {
    return bmp.readPressure() / 100.0; // Converte Pa para hPa
}

float bmp280_readAltitude() {
    return bmp.readAltitude(1013.25); // Pressão ao nível do mar padrão
}

void bmp280_printData() {
    float temp = bmp280_readTemperature();
    float pressure = bmp280_readPressure();
    float altitude = bmp280_readAltitude();

    Serial.println("\n--- Dados BMP280 ---");
    Serial.printf("Temperatura: %.2f °C\n", temp);
    Serial.printf("Pressão: %.2f hPa\n", pressure);
    Serial.printf("Altitude: %.2f m\n", altitude);
}
