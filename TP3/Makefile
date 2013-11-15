CC=gcc
CFLAGS=-W -Wall -Wextra
LDFLAGS=
EXEC=interaction
SRC= $(wildcard *.c)
OBJ= $(SRC:.c=.o)

all: $(EXEC)

interaction: $(OBJ)
	$(CC) -o $@ $^ $(LDFLAGS)

%.o: %.c
	$(CC) -o $@ -c $< $(CFLAGS)

plot:

.PHONY: clean mrproper

clean:
	rm -rf *.o

mrproper: clean
	rm -rf $(EXEC)
