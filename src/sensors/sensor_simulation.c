#include "sensors/sensor_simulation.h"
#include "common/config.h"
#include <math.h>
#include <stdlib.h>
#include <time.h>

static sensor_value_t generate_noise(sensor_value_t amplitude) {
  float u1 = (float)rand() / (float)RAND_MAX;
  float u2 = (float)rand() / (float)RAND_MAX;

  if (u1 < 0.0001f) {
    u1 = 0.0001f;
  }

  return amplitude * sqrtf(-2.0f * logf(u1)) * cosf(2.0f * M_PI * u2);
}

void SensorSimulator_Init(void) { srand((unsigned int)time(NULL)); }

sensor_value_t SensorSimulator_GetTemperature(timestamp_ms_t time_ms) {
  sensor_value_t base_temp;
  sensor_value_t variation;
  sensor_value_t noise;
  sensor_value_t result;

  base_temp = 20.0f + ((float)time_ms / 1000.0f) * 0.5f;

  if (base_temp > 95.0f) {
    base_temp = 95.0f;
  }

  variation = 3.0f * sinf((float)time_ms / 5000.0f);
  noise = generate_noise(1.5f);
  result = base_temp + variation + noise;

  return result;
}

sensor_value_t SensorSimulator_GetOilPressure(timestamp_ms_t time_ms,
                                              sensor_value_t rpm,
                                              sensor_value_t temperature) {
  sensor_value_t base_pressure;
  sensor_value_t temp_factor;
  sensor_value_t pulsation;
  sensor_value_t noise;
  sensor_value_t pressure;

  base_pressure = 1.0f + (rpm / 1000.0f) * 0.5f;

  temp_factor = 1.0f - (temperature - 20.0f) / 200.0f;
  if (temp_factor < 0.5f) {
    temp_factor = 0.5f;
  }

  base_pressure *= temp_factor;
  pulsation = 0.2f * sinf((float)time_ms / 100.0f);
  noise = generate_noise(0.15f);
  pressure = base_pressure + pulsation + noise;

  if (pressure < OIL_PRESSURE_MIN) {
    pressure = OIL_PRESSURE_MIN;
  }
  if (pressure > OIL_PRESSURE_MAX) {
    pressure = OIL_PRESSURE_MAX;
  }

  return pressure;
}

sensor_value_t SensorSimulator_GetRPM(timestamp_ms_t time_ms) {
  sensor_value_t rpm;
  sensor_value_t noise;

  rpm = (float)RPM_IDLE +
        2000.0f * (0.5f + 0.5f * sinf((float)time_ms / 3000.0f));
  noise = generate_noise(30.0f);
  rpm += noise;

  if (rpm < (float)RPM_MIN) {
    rpm = (float)RPM_MIN;
  }
  if (rpm > (float)RPM_MAX) {
    rpm = (float)RPM_MAX;
  }

  return rpm;
}
