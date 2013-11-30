CC=mpicc
CFLAGS=-W -Wall -Wextra -std=c99 -g
LDFLAGS=-lm
EXEC=interaction
SRC= interaction.c parser.c
OBJ= $(SRC:.c=.o)

all: $(EXEC)

interaction: $(OBJ) atom.h
	$(CC) -o $@ atom.h $^ $(LDFLAGS)

%.o: %.c
	$(CC) -o $@ -c $< $(CFLAGS)

plot:

launch:
	mpiexec -np 2 ./interaction ./data/tsl.txt 20000

.PHONY: clean mrproper

clean:
	rm -rf *.o
	rm -rf results/res*

mrproper: clean
	rm -rf $(EXEC)
