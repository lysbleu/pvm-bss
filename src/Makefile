OUTPUT_NAME=craquage
OUTPUT_MULTITHREAD_NAME=craquage_multithread
LDFLAGS=-lpvm3 -lm -Wl,-R gmp/ -lgmp -lpthread
CFLAGS=-Wall -Wextra -g
CC=gcc

all: craquage craquage_multithread
monothread: craquage
multithread: craquage_multithread
$(OUTPUT_NAME): craquage.o craquage_esclave
	$(CC) -o $@ $< $(LDFLAGS)
craquage.o: craquage.c
	$(CC) -o $@ -c $< $(CFLAGS)
craquage_esclave: craquage_esclave.o
	$(CC) -o $@ $< $(LDFLAGS)
craquage_esclave.o: craquage_esclave.c
	$(CC) -o $@ -c $< $(CFLAGS)
$(OUTPUT_MULTITHREAD_NAME): craquage_multithread.o craquage_esclave_multithread
	$(CC) -o $@ $< $(LDFLAGS)
craquage_multithread.o: craquage_multithread.c
	$(CC) -o $@ -c $< $(CFLAGS)
craquage_esclave_multithread: craquage_esclave_multithread.o
	$(CC) -o $@ $< $(LDFLAGS)
craquage_esclave_multithread.o: craquage_esclave_multithread.c
	$(CC) -o $@ -c $< $(CFLAGS)
clean:
	rm *.o
mrproper: clean
	rm craquage craquage_esclave craquage_multithread craquage_esclave_multithread
