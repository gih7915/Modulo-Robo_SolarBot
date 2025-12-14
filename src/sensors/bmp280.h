#ifndef BMP280_H
#define BMP280_H

void bmp280_init();
float bmp280_readTemperature();
float bmp280_readPressure();
float bmp280_readAltitude();
void bmp280_printData();

#endif
