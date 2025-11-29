CC = gcc
CFLAGS = -Wall -Wextra -Werror -std=c99 -O2 -pedantic
LDFLAGS = -lm

SRC_DIR = src
INC_DIR = include
BUILD_DIR = build
BIN_DIR = bin

TARGET = $(BIN_DIR)/ecu_monitor

INC_FLAGS = -I$(INC_DIR)

FILTER_SRC = $(SRC_DIR)/filters/moving_average.c \
             $(SRC_DIR)/filters/low_pass.c

ALERT_SRC = $(SRC_DIR)/alerts/alert.c

SENSOR_SRC = $(SRC_DIR)/sensors/sensor.c \
             $(SRC_DIR)/sensors/sensor_simulation.c

MAIN_SRC = $(SRC_DIR)/main.c

SOURCES = $(FILTER_SRC) $(ALERT_SRC) $(SENSOR_SRC) $(MAIN_SRC)

OBJECTS = $(SOURCES:$(SRC_DIR)/%.c=$(BUILD_DIR)/%.o)

all: directories $(TARGET)

directories:
	@mkdir -p $(BUILD_DIR)/filters
	@mkdir -p $(BUILD_DIR)/alerts
	@mkdir -p $(BUILD_DIR)/sensors
	@mkdir -p $(BIN_DIR)

$(TARGET): $(OBJECTS)
	@echo "Linkando: $@"
	@$(CC) $(OBJECTS) $(LDFLAGS) -o $@
	@echo "✓ Compilação concluída: $@"

$(BUILD_DIR)/%.o: $(SRC_DIR)/%.c
	@echo "Compilando: $<"
	@$(CC) $(CFLAGS) $(INC_FLAGS) -c $< -o $@

clean:
	@echo "Limpando arquivos de build..."
	@rm -rf $(BUILD_DIR) $(BIN_DIR)
	@echo "✓ Limpeza concluída"

run: all
	@echo "Executando $(TARGET)..."
	@./$(TARGET)

analyze: CFLAGS += -fanalyzer
analyze: clean all

debug: CFLAGS += -g -O0 -DDEBUG
debug: clean all
	@echo "✓ Build de debug criado"

info:
	@echo "========================================"
	@echo "Projeto: ECU Sensor Monitor (Modular)"
	@echo "========================================"
	@echo "Compilador: $(CC)"
	@echo "Flags: $(CFLAGS)"
	@echo "Target: $(TARGET)"
	@echo ""
	@echo "Comandos disponíveis:"
	@echo "  make          - Compila o projeto"
	@echo "  make run      - Compila e executa"
	@echo "  make clean    - Remove arquivos de build"
	@echo "  make debug    - Build com símbolos de debug"
	@echo "  make analyze  - Análise estática (GCC 10+)"
	@echo "  make info     - Mostra esta mensagem"
	@echo "========================================"

.PHONY: all clean run analyze debug info directories