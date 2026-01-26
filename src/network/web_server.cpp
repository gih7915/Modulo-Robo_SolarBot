// Limpo e corrigido
#include <Arduino.h>
#include <FS.h>
#include <LittleFS.h>
#include <WiFi.h>
#include <AsyncTCP.h>
#include <ESPAsyncWebServer.h>
#include <ArduinoJson.h>
#include "../sensors/bmp280.h"
#include "../sensors/mpu6050.h"
#include "../sensors/gps.h"
#include "../sensors/ina226.h"
#include "web_server.h"
#include "../config.h"

static AsyncWebServer server(80);
static const char *LOG_FILE = "/logs.csv";
static IPAddress AP_IP(192, 168, 4, 1);
static IPAddress AP_GW(192, 168, 4, 1);
static IPAddress AP_SN(255, 255, 255, 0);

static bool startAP() {
    WiFi.softAPConfig(AP_IP, AP_GW, AP_SN);
    // canal configurável, SSID visível, até 4 conexões
    bool ok = WiFi.softAP(WIFI_AP_SSID, WIFI_AP_PASS, WIFI_AP_CHANNEL, 0, 4);
    if (ok) {
        Serial.print("AP: "); Serial.print(WIFI_AP_SSID);
        Serial.print(" IP: "); Serial.println(WiFi.softAPIP());
        Serial.print("Dashboard (AP): http://"); Serial.println(WiFi.softAPIP());
    } else {
        Serial.println("Falha ao iniciar AP");
    }
    return ok;
}

// Timestamp a partir do GPS (data/hora) ou fallback em millis
static String currentTimestamp() {
    if (gps.date.isValid() && gps.time.isValid()) {
        char buf[25];
        snprintf(
            buf,
            sizeof(buf),
            "%04d-%02d-%02d %02d:%02d:%02d",
            gps.date.year(), gps.date.month(), gps.date.day(),
            gps.time.hour(), gps.time.minute(), gps.time.second()
        );
        return String(buf);
    }
    if (gps.time.isValid()) {
        char buf[16];
        snprintf(buf, sizeof(buf), "%02d:%02d:%02d", gps.time.hour(), gps.time.minute(), gps.time.second());
        return String(buf);
    }
    return String(millis());
}

// Log em CSV (cria cabeçalho se não existir)
void log_measurement(float temperature, float voltage) {
    if (!LittleFS.exists(LOG_FILE)) {
        File f = LittleFS.open(LOG_FILE, "w");
        if (f) {
            f.println("timestamp,temperature,pressure,lat,lng,satellites,fix,voltage");
            f.close();
        }
    }

    File f = LittleFS.open(LOG_FILE, "a");
    if (!f) return;
    String ts = currentTimestamp();
    String line;
    line.reserve(128);
    line += ts; line += ",";
    line += String(temperature, 2);
    line += ",";
    line += String(bmp280_readPressure(), 2);
    line += ",";
    if (gps.location.isValid()) {
        line += String(gps.location.lat(), 6); line += ",";
        line += String(gps.location.lng(), 6);
    } else {
        line += ",";
    }
    line += ",";
    if (gps.satellites.isValid()) {
        line += String(gps.satellites.value());
    }
    line += ",";
    line += gps.location.isValid() ? "1" : "0";
    line += ",";
    if (!isnan(voltage)) {
        line += String(voltage, 3);
    }
    line += "\n";
    f.print(line);
    f.close();
}

static String jsonTemperature() {
    float t = bmp280_readTemperature();
    float p = bmp280_readPressure();
    float alt = bmp280_readAltitude();
    float v = ina226_readBusVoltage();
    DynamicJsonDocument doc(384);
    doc["temperature"] = t;
    doc["pressure"] = p;
    doc["altitude"] = alt;
    doc["ok"] = true;
    if (isnan(v)) {
        doc["voltage"] = nullptr;
    } else {
        doc["voltage"] = v;
    }
    doc["timestamp"] = currentTimestamp();
    String out; serializeJson(doc, out); return out;
}

