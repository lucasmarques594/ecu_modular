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
             $(SRC_DIR)/sensors/sensor_simulator.c

MAIN_SRC = $(SRC_DIR)/main.c

SOURCES = $(FILTER_SRC) $(ALERT_SRC) $(SENSOR_SRC) $(MAIN_SRC)

OBJECTS = $(SOURCES:$(SRC_DIR)/%.c=$(BUILD_DIR)/%.o)

all: directories $(TARGET)

