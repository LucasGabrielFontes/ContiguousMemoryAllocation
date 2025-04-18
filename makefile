CC = gcc
CFLAGS = -Wall -Iinclude
SRC = src
OBJ = obj
BIN = .

OBJS = $(OBJ)/allocator.o $(OBJ)/memory.o
TARGET = $(BIN)/allocator

all: $(TARGET)

$(OBJ)/allocator.o: $(SRC)/allocator.c include/memory.h
	mkdir -p $(OBJ)
	$(CC) $(CFLAGS) -c $(SRC)/allocator.c -o $@

$(OBJ)/memory.o: $(SRC)/memory.c include/memory.h
	mkdir -p $(OBJ)
	$(CC) $(CFLAGS) -c $(SRC)/memory.c -o $@

$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) -o $@ $^

run: $(TARGET)
	$(TARGET) 1048576 < teste.txt

clean:
	rm -rf $(OBJ)/*.o $(TARGET)
