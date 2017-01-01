CC=gcc
CFLAGS=-Wall -g
BIN=miniftpd.out
OBJS=main.o sysutil.o session.o ftpproto.o privparent.o

$(BIN):$(OBJS)
	$(CC) $(CFLAGS) $^ -o $@

%.o:%.c
	$(CC) $(CFLAGS) -c $< -o $@

.PHONY:clean
clean:
	rm -f *.o $(BIN)
