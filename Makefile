CC = gcc
TARGET = music_visualizer.exe

ifeq ($(OS),Windows_NT)
	LIBS = -lraylib -lopengl32 -lgdi32 -lwinmm -lole32 -luuid
	SRC = main.c audioCaptureWindows.c

	RM = del
else
	LIBS = -lraylib -lasound -lm
	SRC = main.c audioCaptureLinux.c

	TARGET = music_visualizer
	RM = rm -f
endif

all: $(TARGET)

$(TARGET): $(SRC)
	$(CC) $(SRC) -o $(TARGET) $(LIBS)

run: $(TARGET)
	./$(TARGET)

clean:
	$(RM) $(TARGET)
