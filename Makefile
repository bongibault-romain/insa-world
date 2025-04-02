# Compiler and flags# Compiler and flags
# Variables de configuration
CC = gcc
CFLAGS = -Wall -g ${INCLUDES}    # Option pour inclure les fichiers .h générés

SRC_DIR = src
BUILD_DIR = build
BUILD_INCLUDE_DIR = $(BUILD_DIR)/include
BIN_DIR = bin
INCLUDE_DIR = include

# Fichiers sources
SRC = $(wildcard $(SRC_DIR)/*.c)
OBJ = $(SRC:$(SRC_DIR)/%.c=$(BUILD_DIR)/%.o)
INCLUDES = -I $(INCLUDE_DIR) -I $(BUILD_INCLUDE_DIR)

TARGET = $(BIN_DIR)/executable.exe

all: $(TARGET)

$(TARGET): $(OBJ)
	mkdir -p $(BIN_DIR)
	$(CC) $(CFLAGS) $^ -o $@

$(BUILD_DIR)/%.o: $(SRC_DIR)/%.c
	$(CC) $(CFLAGS) -c $< -o $@

# Nettoyage des fichiers générés
clean:
	rm -rf $(BUILD_DIR)/* $(BIN_DIR)/* $(BUILD_INCLUDE_DIR)/*

# Phony targets
.PHONY: all clean
