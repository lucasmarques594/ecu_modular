#ifndef ALERT_H
#define ALERT_H

#include "../common/config.h"
#include "../common/types.h"

typedef struct {
  AlertLevel_t current_level;
  AlertLevel_t pending_level;
  counter_t counter;
} AlertDebounce_t;

void Alert_DebounceInit(AlertDebounce_t *debounce);
AlertLevel_t Alert_DebounceUpdate(AlertDebounce_t *debounce,
                                  AlertLevel_t new_level);
AlertLevel_t Alert_GetCurrentLevel(const AlertDebounce_t *debounce);
void Alert_DebounceReset(AlertDebounce_t *debounce);

AlertLevel_t Alert_DetermineLevel(sensor_value_t value, sensor_value_t warning,
                                  sensor_value_t critical, bool is_low_alert);

#endif