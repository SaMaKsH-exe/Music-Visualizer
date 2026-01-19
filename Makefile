CC = gcc
CFLAGS = -Wall -fPIC
INCLUDES = -Isrc -Iheader
BUILD_DIR = build
SRC_DIR = src
TARGET = $(BUILD_DIR)/music_visualizer.exe
PLUGIN = $(BUILD_DIR)/libpplug.so

ifeq ($(OS),Windows_NT)
	LIBS = -lraylib -lopengl32 -lgdi32 -lwinmm -lole32 -luuid
	SRC = $(SRC_DIR)/main.c $(SRC_DIR)/audioCaptureWindows.c $(SRC_DIR)/plug.c
	PLUGIN_SRC = $(SRC_DIR)/plug.c
	PLUGIN = $(BUILD_DIR)/libpplug.dll
	RM = del /Q /F
	MKDIR = if not exist $(BUILD_DIR) mkdir $(BUILD_DIR)
	CLEAN_TARGET = $(BUILD_DIR)\*
else
	LIBS = -lraylib -lasound -lm
	SRC = $(SRC_DIR)/main.c $(SRC_DIR)/audioCaptureLinux.c $(SRC_DIR)/plug.c
	PLUGIN_SRC = $(SRC_DIR)/plug.c
	TARGET = $(BUILD_DIR)/music_visualizer
	RM = rm -f
	MKDIR = mkdir -p $(BUILD_DIR)
	CLEAN_TARGET = $(BUILD_DIR)/*
endif

all: $(TARGET) $(PLUGIN)

$(TARGET): $(SRC)
	@$(MKDIR)
	$(CC) $(CFLAGS) $(INCLUDES) $(SRC) -o $(TARGET) $(LIBS)

$(PLUGIN): $(PLUGIN_SRC)
	@$(MKDIR)
	$(CC) $(CFLAGS) $(INCLUDES) -shared $(PLUGIN_SRC) -o $(PLUGIN) $(LIBS)

run: $(TARGET)
	./$(TARGET)

clean:
	$(RM) $(CLEAN_TARGET)
