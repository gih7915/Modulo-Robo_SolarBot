#include <Arduino.h>
#include "gps.h"
#include "ds18b20.h"

unsigned long lastUpdate = 0;

void setup() {
    Serial.begin(115200);
    delay(1000);

    gps_init();
    ds18_init();
}

void loop() {
    gps_update();

    if (millis() - lastUpdate > 1000) {
        lastUpdate = millis();
        gps_printStatus();
        ds18_printTemperature();
    }
}
