#ifndef SENSOR_H
#define SENSOR_H

#include "alerts/alert.h"
#include "common/types.h"
#include "filters/low_pass.h"
#include "filters/moving_average.h"

typedef struct {
  const char *name;
  sensor_value_t raw_value;
  sensor_value_t filtered_ma;
  sensor_value_t filtered_lp;
  MovingAverageFilter_t ma_filter;
  LowPassFilter_t lp_filter;
  AlertDebounce_t alert_debounce;
  sensor_value_t warning_threshold;
  sensor_value_t critical_threshold;
  bool is_low_alert;
} Sensor_t;

void Sensor_Init(Sensor_t *sensor, const char *name, sensor_value_t warning,
                 sensor_value_t critical, bool is_low_alert,
                 sensor_value_t alpha);

void Sensor_Update(Sensor_t *sensor, sensor_value_t raw_value);
AlertLevel_t Sensor_GetAlertLevel(const Sensor_t *sensor);
sensor_value_t Sensor_GetRawValue(const Sensor_t *sensor);
sensor_value_t Sensor_GetFilteredMA(const Sensor_t *sensor);
sensor_value_t Sensor_GetFilteredLP(const Sensor_t *sensor);
void Sensor_Print(const Sensor_t *sensor);
void Sensor_Reset(Sensor_t *sensor);

#endif