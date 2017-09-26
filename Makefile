.PHONY: all clean

all: qotdd

qotdd: qotdd.o
	$(CC) $(LDFLAGS) -o $@ $^

qotdd.o: qotdd.c
	$(CC) $(CFLAGS) -c -g $<

clean:
	$(RM) qotdd qotdd.o