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