static String jsonGPS() {
    DynamicJsonDocument doc(384);
    if (gps.location.isValid()) {
        doc["lat"] = gps.location.lat();
        doc["lng"] = gps.location.lng();
        doc["fix"] = true;
    } else {
        doc["lat"] = nullptr;
        doc["lng"] = nullptr;
        doc["fix"] = false;
    }
    doc["satellites"] = gps.satellites.isValid() ? gps.satellites.value() : -1;
    if (gps.time.isValid()) {
        char buf[16];
        snprintf(buf, sizeof(buf), "%02d:%02d:%02d", gps.time.hour(), gps.time.minute(), gps.time.second());
        doc["utc"] = buf;
    } else {
        doc["utc"] = nullptr;
    }
    doc["timestamp"] = currentTimestamp();
    String out; serializeJson(doc, out); return out;
}

static String jsonAll() {
    float t = bmp280_readTemperature();
    float p = bmp280_readPressure();
    float alt = bmp280_readAltitude();
    float v = ina226_readBusVoltage();
    MPU6050_Data mpu_data = mpu6050_read();
    
    DynamicJsonDocument doc(1536);
    doc["temperature"] = t;
    doc["pressure"] = p;
    doc["altitude"] = alt;
    if (isnan(v)) {
        doc["voltage"] = nullptr;
    } else {
        doc["voltage"] = v;
    }
    doc["temperature_ok"] = true;

    JsonObject mpu = doc["mpu6050"].to<JsonObject>();
    mpu["accel_x"] = mpu_data.accel_x;
    mpu["accel_y"] = mpu_data.accel_y;
    mpu["accel_z"] = mpu_data.accel_z;
    mpu["gyro_x"] = mpu_data.gyro_x;
    mpu["gyro_y"] = mpu_data.gyro_y;
    mpu["gyro_z"] = mpu_data.gyro_z;
    mpu["temp"] = mpu_data.temperature;

    JsonObject g = doc["gps"].to<JsonObject>();
    if (gps.location.isValid()) {
        g["lat"] = gps.location.lat();
        g["lng"] = gps.location.lng();
        g["fix"] = true;
    } else {
        g["lat"] = nullptr;
        g["lng"] = nullptr;
        g["fix"] = false;
    }
    g["satellites"] = gps.satellites.isValid() ? gps.satellites.value() : -1;
    if (gps.time.isValid()) {
        char buf[16];
        snprintf(buf, sizeof(buf), "%02d:%02d:%02d", gps.time.hour(), gps.time.minute(), gps.time.second());
        g["utc"] = buf;
    } else {
        g["utc"] = nullptr;
    }
    doc["timestamp"] = currentTimestamp();
    String out; serializeJson(doc, out); return out;
}

static String exportJSON() {
    File f = LittleFS.open(LOG_FILE, "r");
    if (!f) return String("[]");
    // pular cabeçalho
    (void)f.readStringUntil('\n');
    DynamicJsonDocument doc(8192);
    JsonArray arr = doc.to<JsonArray>();
    while (f.available()) {
        String line = f.readStringUntil('\n');
        if (line.length() < 5) continue;
        int idx1 = line.indexOf(',');
        int idx2 = line.indexOf(',', idx1 + 1);
        int idx3 = line.indexOf(',', idx2 + 1);
        int idx4 = line.indexOf(',', idx3 + 1);
        int idx5 = line.indexOf(',', idx4 + 1);
        int idx6 = line.indexOf(',', idx5 + 1);
        int idx7 = line.indexOf(',', idx6 + 1);
        if (idx1 < 0 || idx2 < 0 || idx3 < 0 || idx4 < 0 || idx5 < 0 || idx6 < 0 || idx7 < 0) continue;
        JsonObject o = arr.add<JsonObject>();
        o["timestamp"] = line.substring(0, idx1);
        o["temperature"] = line.substring(idx1 + 1, idx2).toFloat();
        o["pressure"] = line.substring(idx2 + 1, idx3).toFloat();
        String lat = line.substring(idx3 + 1, idx4);
        String lng = line.substring(idx4 + 1, idx5);
        if (lat.length()) o["lat"] = lat.toFloat(); else o["lat"] = nullptr;
        if (lng.length()) o["lng"] = lng.toFloat(); else o["lng"] = nullptr;
        String sats = line.substring(idx5 + 1, idx6);
        o["satellites"] = sats.length() ? sats.toInt() : -1;
        String fix = line.substring(idx6 + 1, idx7);
        fix.trim();
        o["fix"] = (fix == "1");
        String volt = line.substring(idx7 + 1);
        volt.trim();
        if (volt.length()) {
            o["voltage"] = volt.toFloat();
        } else {
            o["voltage"] = nullptr;
        }
    }
    f.close();
    String out; serializeJson(arr, out); return out;
}

