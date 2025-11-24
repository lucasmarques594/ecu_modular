#ifndef TYPES_H
#define TYPES_H

#include <stdbool.h>
#include <stdint.h>

typedef enum {
  ALERT_NONE = 0,
  ALERT_WARNING = 1,
  ALERT_CRITICAL = 2
} AlertLevel_t;

typedef float sensor_value_t;
typedef uint32_t counter_t;
typedef uint32_t timestamp_ms_t;

#endif
