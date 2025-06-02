BIN := zinf
CFLAGS := -Wall -Wextra
LFLAGS := -lraylib
OBJS := sprite.o

all: main.c $(OBJS)
	cc $(CFLAGS) -o $(BIN) main.c $(OBJS) $(LFLAGS)

%.o: %.c
	cc $(CFLAGS) -c $^
