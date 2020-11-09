CC=gcc
CFLAGS=-Wall -Wextra
EXE=bmp

all: build

build: bmp.c
	$(CC) $^ $(CFLAGS) -o $(EXE)

run: $(EXE)
	./$(EXE)

clean:
	rm -f $(EXE)

.PHONY: clean run
