#ifndef SENSOR_H
#define SENSOR_H

#include "../alerts/alert.h"
#include "../common/types.h"
#include "../filters/low_pass.h"
#include "../filters/moving_average.h"

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

#endif