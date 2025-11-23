#ifndef ALERT_H
#define ALERT_H

#include "../common/config.h"
#include "../common/types.h"

typedef struct {
  AlertLevel_t current_level;
  AlertLevel_t pending_level;
  counter_t counter;
} AlertDebounce_t;

#endif