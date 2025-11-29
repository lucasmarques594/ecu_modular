#include "common/config.h"
#include "common/types.h"
#include "sensors/sensor.h"
#include "sensors/sensor_simulation.h"
#include <stdio.h>

static void print_header(void) {
  printf("=================================================================\n");
  printf("     ECU Sensor Monitor - Sistema de Monitoramento v1.0\n");
  printf("          Arquitetura Modular - MISRA-C Compliant\n");
  printf("================================================================="
         "\n\n");
}

static void print_system_info(void) {
  printf("Sensores inicializados:\n");
  printf("  - Temperatura: Warning >%d°C, Critical >%d°C\n", TEMP_WARNING,
         TEMP_CRITICAL);
  printf("  - Pressão Óleo: Warning <%.1f bar, Critical <%.1f bar\n",
         OIL_PRESSURE_WARNING, OIL_PRESSURE_CRITICAL);
  printf("  - RPM: Redline >%u, Max >%u\n\n", RPM_REDLINE, RPM_MAX);

  printf("Configuração do sistema:\n");
  printf("  - Taxa de amostragem: %u Hz\n", SAMPLE_RATE_HZ);
  printf("  - Janela média móvel: %u amostras\n", MOVING_AVG_WINDOW);
  printf("  - Alpha passa-baixa: %.2f\n", LOW_PASS_ALPHA);
  printf("  - Debounce: %u amostras (%.0f ms)\n\n", DEBOUNCE_COUNT,
         (float)DEBOUNCE_COUNT * 1000.0f / (float)SAMPLE_RATE_HZ);

  printf("Iniciando monitoramento...\n");
  printf(
      "=================================================================\n\n");
}

static void print_sensor_status(const Sensor_t *temp_sensor,
                                const Sensor_t *oil_sensor,
                                const Sensor_t *rpm_sensor,
                                timestamp_ms_t time_ms) {
  printf("Tempo: %6.2f s\n", (float)time_ms / 1000.0f);
  printf("-----------------------------------------------------------------\n");
  Sensor_Print(temp_sensor);
  Sensor_Print(oil_sensor);
  Sensor_Print(rpm_sensor);
}

static void check_and_print_alerts(const Sensor_t *temp_sensor,
                                   const Sensor_t *oil_sensor,
                                   const Sensor_t *rpm_sensor) {
  AlertLevel_t temp_alert = Sensor_GetAlertLevel(temp_sensor);
  AlertLevel_t oil_alert = Sensor_GetAlertLevel(oil_sensor);
  AlertLevel_t rpm_alert = Sensor_GetAlertLevel(rpm_sensor);

  if ((temp_alert != ALERT_NONE) || (oil_alert != ALERT_NONE) ||
      (rpm_alert != ALERT_NONE)) {

    printf("\n");

    if (temp_alert != ALERT_NONE) {
      printf("⚠️  ALERTA: Temperatura %s! (%.1f°C)\n",
             Alert_LevelToString(temp_alert),
             Sensor_GetFilteredMA(temp_sensor));
    }

    if (oil_alert != ALERT_NONE) {
      printf("⚠️  ALERTA: Pressão de óleo %s! (%.2f bar)\n",
             Alert_LevelToString(oil_alert), Sensor_GetFilteredMA(oil_sensor));
    }

    if (rpm_alert != ALERT_NONE) {
      printf("⚠️  ALERTA: RPM %s! (%.0f RPM)\n", Alert_LevelToString(rpm_alert),
             Sensor_GetFilteredMA(rpm_sensor));
    }
  }
}

int main(void) {
  Sensor_t temp_sensor;
  Sensor_t oil_sensor;
  Sensor_t rpm_sensor;

  timestamp_ms_t time_ms = 0U;
  timestamp_ms_t last_print_time = 0U;

  sensor_value_t temp_reading;
  sensor_value_t oil_reading;
  sensor_value_t rpm_reading;

  print_header();

  SensorSimulator_Init();

  Sensor_Init(&temp_sensor, "Temperatura Motor", (sensor_value_t)TEMP_WARNING,
              (sensor_value_t)TEMP_CRITICAL, false, LOW_PASS_ALPHA);

  Sensor_Init(&oil_sensor, "Pressão de Óleo", OIL_PRESSURE_WARNING,
              OIL_PRESSURE_CRITICAL, true, LOW_PASS_ALPHA);

  Sensor_Init(&rpm_sensor, "RPM Motor", (sensor_value_t)RPM_REDLINE,
              (sensor_value_t)RPM_MAX, false, LOW_PASS_ALPHA);

  print_system_info();

  while (time_ms < SIMULATION_DURATION_MS) {

    temp_reading = SensorSimulator_GetTemperature(time_ms);
    rpm_reading = SensorSimulator_GetRPM(time_ms);
    oil_reading =
        SensorSimulator_GetOilPressure(time_ms, rpm_reading, temp_reading);

    Sensor_Update(&temp_sensor, temp_reading);
    Sensor_Update(&rpm_sensor, rpm_reading);
    Sensor_Update(&oil_sensor, oil_reading);

    if ((time_ms - last_print_time) >= PRINT_INTERVAL_MS) {
      print_sensor_status(&temp_sensor, &oil_sensor, &rpm_sensor, time_ms);
      check_and_print_alerts(&temp_sensor, &oil_sensor, &rpm_sensor);

      printf("\n==============================================================="
             "==\n\n");
      last_print_time = time_ms;
    }

    time_ms += SAMPLE_PERIOD_MS;
  }

  printf("\nSimulação concluída.\n");
  printf("=================================================================\n");

  return 0;
}
