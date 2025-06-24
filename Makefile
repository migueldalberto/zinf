BIN := zinf
CFLAGS := -Wall -Wextra
LFLAGS := -lraylib -lm
OBJS := sprite.o mapa.o entidade.o controles.o menu.o ranking.o

all: main.c $(OBJS) defs.h
	cc $(CFLAGS) -o $(BIN) main.c $(OBJS) $(LFLAGS)

%.o: %.c defs.h
	cc $(CFLAGS) -c $<
