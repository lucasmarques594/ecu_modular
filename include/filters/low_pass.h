#ifndef LOW_PASS_H
#define LOW_PASS_H

#include "../common/types.h"

typedef struct {
  sensor_value_t previous_output;
  sensor_value_t alpha;
} LowPassFilter_t;

void LowPass_Init(LowPassFilter_t *filter, sensor_value_t alpha);
sensor_value_t LowPass_Update(LowPassFilter_t *filter,
                              sensor_value_t new_value);
void LowPass_Reset(LowPassFilter_t *filter);
sensor_value_t LowPass_GetValue(const LowPassFilter_t *filter);
void LowPass_SetAlpha(LowPassFilter_t *filter, sensor_value_t alpha);

#endif