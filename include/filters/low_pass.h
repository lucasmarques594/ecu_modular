#ifndef LOW_PASS_H
#define LOW_PASS_H

#include "../common/types.h"

typedef struct {
  sensor_value_t previous_output;
  sensor_value_t alpha;
} LowPassFilter_t;

#endif