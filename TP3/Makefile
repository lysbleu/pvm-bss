CC=mpicc
CFLAGS=-W -Wall -Wextra -std=c99 -g
LDFLAGS=-lm
EXEC=interaction
SRC= $(wildcard *.c)
OBJ= $(SRC:.c=.o)

all: $(EXEC)

interaction: $(OBJ)
	$(CC) -o $@ $^ $(LDFLAGS)

%.o: %.c
	$(CC) -o $@ -c $< $(CFLAGS)

plot:

launch:
	mpiexec -np 4 interaction data/data_test.txt 10 1

.PHONY: clean mrproper

clean:
	rm -rf *.o
	rm -rf results/res*

mrproper: clean
	rm -rf $(EXEC)
