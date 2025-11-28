#include "../../include/sensors/sensor.h"
#include <stddef.h>
#include <stdio.h>

void Sensor_Init(Sensor_t *sensor, const char *name, sensor_value_t warning,
                 sensor_value_t critical, bool is_low_alert,
                 sensor_value_t alpha) {
  if (sensor == NULL) {
    return;
  }

  sensor->name = name;
  sensor->warning_threshold = warning;
  sensor->critical_threshold = critical;
  sensor->is_low_alert = is_low_alert;

  sensor->raw_value = 0.0f;
  sensor->filtered_ma = 0.0f;
  sensor->filtered_lp = 0.0f;

  MovingAverage_Init(&sensor->ma_filter);
  LowPass_Init(&sensor->lp_filter, alpha);
  Alert_DebounceInit(&sensor->alert_debounce);
}

void Sensor_Update(Sensor_t *sensor, sensor_value_t raw_value) {
  AlertLevel_t new_alert_level;

  if (sensor == NULL) {
    return;
  }

  sensor->raw_value = raw_value;
  sensor->filtered_ma = MovingAverage_Update(&sensor->ma_filter, raw_value);
  sensor->filtered_lp = LowPass_Update(&sensor->lp_filter, raw_value);

  new_alert_level =
      Alert_DetermineLevel(sensor->filtered_ma, sensor->warning_threshold,
                           sensor->critical_threshold, sensor->is_low_alert);

  (void)Alert_DebounceUpdate(&sensor->alert_debounce, new_alert_level);
}

AlertLevel_t Sensor_GetAlertLevel(const Sensor_t *sensor) {
  if (sensor == NULL) {
    return ALERT_NONE;
  }

  return Alert_GetCurrentLevel(&sensor->alert_debounce);
}

sensor_value_t Sensor_GetRawValue(const Sensor_t *sensor) {
  if (sensor == NULL) {
    return 0.0f;
  }

  return sensor->raw_value;
}

sensor_value_t Sensor_GetFilteredMA(const Sensor_t *sensor) {
  if (sensor == NULL) {
    return 0.0f;
  }

  return sensor->filtered_ma;
}

sensor_value_t Sensor_GetFilteredLP(const Sensor_t *sensor) {
  if (sensor == NULL) {
    return 0.0f;
  }

  return sensor->filtered_lp;
}

void Sensor_Print(const Sensor_t *sensor) {
  if (sensor == NULL) {
    return;
  }

  printf("%-20s | Raw: %7.2f | MA: %7.2f | LP: %7.2f | Status: %-8s\n",
         sensor->name, sensor->raw_value, sensor->filtered_ma,
         sensor->filtered_lp,
         Alert_LevelToString(Sensor_GetAlertLevel(sensor)));
}

void Sensor_Reset(Sensor_t *sensor) {
  if (sensor == NULL) {
    return;
  }

  sensor->raw_value = 0.0f;
  sensor->filtered_ma = 0.0f;
  sensor->filtered_lp = 0.0f;

  MovingAverage_Reset(&sensor->ma_filter);
  LowPass_Reset(&sensor->lp_filter);
  Alert_DebounceReset(&sensor->alert_debounce);
}