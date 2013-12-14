DEBUG=yes
CC=gcc
ifeq ($(DEBUG),yes)
	CFLAGS=-W -Wall -Wextra -ansi -pedantic -g -std=c99
	LDFLAGS=
else
	CFLAGS=-W -Wall -Wextra	-O3 -ansi -pedantic -std=c99
	LDFLAGS=
endif
EXEC=driver
SRC= $(wildcard *.c)
OBJ= $(SRC:.c=.o)

all: $(EXEC)
ifeq ($(DEBUG),yes)
	@echo "Génération en mode debug"
else
	@echo "Génération en mode release"
endif

driver: $(OBJ)
	@$(CC) -o $@ $^ $(LDFLAGS)

util.o:util.c util.h 
	@$(CC) -o $@ -c $< $(CFLAGS)

%.o: %.c
	@$(CC) -o $@ -c $< $(CFLAGS)

.PHONY: clean mrproper

clean:
	@rm -rf *.o

mrproper: clean
	@rm -rf $(EXEC)