# -----------------------------------------------------------------------------
# Makefile
# Jordan McCaskill
# CMPT 361
# 
# Makefile for the creation of the qotdd
# -----------------------------------------------------------------------------

# CFLAGS additions
CFLAGS=-Wall -std=c89 -g -D_POSIX_C_SOURCE=201112L

# Phony target declaration
.PHONY: all clean

# make qotdd executable
all: qotdd

# run the compiler on the .o files to create the executbale
qotdd: qotdd.o server.o client.o common.o reader.o jsmn/jsmn.o
	$(CC) $(LDFLAGS) $(CFLAGS) -o $@ $^

# Create qotdd.o
qotdd.o: qotdd.c qotdd.h
	$(CC) $(CFLAGS) -c $<

# Create server.o
server.o: server.c server.h
	$(CC) $(CFLAGS) -c $<

# Create client.o
client.o: client.c client.h
	$(CC) $(CFLAGS) -c $<

# Create reader.o
reader.o: reader.c reader.h
	$(CC) $(CFLAGS) -c $<

# Create common.o
common.o: common.c common.h
	$(CC) $(CFLAGS) -c $<

# Create jsmn.o
jsmn/jsmn.o:
	make -C jsmn

# clean up the extra files
clean:
	$(RM) qotdd qotdd.o server.o client.o reader.o common.o
	make clean -C jsmn
