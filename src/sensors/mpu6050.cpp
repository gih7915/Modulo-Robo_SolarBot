//Sensor Acelerometro

#include <Arduino.h>
#include <Wire.h>
#include "mpu6050.h"

#define MPU6050_ADDR 0x68
#define PWR_MGMT_1 0x6B
#define ACCEL_XOUT_H 0x3B
#define GYRO_XOUT_H 0x43
#define TEMP_OUT_H 0x41

class MPU6050_Simple {
private:
    uint8_t addr;

public:
    MPU6050_Simple(uint8_t address = MPU6050_ADDR) : addr(address) {}

    bool initialize() {
        Wire.beginTransmission(addr);
        Wire.write(PWR_MGMT_1);
        Wire.write(0x00);
        return Wire.endTransmission() == 0;
    }

    void readAccel(int16_t &x, int16_t &y, int16_t &z) {
        Wire.beginTransmission(addr);
        Wire.write(ACCEL_XOUT_H);
        Wire.endTransmission();
        
        Wire.requestFrom(addr, (uint8_t)6);
        x = (Wire.read() << 8) | Wire.read();
        y = (Wire.read() << 8) | Wire.read();
        z = (Wire.read() << 8) | Wire.read();
    }

    void readGyro(int16_t &x, int16_t &y, int16_t &z) {
        Wire.beginTransmission(addr);
        Wire.write(GYRO_XOUT_H);
        Wire.endTransmission();
        
        Wire.requestFrom(addr, (uint8_t)6);
        x = (Wire.read() << 8) | Wire.read();
        y = (Wire.read() << 8) | Wire.read();
        z = (Wire.read() << 8) | Wire.read();
    }

    int16_t readTemp() {
        Wire.beginTransmission(addr);
        Wire.write(TEMP_OUT_H);
        Wire.endTransmission();
        
        Wire.requestFrom(addr, (uint8_t)2);
        return (Wire.read() << 8) | Wire.read();
    }
};

static MPU6050_Simple mpu;

void mpu6050_init() {
    Serial.println("\n--- Inicializando MPU6050 ---");
    Wire.begin(21, 22);
    delay(100);

    if (!mpu.initialize()) {
        Serial.println("❌ MPU6050 não encontrado!");
        Serial.println("Verifique as conexões:");
        Serial.println("  VCC -> 3.3V");
        Serial.println("  GND -> GND");
        Serial.println("  SCL -> GPIO 22");
        Serial.println("  SDA -> GPIO 21");
        return;
    }

    Serial.println("✓ MPU6050 inicializado com sucesso!");
}

MPU6050_Data mpu6050_read() {
    MPU6050_Data data;
    mpu.readAccel(data.accel_x, data.accel_y, data.accel_z);
    mpu.readGyro(data.gyro_x, data.gyro_y, data.gyro_z);
    
    int16_t temp_raw = mpu.readTemp();
    data.temperature = temp_raw / 340.0 + 36.53;

    return data;
}

void mpu6050_printData() {
    MPU6050_Data data = mpu6050_read();

    Serial.println("\n--- Dados MPU6050 ---");
    Serial.printf("Aceleração:\n");
    Serial.printf("  X: %6d | Y: %6d | Z: %6d\n", data.accel_x, data.accel_y, data.accel_z);
    Serial.printf("Giroscópio:\n");
    Serial.printf("  X: %6d | Y: %6d | Z: %6d\n", data.gyro_x, data.gyro_y, data.gyro_z);
    Serial.printf("Temperatura: %.2f°C\n", data.temperature);
}
