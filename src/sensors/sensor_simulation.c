#include "sensors/sensor_simulation.h"
#include "common/config.h"
#include <math.h>
#include <stdlib.h>
#include <time.h>

static sensor_value_t generate_noise(sensor_value_t amplitude) {
  float u1 = (float)rand() / (float)RAND_MAX;
  float u2 = (float)rand() / (float)RAND_MAX;

  if (u1 < 0.0001f) {
    u1 = 0.0001f;
  }

  return amplitude * sqrtf(-2.0f * logf(u1)) * cosf(2.0f * M_PI * u2);
}
