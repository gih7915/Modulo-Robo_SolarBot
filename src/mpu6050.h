#ifndef MPU6050_H
#define MPU6050_H

#include <stdint.h>

struct MPU6050_Data {
    int16_t accel_x;
    int16_t accel_y;
    int16_t accel_z;
    int16_t gyro_x;
    int16_t gyro_y;
    int16_t gyro_z;
    float temperature;
};

void mpu6050_init();
MPU6050_Data mpu6050_read();
void mpu6050_printData();

#endif
