#ifndef INA226_SENSOR_H
#define INA226_SENSOR_H

void ina226_init();
float ina226_readBusVoltage();
void ina226_printData();

#endif
