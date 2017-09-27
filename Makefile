CFLAGS=-Wall -std=c89 -D_POSIX_C_SOURCE=201112L

.PHONY: all clean

all: qotdd

qotdd: qotdd.o jsmn/jsmn.o
	$(CC) $(LDFLAGS) $(CFLAGS) -o $@ $^

qotdd.o: qotdd.c
	$(CC) $(CFLAGS) -c -g $<

jsmn/jsmn.o:
	make -C jsmn

clean:
	$(RM) qotdd qotdd.o
	make clean -C jsmn
