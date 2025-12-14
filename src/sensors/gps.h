#ifndef GPSMODULE_H
#define GPSMODULE_H

#include <Arduino.h>
#include <TinyGPSPlus.h>

extern TinyGPSPlus gps;

void gps_init();
void gps_update();
void gps_printStatus();

#endif
