CC=mpicc
CFLAGS=-W -Wall -Wextra
LDFLAGS=-lm
EXEC=test_parse
SRC= $(wildcard *.c)
OBJ= $(SRC:.c=.o)

all: $(EXEC)

interaction: $(OBJ) ../atom.o ../parse.o
	$(CC) -o $@ $^ $(LDFLAGS)

%.o: %.c
	$(CC) -o $@ -c $< $(CFLAGS)

plot:

.PHONY: clean mrproper

clean:
	rm -rf *.o

mrproper: clean
	rm -rf $(EXEC)
