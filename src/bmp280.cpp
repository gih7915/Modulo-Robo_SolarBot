#include <Arduino.h>
#include <Wire.h>
#include <Adafruit_BMP280.h>
#include "bmp280.h"
#include "config.h"

Adafruit_BMP280 bmp; // I2C

void bmp280_init() {
    Serial.println("\n--- Inicializando BMP280 ---");

#if SIMULATE_SENSORS
    Serial.println("⚠️  MODO SIMULAÇÃO ATIVO - Dados fictícios");
    Serial.println("Sensor simulado: BMP280 Virtual");
    Serial.println("Temperatura: 20-30°C (variável)");
    Serial.println("Pressão: 980-1020 hPa");
#else
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
#endif
}

float bmp280_readTemperature() {
#if SIMULATE_SENSORS
    // Simula temperatura variando entre 20-30°C
    static unsigned long lastChange = 0;
    static float baseTemp = 25.0;
    
    if (millis() - lastChange > 5000) {
        baseTemp = 20.0 + (random(0, 1000) / 100.0); // 20.00 - 30.00°C
        lastChange = millis();
    }
    
    float variation = (sin(millis() / 10000.0) * 2.0); // ±2°C variation
    return baseTemp + variation;
#else
    return bmp.readTemperature();
#endif
}

float bmp280_readPressure() {
#if SIMULATE_SENSORS
    // Simula pressão entre 980-1020 hPa
    float basePressure = 1000.0;
    float variation = (sin(millis() / 15000.0) * 20.0); // ±20 hPa
    return basePressure + variation;
#else
    return bmp.readPressure() / 100.0; // Converte Pa para hPa
#endif
}

float bmp280_readAltitude() {
#if SIMULATE_SENSORS
    // Simula altitude baseada na pressão simulada
    float pressure = bmp280_readPressure();
    return 44330.0 * (1.0 - pow(pressure / 1013.25, 0.1903));
#else
    return bmp.readAltitude(1013.25); // Pressão ao nível do mar padrão
#endif
}

void bmp280_printData() {
    float temp = bmp280_readTemperature();
    float pressure = bmp280_readPressure();
    float altitude = bmp280_readAltitude();

    Serial.println("\n--- Dados BMP280 ---");
    Serial.printf("Temperatura: %.2f °C\n", temp);
    Serial.printf("Pressão: %.2f hPa\n", pressure);
    Serial.printf("Altitude: %.2f m\n", altitude);

#if SIMULATE_SENSORS
    Serial.println("(Dados simulados)");
#endif
}
