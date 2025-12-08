#!/bin/bash

echo "=========================================="
echo "🚗 ECU Monitor - Instalação GUI"
echo "=========================================="
echo ""

GREEN='\033[0;32m'
YELLOW='\033[1;33m'
RED='\033[0;31m'
NC='\033[0m'

OS="$(uname -s)"
echo "Sistema: $OS"
echo ""

if [ ! -d "include" ] || [ ! -d "src" ]; then
    echo -e "${RED}✗ Execute na pasta ecu_modular${NC}"
    exit 1
fi

echo -e "${YELLOW}[1/4] Criando estrutura...${NC}"
mkdir -p gui bin
echo -e "${GREEN}✓ Pastas criadas${NC}"
echo ""

echo -e "${YELLOW}[2/4] Instalando Raylib...${NC}"
if pkg-config --exists raylib 2>/dev/null; then
    echo -e "${GREEN}✓ Raylib já instalado!${NC}"
else
    if [ "$OS" = "Linux" ]; then
        sudo apt-get update
        sudo apt-get install -y libasound2-dev libx11-dev libxrandr-dev \
            libxi-dev libgl1-mesa-dev libglu1-mesa-dev libxcursor-dev \
            libxinerama-dev libwayland-dev libxkbcommon-dev
        
        cd /tmp
        git clone --depth 1 https://github.com/raysan5/raylib.git
        cd raylib/src
        make PLATFORM=PLATFORM_DESKTOP
        sudo make install
        rm -rf /tmp/raylib
        cd - > /dev/null
        
    elif [ "$OS" = "Darwin" ]; then
        brew install raylib
    fi
    echo -e "${GREEN}✓ Raylib instalado!${NC}"
fi
echo ""

echo -e "${YELLOW}[3/4] Criando GUI...${NC}"
cat > gui/main_gui.c << 'EOF'
#include "raylib.h"
#include "sensors/sensor.h"
#include <stdio.h>
#include <math.h>

#define SCREEN_WIDTH 1200
#define SCREEN_HEIGHT 800
#define GRAPH_POINTS 200

typedef struct {
    float data[GRAPH_POINTS];
    int index;
    Color color;
} RealtimeGraph_t;

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

void DrawRealtimeGraph(RealtimeGraph_t *graph, int x, int y, int width, int height,
                       float min_val, float max_val, const char *label) {
    
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

void DrawGauge(int centerX, int centerY, int radius, float value, 
               float min_val, float max_val, const char *label, Color color) {
    
    DrawCircle(centerX, centerY, radius, (Color){20, 20, 30, 255});
    DrawCircleLines(centerX, centerY, radius, WHITE);
    
    float angle = 135 + (270 * (value - min_val) / (max_val - min_val));
    float radians = angle * DEG2RAD;
    
    int needleX = centerX + (int)(cosf(radians) * (radius - 10));
    int needleY = centerY + (int)(sinf(radians) * (radius - 10));
    
    DrawLineEx((Vector2){centerX, centerY}, (Vector2){needleX, needleY}, 3, color);
    DrawCircle(centerX, centerY, 8, color);
    
    DrawText(label, centerX - MeasureText(label, 20) / 2, centerY + radius + 10, 20, WHITE);
    
    char value_str[32];
    snprintf(value_str, sizeof(value_str), "%.1f", value);
    DrawText(value_str, centerX - MeasureText(value_str, 24) / 2, centerY - 10, 24, color);
}

int main(void) {
    InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "ECU Sensor Monitor - Real-Time Dashboard");
    SetTargetFPS(10);
    
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
    
    while (!WindowShouldClose()) {
        float temp = SensorSimulator_GetTemperature(time_ms);
        float rpm = SensorSimulator_GetRPM(time_ms);
        float oil = SensorSimulator_GetOilPressure(time_ms, rpm, temp);
        
        Sensor_Update(&temp_sensor, temp);
        Sensor_Update(&oil_sensor, oil);
        Sensor_Update(&rpm_sensor, rpm);
        
        UpdateGraph(&temp_graph, Sensor_GetFilteredMA(&temp_sensor));
        UpdateGraph(&oil_graph, Sensor_GetFilteredMA(&oil_sensor));
        UpdateGraph(&rpm_graph, Sensor_GetFilteredMA(&rpm_sensor));
        
        BeginDrawing();
        ClearBackground((Color){15, 15, 25, 255});
        
        DrawText("ECU SENSOR MONITOR", 20, 20, 40, WHITE);
        DrawText("Real-Time Dashboard", 20, 65, 20, LIGHTGRAY);
        
        DrawGauge(200, 220, 80, Sensor_GetFilteredMA(&temp_sensor), 
                  0, 150, "TEMP C", temp_graph.color);
        DrawGauge(400, 220, 80, Sensor_GetFilteredMA(&oil_sensor), 
                  0, 10, "OIL bar", oil_graph.color);
        DrawGauge(600, 220, 80, Sensor_GetFilteredMA(&rpm_sensor) / 1000, 
                  0, 8, "RPM x1000", rpm_graph.color);
        
        DrawRealtimeGraph(&temp_graph, 50, 350, 500, 150, 0, 150, "Temperatura (C)");
        DrawRealtimeGraph(&oil_graph, 650, 350, 500, 150, 0, 10, "Pressao Oleo (bar)");
        DrawRealtimeGraph(&rpm_graph, 50, 550, 1100, 200, 0, 8000, "RPM Motor");
        
        AlertLevel_t temp_level = Sensor_GetAlertLevel(&temp_sensor);
        if (temp_level == ALERT_WARNING) {
            DrawText("WARNING: Alta Temperatura!", 800, 50, 24, YELLOW);
        } else if (temp_level == ALERT_CRITICAL) {
            DrawText("CRITICAL: Temperatura Critica!", 800, 50, 24, RED);
        }
        
        char fps_text[32];
        snprintf(fps_text, sizeof(fps_text), "FPS: %d", GetFPS());
        DrawText(fps_text, SCREEN_WIDTH - 100, 20, 20, GREEN);
        
        EndDrawing();
        
        time_ms += 100;
    }
    
    CloseWindow();
    return 0;
}
EOF
echo -e "${GREEN}✓ GUI criada${NC}"
echo ""

echo -e "${YELLOW}[4/4] Criando Makefile...${NC}"
cat > Makefile.gui << 'EOF'
CC = gcc
CFLAGS = -Wall -Wextra -std=c99 -O2
LDFLAGS = -lm -lraylib -lGL -lpthread -ldl -lrt -lX11

TARGET = bin/ecu_gui

all:
	@mkdir -p bin
	@$(CC) $(CFLAGS) -Iinclude gui/main_gui.c \
		src/filters/*.c src/alerts/*.c src/sensors/*.c \
		$(LDFLAGS) -o $(TARGET)
	@echo "✓ Build concluído!"

run: all
	@./$(TARGET)

clean:
	@rm -rf bin build
	@echo "✓ Limpeza concluída"

.PHONY: all run clean
EOF
echo -e "${GREEN}✓ Makefile criado${NC}"
echo ""

echo -e "${YELLOW}Compilando...${NC}"
make -f Makefile.gui

if [ $? -eq 0 ]; then
    echo ""
    echo -e "${GREEN}=========================================="
    echo "✓ INSTALAÇÃO COMPLETA!"
    echo "==========================================${NC}"
    echo ""
    echo "Execute:"
    echo -e "${YELLOW}  make -f Makefile.gui run${NC}"
    echo ""
else
    echo -e "${RED}✗ Erro na compilação${NC}"
    exit 1
fi