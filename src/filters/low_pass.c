#include "filters/low_pass.h"
#include <stddef.h>

void LowPass_Init(LowPassFilter_t *filter, sensor_value_t alpha) {
  if (filter == NULL) {
    return;
  }

  if ((alpha <= 0.0f) || (alpha >= 1.0f)) {
    filter->alpha = 0.2f;
  } else {
    filter->alpha = alpha;
  }

  filter->previous_output = 0.0f;
}

sensor_value_t LowPass_Update(LowPassFilter_t *filter,
                              sensor_value_t new_value) {
  sensor_value_t output;

  if (filter == NULL) {
    return 0.0f;
  }

  output = (filter->alpha * new_value) +
           ((1.0f - filter->alpha) * filter->previous_output);

  filter->previous_output = output;
  return output;
}

void LowPass_Reset(LowPassFilter_t *filter) {
  if (filter == NULL) {
    return;
  }

  filter->previous_output = 0.0f;
}

sensor_value_t LowPass_GetValue(const LowPassFilter_t *filter) {
  if (filter == NULL) {
    return 0.0f;
  }

  return filter->previous_output;
}

void LowPass_SetAlpha(LowPassFilter_t *filter, sensor_value_t alpha) {
  if (filter == NULL) {
    return;
  }

  if ((alpha > 0.0f) && (alpha < 1.0f)) {
    filter->alpha = alpha;
  }
}