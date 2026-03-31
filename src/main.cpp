#include <Arduino.h>
#include "sensors/gps.h"
#include "sensors/bmp280.h"
#include "sensors/mpu6050.h"
#include "sensors/ina226.h"
#include "sensors/acs758.h"
#include "network/web_server.h"
#include "storage/sd_card.h"

unsigned long lastPublish = 0;
unsigned long lastSdLog = 0;
const unsigned long SD_LOG_INTERVAL = 5000; // Log no SD a cada 5 segundos

void setup() {
    Serial.begin(115200);
    delay(500);
    Serial.println("Boot...");

    // Inicializa o cartão SD
    if (sd_init()) {
        Serial.println("✓ Cartão SD pronto para uso");
        
        // Executa testes completos (comentar após os testes iniciais)
        sd_testCardFunctions();
    } else {
        Serial.println("⚠ Sistema continuará sem cartão SD");
    }

    gps_init();
    bmp280_init();
    mpu6050_init();
    ina226_init();
    acs758_init();
    webserver_begin();
}

void loop() {
    gps_update();

    if (millis() - lastPublish >= 1000) {
        lastPublish = millis();
        float t = bmp280_readTemperature();
        float v = ina226_readBusVoltage();
        float c = acs758_readCurrentA();
        log_measurement(t, v, c);
        // Debug serial opcional
        gps_printStatus();
        bmp280_printData();
        mpu6050_printData();
        ina226_printData();
        acs758_printData();
    }

    // Log periódico no cartão SD
    if (sd_isAvailable() && millis() - lastSdLog >= SD_LOG_INTERVAL) {
        lastSdLog = millis();
        
        float temp = bmp280_readTemperature();
        float pressure = bmp280_readPressure();
        double lat = gps.location.lat();
        double lon = gps.location.lng();
        int sats = gps.satellites.value();
        float alt = gps.altitude.meters();
        
        float voltage = ina226_readBusVoltage();
        float current = acs758_readCurrentA();
        if (sd_logSensorData(temp, voltage, current, lat, lon, sats, alt)) {
            Serial.println("✓ Dados salvos no SD");
        }
    }
}
