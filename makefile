CC = gcc
CFLAGS = -Wall -Wextra -o3

TARGET = fuzzer
SRC = main.c fuzz.c tar_helper.c


.PHONY: all clean

all: $(TARGET)

$(TARGET): $(SRC)
	$(CC) $(CFLAGS) $(SRC) -o $(TARGET)

clean:
	rm -f $(TARGET)
