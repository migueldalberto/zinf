BIN := zinf
CFLAGS := -Wall -Wextra
LFLAGS := -lraylib

all: main.c
	cc $(CFLAGS) -o $(BIN) main.c $(LFLAGS)
