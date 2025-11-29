#include "filters/moving_average.h"
#include <stddef.h>
#include <string.h>

void MovingAverage_Init(MovingAverageFilter_t *filter) {
  if (filter == NULL) {
    return;
  }

  filter->index = 0U;
  filter->count = 0U;
  filter->sum = 0.0f;

  memset(filter->buffer, 0, sizeof(filter->buffer));
}

sensor_value_t MovingAverage_Update(MovingAverageFilter_t *filter,
                                    sensor_value_t new_value) {
  sensor_value_t result;

  if (filter == NULL) {
    return 0.0f;
  }

  filter->sum -= filter->buffer[filter->index];
  filter->buffer[filter->index] = new_value;
  filter->sum += new_value;
  filter->index = (filter->index + 1U) % MOVING_AVG_WINDOW;

  if (filter->count < MOVING_AVG_WINDOW) {
    filter->count++;
  }

  result = filter->sum / (sensor_value_t)filter->count;
  return result;
}

void MovingAverage_Reset(MovingAverageFilter_t *filter) {
  MovingAverage_Init(filter);
}

sensor_value_t MovingAverage_GetValue(const MovingAverageFilter_t *filter) {
  sensor_value_t result;

  if ((filter == NULL) || (filter->count == 0U)) {
    return 0.0f;
  }

  result = filter->sum / (sensor_value_t)filter->count;
  return result;
}