void webserver_begin() {
    if (!LittleFS.begin(true)) {
        Serial.println("Falha ao montar LittleFS");
    } else {
        Serial.println("LittleFS montado");
    }

    bool apOk = false;
#if WIFI_USE_STA
    // AP+STA: mantém AP ativo e tenta conectar no STA por até 60s
    WiFi.mode(WIFI_AP_STA);
    apOk = startAP();

    WiFi.setSleep(false);
    WiFi.begin(WIFI_STA_SSID, WIFI_STA_PASS);
    Serial.print("Conectando em "); Serial.println(WIFI_STA_SSID);
    uint32_t t0 = millis();
    while (WiFi.status() != WL_CONNECTED && millis() - t0 < 60000) {
        delay(250);
        Serial.print(".");
    }
    Serial.println();
    if (WiFi.status() == WL_CONNECTED) {
        Serial.print("Conectado (STA). IP: "); Serial.println(WiFi.localIP());
        Serial.print("Dashboard (STA): http://"); Serial.println(WiFi.localIP());
    } else {
        Serial.println("STA não conectou em 60s. Mantendo AP ativo.");
    }
#else
    WiFi.mode(WIFI_AP);
    apOk = startAP();
#endif

    // Configurar rotas de API ANTES dos arquivos estáticos
    server.on("/api/temperature", HTTP_GET, [](AsyncWebServerRequest *req){
        req->send(200, "application/json", jsonTemperature());
    });
    server.on("/api/gps", HTTP_GET, [](AsyncWebServerRequest *req){
        req->send(200, "application/json", jsonGPS());
    });
    server.on("/api/mpu6050", HTTP_GET, [](AsyncWebServerRequest *req){
        MPU6050_Data data = mpu6050_read();
        DynamicJsonDocument doc(384);
        doc["accel_x"] = data.accel_x;
        doc["accel_y"] = data.accel_y;
        doc["accel_z"] = data.accel_z;
        doc["gyro_x"] = data.gyro_x;
        doc["gyro_y"] = data.gyro_y;
        doc["gyro_z"] = data.gyro_z;
        doc["temperature"] = data.temperature;
        String out; serializeJson(doc, out);
        req->send(200, "application/json", out);
    });
    server.on("/api/all", HTTP_GET, [](AsyncWebServerRequest *req){
        req->send(200, "application/json", jsonAll());
    });
    server.on("/export", HTTP_GET, [](AsyncWebServerRequest *req){
        if (!req->hasParam("format")) {
            req->send(400, "application/json", "{\"error\":\"missing format\"}");
            return;
        }
        String fmt = req->getParam("format")->value();
        if (fmt == "csv") {
            File f = LittleFS.open(LOG_FILE, "r");
            if (!f) { req->send(404, "text/plain", "Sem dados"); return; }
            String content = f.readString();
            f.close();
            req->send(200, "text/csv", content);
        } else if (fmt == "json") {
            req->send(200, "application/json", exportJSON());
        } else {
            req->send(400, "application/json", "{\"error\":\"format invalido\"}");
        }
    });
    
    server.on("/api/robot/stats", HTTP_GET, [](AsyncWebServerRequest *req){
        req->send(200, "application/json", jsonAll());
    });

    // Arquivos estáticos POR ÚLTIMO; index.html como default
    server.serveStatic("/", LittleFS, "/").setDefaultFile("index.html");

    server.onNotFound([](AsyncWebServerRequest *req){
        req->send(404, "text/plain", "Not Found");
    });

    server.begin();
    Serial.println("Servidor HTTP iniciado na porta 80");
    // Imprime URLs úteis para acessar o dashboard
    if (WiFi.getMode() == WIFI_AP || WiFi.getMode() == WIFI_AP_STA) {
        Serial.print("Acesse no navegador (AP): http://"); Serial.println(WiFi.softAPIP());
    }
    if (WiFi.status() == WL_CONNECTED) {
        Serial.print("Acesse no navegador (STA): http://"); Serial.println(WiFi.localIP());
    }
}
