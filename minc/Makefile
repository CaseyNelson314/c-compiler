CFLAGS=-std=c11 -g -static
SRC=$(wildcard *.c)
OBJ=$(SRC:.c=.o)

main: $(OBJ)
	$(CC) -o main $(OBJ) $(LDFLAGS)

$(OBJ):
	minc.h

test: main
	./test.sh

clean:
	rm -f main *.o *~ tmp*

.PHONY: test clean
