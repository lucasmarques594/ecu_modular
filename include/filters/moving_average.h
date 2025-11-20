#ifndef MOVING_AVERAGE_H
#define MOVING_AVERAGE_H

#include "../common/config.h"
#include "../common/types.h"

typedef struct {
  sensor_valuer_t values[MOVING_AVG_WINDOW];
  counter_t index;
  sensor_valuer_t sum;
} MovingAverageFilter_t;

#endif