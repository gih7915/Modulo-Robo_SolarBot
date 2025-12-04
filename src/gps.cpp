#include "gps.h"
#include "config.h"
//ordem dos pinos: GND, VCC, RX, TX
TinyGPSPlus gps;

#if SIMULATE_SENSORS
// Dados simulados para GPS
static float simLat = -16.078678;  // Coordenadas personalizadas
static float simLng = -47.998907;
static int simSats = 8;
static bool simFix = true;
static unsigned long simStartTime = 0;
#endif

static const int RXPin = 16;
static const int TXPin = 17;
static const uint32_t GPSBaud = 9600;

void gps_init() {
    Serial.println("\nInicializando GPS...");
#if SIMULATE_SENSORS
    Serial.println("⚠️  MODO SIMULAÇÃO ATIVO - Dados GPS fictícios");
    Serial.println("Localização: Coordenadas personalizadas");
    Serial.printf("Lat: %.6f, Lng: %.6f\n", simLat, simLng);
    Serial.printf("Satélites: %d (fixo)\n", simSats);
    simStartTime = millis();
#else
    Serial2.begin(GPSBaud, SERIAL_8N1, RXPin, TXPin);
#endif
}

void gps_update() {
#if SIMULATE_SENSORS
    // Simula movimento lento (deriva de ~0.0001° a cada 10s)
    static unsigned long lastUpdate = 0;
    if (millis() - lastUpdate > 10000) {
        simLat += (random(-10, 10) / 100000.0);
        simLng += (random(-10, 10) / 100000.0);
        lastUpdate = millis();
    }
    
    // Gera sentença NMEA simulada para alimentar o parser
    unsigned long elapsed = (millis() - simStartTime) / 1000;
    int hours = (elapsed / 3600) % 24;
    int minutes = (elapsed / 60) % 60;
    int seconds = elapsed % 60;
    
    char nmea[128];
    // $GPGGA,HHMMSS.00,DDMM.MMMM,N,DDDMM.MMMM,W,1,08,1.0,760.0,M,0.0,M,,*checksum
    int latDeg = (int)abs(simLat);
    float latMin = (abs(simLat) - latDeg) * 60.0;
    int lngDeg = (int)abs(simLng);
    float lngMin = (abs(simLng) - lngDeg) * 60.0;
    
    snprintf(nmea, sizeof(nmea), 
             "$GPGGA,%02d%02d%02d.00,%02d%07.4f,%c,%03d%07.4f,%c,1,%02d,1.0,760.0,M,0.0,M,,*00\r\n",
             hours, minutes, seconds,
             latDeg, latMin, simLat >= 0 ? 'N' : 'S',
             lngDeg, lngMin, simLng >= 0 ? 'E' : 'W',
             simSats);
    
    for (int i = 0; nmea[i] != '\0'; i++) {
        gps.encode(nmea[i]);
    }
#else
    while (Serial2.available() > 0) {
        gps.encode(Serial2.read());
    }
#endif
}

void gps_printStatus() {
#if SIMULATE_SENSORS
    Serial.println("====== GPS (SIMULADO) ======");
#else
    Serial.println("====== GPS ======");
#endif

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
