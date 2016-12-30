CC=gcc
CFLAGS=-Wall -g
BIN=miniftpd
OBJS=main.o sysutil.o

$(BIN):$(OBJS)
	$(CC) $(CFLAGS) $^ -o $@

%.o:%.c
	$(CC) $(CFLAGS) -c $< -o $@

.PHONY:clean
clean:
	rm -f *.o $(BIN)
