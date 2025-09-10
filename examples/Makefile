CC = gcc
CFLAGS = `sdl2-config --cflags`
LDFLAGS = `sdl2-config --libs` -lm
SRC = example.c
OUT = example

default:
	$(CC) $(SRC) -o $(OUT) $(CFLAGS) $(LDFLAGS)

run: default
	./$(OUT)

clean:
	rm -rf $(OUT)

rebuild: clean default
	./$(OUT)

