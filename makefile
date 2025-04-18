CC = gcc
CFLAGS = -Wall
TARGET = allocator

all: $(TARGET)

$(TARGET): allocator.c
	$(CC) $(CFLAGS) -o $(TARGET) allocator.c

run: $(TARGET)
	./$(TARGET) 1048576

clean:
	rm -f $(TARGET)
