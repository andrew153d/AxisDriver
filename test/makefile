CC = gcc
CFLAGS = -Wall
TARGET = doub
SRC = doub.c

all: $(TARGET)

$(TARGET): $(SRC)
	$(CC) $(CFLAGS) -o $(TARGET) $(SRC)  -lm

clean:
	rm -f $(TARGET)