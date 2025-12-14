#include <Arduino.h>
#include <Wire.h>
#include "mpu6050.h"
#include "../config.h"

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

#if SIMULATE_SENSORS
    Serial.println("⚠️  MODO SIMULAÇÃO ATIVO - Dados fictícios");
    Serial.println("Sensor simulado: MPU6050 Virtual");
#else
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
#endif
}

MPU6050_Data mpu6050_read() {
    MPU6050_Data data;

#if SIMULATE_SENSORS
    // Simula dados do acelerômetro (pequenas variações)
    data.accel_x = random(-2000, 2000);
    data.accel_y = random(-2000, 2000);
    data.accel_z = 16384 + random(-1000, 1000); // ~1g no eixo Z
    
    // Simula dados do giroscópio (pequenas variações)
    data.gyro_x = random(-100, 100);
    data.gyro_y = random(-100, 100);
    data.gyro_z = random(-100, 100);
    
    // Simula temperatura
    data.temperature = 25.0 + (random(-50, 50) / 10.0);
#else
    mpu.readAccel(data.accel_x, data.accel_y, data.accel_z);
    mpu.readGyro(data.gyro_x, data.gyro_y, data.gyro_z);
    
    int16_t temp_raw = mpu.readTemp();
    data.temperature = temp_raw / 340.0 + 36.53;
#endif

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

#if SIMULATE_SENSORS
    Serial.println("(Dados simulados)");
#endif
}
