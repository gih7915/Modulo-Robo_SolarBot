#include <Arduino.h>
#include "gps.h"
#include "ds18b20.h"
#include "web_server.h"
#include "sd_card.h"

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
    ds18_init();
    webserver_begin();
}

void loop() {
    gps_update();

    if (millis() - lastPublish >= 1000) {
        lastPublish = millis();
        float t = ds18_readTemperature();
        log_measurement(t);
        // Debug serial opcional
        gps_printStatus();
        ds18_printTemperature();
    }

    // Log periódico no cartão SD
    if (sd_isAvailable() && millis() - lastSdLog >= SD_LOG_INTERVAL) {
        lastSdLog = millis();
        
        float temp = ds18_readTemperature();
        double lat = gps.location.lat();
        double lon = gps.location.lng();
        int sats = gps.satellites.value();
        float alt = gps.altitude.meters();
        
        if (sd_logSensorData(temp, lat, lon, sats, alt)) {
            Serial.println("✓ Dados salvos no SD");
        }
    }
}
