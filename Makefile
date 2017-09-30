CFLAGS=-Wall -std=c89 -g -D_POSIX_C_SOURCE=201112L

.PHONY: all clean

all: qotdd

qotdd: qotdd.o server.o client.o common.o reader.o jsmn/jsmn.o
	$(CC) $(LDFLAGS) $(CFLAGS) -o $@ $^

qotdd.o: qotdd.c qotdd.h
	$(CC) $(CFLAGS) -c $<

server.o: server.c server.h
	$(CC) $(CFLAGS) -c $<

client.o: client.c client.h
	$(CC) $(CFLAGS) -c $<

reader.o: reader.c reader.h
	$(CC) $(CFLAGS) -c $<

common.o: common.c common.h
	$(CC) $(CFLAGS) -c $<

jsmn/jsmn.o:
	make -C jsmn

clean:
	$(RM) qotdd qotdd.o server.o client.o reader.o common.o
	make clean -C jsmn
