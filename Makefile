CC = gcc
LIBS = -lraylib -lopengl32 -lgdi32 -lwinmm -lole32 -luuid
TARGET = music_visualizer.exe
SRC = main.c audioCapture.c

all: $(TARGET)

$(TARGET): $(SRC)
	$(CC) $(SRC) -o $(TARGET) $(LIBS)

run: $(TARGET)
	./$(TARGET)

clean:
	del $(TARGET)
