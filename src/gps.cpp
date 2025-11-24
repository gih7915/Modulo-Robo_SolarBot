#include "gps.h"

TinyGPSPlus gps;

static const int RXPin = 16;
static const int TXPin = 17;
static const uint32_t GPSBaud = 9600;

void gps_init() {
    Serial.println("\nInicializando GPS...");
    Serial2.begin(GPSBaud, SERIAL_8N1, RXPin, TXPin);
}

void gps_update() {
    while (Serial2.available() > 0) {
        gps.encode(Serial2.read());
    }
}

void gps_printStatus() {
    Serial.println("====== GPS ======");

    if (gps.satellites.isValid())
        Serial.printf("Satélites: %d\n", gps.satellites.value());
    else
        Serial.println("Satélites: N/A");

    if (gps.location.isValid()) {
        Serial.printf("Latitude : %.6f\n", gps.location.lat());
        Serial.printf("Longitude: %.6f\n", gps.location.lng());
        Serial.println("STATUS FIX: OK");
    } else {
        Serial.println("Latitude : ---");
        Serial.println("Longitude: ---");
        Serial.println("STATUS FIX: SEM FIX");
    }

    if (gps.time.isValid()) {
        Serial.printf("Hora UTC: %02d:%02d:%02d\n",
            gps.time.hour(),
            gps.time.minute(),
            gps.time.second()
        );
    } else {
        Serial.println("Hora UTC: ---");
    }

    Serial.println("=================\n");
}
