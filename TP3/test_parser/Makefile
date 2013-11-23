CC=mpicc
CFLAGS=-W -Wall -Wextra
LDFLAGS=-lm
EXEC=test_parse
SRC= test_parse.c
OBJ= $(SRC:.c=.o)

all: $(EXEC)

test_parse: $(OBJ)
	$(CC) -o $@ $^ ../atom.o ../parser.o $(LDFLAGS)

%.o: %.c
	$(CC) -o $@ -c $< $(CFLAGS)

plot:

.PHONY: clean mrproper

clean:
	rm -rf *.o

mrproper: clean
	rm -rf $(EXEC)
