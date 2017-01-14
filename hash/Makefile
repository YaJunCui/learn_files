CC=gcc
CFLAGS=-Wall -g
BIN=main.out
OBJS=main.o hash.o 

$(BIN):$(OBJS)
	$(CC) $(CFLAGS) $^ -o $@

%.o:%.c
	$(CC) $(CFLAGS) -c $< -o $@

.PHONY:clean
clean:
	rm -f *.o $(BIN)
