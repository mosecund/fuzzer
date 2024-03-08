CC = gcc
CFLAGS = -Wall -Wextra -o3

TARGET = fuzzer
SRC = src/main.c src/fuzz.c src/tar_helper.c


.PHONY: all clean

all: $(TARGET)

$(TARGET): $(SRC)
	$(CC) $(CFLAGS) $(SRC) -o $(TARGET)

clean:
	rm -f $(TARGET)
