#include "alerts/alert.h"
#include <stddef.h>

void Alert_DebounceInit(AlertDebounce_t *debounce) {
  if (debounce == NULL) {
    return;
  }

  debounce->current_level = ALERT_NONE;
  debounce->pending_level = ALERT_NONE;
  debounce->counter = 0U;
}

AlertLevel_t Alert_DebounceUpdate(AlertDebounce_t *debounce,
                                  AlertLevel_t new_level) {
  if (debounce == NULL) {
    return ALERT_NONE;
  }

  if (new_level == debounce->pending_level) {
    if (debounce->counter < DEBOUNCE_COUNT) {
      debounce->counter++;
    }

    if (debounce->counter >= DEBOUNCE_COUNT) {
      debounce->current_level = debounce->pending_level;
    }
  } else {
    debounce->pending_level = new_level;
    debounce->counter = 0U;
  }

  return debounce->current_level;
}

AlertLevel_t Alert_GetCurrentLevel(const AlertDebounce_t *debounce) {
  if (debounce == NULL) {
    return ALERT_NONE;
  }

  return debounce->current_level;
}

void Alert_DebounceReset(AlertDebounce_t *debounce) {
  Alert_DebounceInit(debounce);
}

AlertLevel_t Alert_DetermineLevel(sensor_value_t value, sensor_value_t warning,
                                  sensor_value_t critical, bool is_low_alert) {
  AlertLevel_t level = ALERT_NONE;

  if (is_low_alert) {
    if (value <= critical) {
      level = ALERT_CRITICAL;
    } else if (value <= warning) {
      level = ALERT_WARNING;
    } else {
      level = ALERT_NONE;
    }
  } else {
    if (value >= critical) {
      level = ALERT_CRITICAL;
    } else if (value >= warning) {
      level = ALERT_WARNING;
    } else {
      level = ALERT_NONE;
    }
  }

  return level;
}

const char *Alert_LevelToString(AlertLevel_t level) {
  const char *str;

  switch (level) {
  case ALERT_NONE:
    str = "OK";
    break;
  case ALERT_WARNING:
    str = "WARNING";
    break;
  case ALERT_CRITICAL:
    str = "CRITICAL";
    break;
  default:
    str = "UNKNOWN";
    break;
  }

  return str;
}