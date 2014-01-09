DEBUG=yes
CC=gcc
ifeq ($(DEBUG),yes)
	CFLAGS=-W -Wall -Wextra -ansi -pedantic -g -std=c99
	LDFLAGS= -lpthread
else
	CFLAGS=-W -Wall -Wextra	-O3 -ansi -pedantic -std=c99
	LDFLAGS= -lpthread
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

mkl: driver.o util.o perf.o
	icc -o driver_mkl driver.o perf.o util.o $(LDFLAGS) -mkl

.PHONY: clean mrproper

clean:
	@rm -rf *.o

mrproper: clean
	@rm -rf $(EXEC)
