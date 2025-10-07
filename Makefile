CC = gcc
LIBS = -lraylib -lopengl32 -lgdi32 -lwinmm
TARGET = music_visualizer.exe
SRC = main.c

all: $(TARGET)

$(TARGET): $(SRC)
	$(CC) $(SRC) -o $(TARGET) $(LIBS)

run: $(TARGET)
	./$(TARGET)

clean:
	del $(TARGET)
