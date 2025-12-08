#include "raylib.h"
#include "sensors/sensor.h"
#include "sensors/sensor_simulation.h"
#include <math.h>
#include <stdio.h>
#include <stdlib.h>

#define SCREEN_WIDTH 1200
#define SCREEN_HEIGHT 800
#define GRAPH_POINTS 200

typedef struct {
  float data[GRAPH_POINTS];
  int index;
  Color color;
} RealtimeGraph_t;

typedef struct {
  float current_rpm;
  float target_rpm;
  float acceleration;
  int idle_counter;
  int gear_shift_counter;
} RealisticRPM_t;

static RealisticRPM_t rpm_state = {800.0f, 800.0f, 0.0f, 0, 0};

void InitGraph(RealtimeGraph_t *graph, Color color) {
  for (int i = 0; i < GRAPH_POINTS; i++) {
    graph->data[i] = 0.0f;
  }
  graph->index = 0;
  graph->color = color;
}

void UpdateGraph(RealtimeGraph_t *graph, float value) {
  graph->data[graph->index] = value;
  graph->index = (graph->index + 1) % GRAPH_POINTS;
}

float GetRealisticRPM(void) {
  rpm_state.idle_counter++;
  rpm_state.gear_shift_counter++;

  if (rpm_state.idle_counter > 300) {
    float rand_val = (float)(rand() % 100) / 100.0f;

    if (rand_val < 0.3f) {
      rpm_state.target_rpm = 800.0f + (float)(rand() % 100);
      rpm_state.acceleration = 0.5f;
    } else if (rand_val < 0.5f) {
      rpm_state.target_rpm = 1500.0f + (float)(rand() % 500);
      rpm_state.acceleration = 3.0f;
    } else if (rand_val < 0.7f) {
      rpm_state.target_rpm = 2500.0f + (float)(rand() % 1000);
      rpm_state.acceleration = 5.0f;
    } else if (rand_val < 0.85f) {
      rpm_state.target_rpm = 3500.0f + (float)(rand() % 1500);
      rpm_state.acceleration = 7.0f;
    } else {
      rpm_state.target_rpm = 4500.0f + (float)(rand() % 1000);
      rpm_state.acceleration = 10.0f;
    }

    rpm_state.idle_counter = 0;
  }

  if (rpm_state.gear_shift_counter > 100 && rpm_state.current_rpm > 3000) {
    if ((float)(rand() % 100) / 100.0f < 0.3f) {
      rpm_state.target_rpm = 2000.0f + (float)(rand() % 500);
      rpm_state.acceleration = 15.0f;
      rpm_state.gear_shift_counter = 0;
    }
  }

  float diff = rpm_state.target_rpm - rpm_state.current_rpm;

  if (fabs(diff) > 1.0f) {
    if (diff > 0) {
      rpm_state.current_rpm += rpm_state.acceleration;
    } else {
      rpm_state.current_rpm -= rpm_state.acceleration * 1.5f;
    }
  }

  float noise = ((float)(rand() % 100) - 50.0f) / 2.0f;
  float result = rpm_state.current_rpm + noise;

  if (result < 750.0f)
    result = 750.0f;
  if (result > 7000.0f)
    result = 7000.0f;

  return result;
}

void DrawRealtimeGraph(RealtimeGraph_t *graph, int x, int y, int width,
                       int height, float min_val, float max_val,
                       const char *label) {

  DrawRectangle(x, y, width, height, (Color){20, 20, 30, 255});
  DrawRectangleLines(x, y, width, height, WHITE);
  DrawText(label, x + 10, y + 5, 20, WHITE);

  char max_str[16], min_str[16];
  snprintf(max_str, sizeof(max_str), "%.0f", max_val);
  snprintf(min_str, sizeof(min_str), "%.0f", min_val);
  DrawText(max_str, x + width + 5, y, 16, LIGHTGRAY);
  DrawText(min_str, x + width + 5, y + height - 16, 16, LIGHTGRAY);

  for (int i = 1; i < GRAPH_POINTS; i++) {
    int idx1 = (graph->index + i - 1) % GRAPH_POINTS;
    int idx2 = (graph->index + i) % GRAPH_POINTS;

    float val1 = graph->data[idx1];
    float val2 = graph->data[idx2];

    float norm1 = (val1 - min_val) / (max_val - min_val);
    float norm2 = (val2 - min_val) / (max_val - min_val);

    norm1 = fmaxf(0.0f, fminf(1.0f, norm1));
    norm2 = fmaxf(0.0f, fminf(1.0f, norm2));

    int x1 = x + ((i - 1) * width) / GRAPH_POINTS;
    int y1 = y + height - (int)(norm1 * height);
    int x2 = x + (i * width) / GRAPH_POINTS;
    int y2 = y + height - (int)(norm2 * height);

    DrawLine(x1, y1, x2, y2, graph->color);

    if (i == GRAPH_POINTS - 1) {
      DrawCircle(x2, y2, 4, graph->color);
    }
  }
}

