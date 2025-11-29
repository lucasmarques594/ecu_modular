#include "common/config.h"
#include "common/types.h"
#include "sensors/sensor.h"
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
