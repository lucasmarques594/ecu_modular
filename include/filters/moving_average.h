#ifndef MOVING_AVERAGE_H
#define MOVING_AVERAGE_H

#include "common/config.h"
#include "common/types.h"

typedef struct {
  sensor_value_t buffer[MOVING_AVG_WINDOW]; // era 'values'
  counter_t index;
  counter_t count; // estava faltando
  sensor_value_t sum;
} MovingAverageFilter_t;

void MovingAverage_Init(MovingAverageFilter_t *filter);
sensor_value_t MovingAverage_Update(MovingAverageFilter_t *filter,
                                    sensor_value_t new_value);
void MovingAverage_Reset(MovingAverageFilter_t *filter);
sensor_value_t MovingAverage_GetValue(const MovingAverageFilter_t *filter);

#endif