void DrawGauge(int centerX, int centerY, int radius, float value, float min_val,
               float max_val, const char *label, Color color) {

  DrawCircle(centerX, centerY, radius, (Color){20, 20, 30, 255});
  DrawCircleLines(centerX, centerY, radius, WHITE);

  float angle = 135 + (270 * (value - min_val) / (max_val - min_val));
  float radians = angle * DEG2RAD;

  int needleX = centerX + (int)(cosf(radians) * (radius - 10));
  int needleY = centerY + (int)(sinf(radians) * (radius - 10));

  DrawLineEx((Vector2){centerX, centerY}, (Vector2){needleX, needleY}, 3,
             color);
  DrawCircle(centerX, centerY, 8, color);

  DrawText(label, centerX - MeasureText(label, 18) / 2, centerY + radius + 10,
           18, LIGHTGRAY);

  char value_str[32];
  if (value >= 1000) {
    snprintf(value_str, sizeof(value_str), "%.0f", value);
  } else if (value >= 100) {
    snprintf(value_str, sizeof(value_str), "%.1f", value);
  } else {
    snprintf(value_str, sizeof(value_str), "%.2f", value);
  }

  int text_width = MeasureText(value_str, 28);
  DrawText(value_str, centerX - text_width / 2, centerY - radius - 35, 28,
           color);
}

int main(void) {
  InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT,
             "ECU Sensor Monitor - Real-Time Dashboard");
  SetTargetFPS(60);

  Sensor_t temp_sensor, oil_sensor, rpm_sensor;
  SensorSimulator_Init();

  Sensor_Init(&temp_sensor, "Temperatura", 95, 110, false, 0.2f);
  Sensor_Init(&oil_sensor, "Pressao Oleo", 1.5f, 1.0f, true, 0.2f);
  Sensor_Init(&rpm_sensor, "RPM", 6500, 8000, false, 0.2f);

  RealtimeGraph_t temp_graph, oil_graph, rpm_graph;
  InitGraph(&temp_graph, (Color){255, 100, 100, 255});
  InitGraph(&oil_graph, (Color){100, 255, 100, 255});
  InitGraph(&rpm_graph, (Color){100, 150, 255, 255});

  timestamp_ms_t time_ms = 0;
  int frame_counter = 0;

  while (!WindowShouldClose()) {
    frame_counter++;

    if (frame_counter % 6 == 0) {
      float temp = SensorSimulator_GetTemperature(time_ms);
      float realistic_rpm = GetRealisticRPM();
      float oil = SensorSimulator_GetOilPressure(time_ms, realistic_rpm, temp);

      Sensor_Update(&temp_sensor, temp);
      Sensor_Update(&oil_sensor, oil);
      Sensor_Update(&rpm_sensor, realistic_rpm);

      UpdateGraph(&temp_graph, Sensor_GetFilteredMA(&temp_sensor));
      UpdateGraph(&oil_graph, Sensor_GetFilteredMA(&oil_sensor));
      UpdateGraph(&rpm_graph, Sensor_GetFilteredMA(&rpm_sensor));

      time_ms += 100;
    }

    BeginDrawing();
    ClearBackground((Color){15, 15, 25, 255});

    DrawText("ECU SENSOR MONITOR", 20, 20, 40, WHITE);
    DrawText("Real-Time Dashboard", 20, 65, 20, LIGHTGRAY);

    DrawGauge(200, 220, 80, Sensor_GetFilteredMA(&temp_sensor), 0, 150,
              "TEMP C", temp_graph.color);
    DrawGauge(400, 220, 80, Sensor_GetFilteredMA(&oil_sensor), 0, 10, "OIL bar",
              oil_graph.color);
    DrawGauge(600, 220, 80, Sensor_GetFilteredMA(&rpm_sensor) / 1000, 0, 8,
              "RPM x1000", rpm_graph.color);

    DrawRealtimeGraph(&temp_graph, 50, 350, 500, 150, 0, 150,
                      "Temperatura (C)");
    DrawRealtimeGraph(&oil_graph, 650, 350, 500, 150, 0, 10,
                      "Pressao Oleo (bar)");
    DrawRealtimeGraph(&rpm_graph, 50, 550, 1100, 200, 0, 8000, "RPM Motor");

    AlertLevel_t temp_level = Sensor_GetAlertLevel(&temp_sensor);
    if (temp_level == ALERT_WARNING) {
      DrawRectangle(750, 40, 420, 40, (Color){200, 150, 0, 100});
      DrawText("WARNING: Alta Temperatura!", 760, 50, 24, YELLOW);
    } else if (temp_level == ALERT_CRITICAL) {
      DrawRectangle(750, 40, 450, 40, (Color){200, 0, 0, 100});
      DrawText("CRITICAL: Temperatura Critica!", 760, 50, 24, RED);
    }

    char fps_text[32];
    snprintf(fps_text, sizeof(fps_text), "FPS: %d", GetFPS());
    DrawText(fps_text, SCREEN_WIDTH - 100, 20, 20, GREEN);

    EndDrawing();
  }

  CloseWindow();
  return 0;
}