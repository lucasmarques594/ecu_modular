#ifndef SENSOR_SIMULATOR_H
#define SENSOR_SIMULATOR_H

#include "../common/types.h"

void SensorSimulator_Init(void);
sensor_value_t SensorSimulator_GetTemperature(timestamp_ms_t time_ms);
sensor_value_t SensorSimulator_GetOilPressure(timestamp_ms_t time_ms,
                                              sensor_value_t rpm,
                                              sensor_value_t temperature);
sensor_value_t SensorSimulator_GetRPM(timestamp_ms_t time_ms);

#endif