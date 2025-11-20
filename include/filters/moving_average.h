#ifndef MOVING_AVERAGE_H
#define MOVING_AVERAGE_H

#include "../common/config.h"
#include "../common/types.h"

typedef struct {
  sensor_valuer_t values[MOVING_AVG_WINDOW];
  counter_t index;
  sensor_valuer_t sum;
} MovingAverageFilter_t;
void MovingAverageFilter_Init(MovingAverageFilter_t *filter);
sensor_valuer_t MovingAverageFilter_Apply(MovingAverageFilter_t *filter,
                                          sensor_valuer_t value);
void MovingAverageFilter_Reset(MovingAverageFilter_t *filter);
sensor_valuer_t MovingAvarage_GetValue(const MovingAverageFilter_t *filter);

#endif