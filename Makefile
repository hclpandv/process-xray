CC = cc
CFLAGS = -Wall -Wextra -Wpedantic -g

TARGET = process-xray
SRC = src/main.c

$(TARGET): $(SRC)
	$(CC) $(CFLAGS) $(SRC) -o $(TARGET)

.PHONY: clean

clean:
	rm -f process-xray
	rm -rf process-xray.dSYM