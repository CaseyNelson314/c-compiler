CFLAGS=-std=gnu11 -g -static
SRCS=$(wildcard *.c)
OBJS=$(SRCS:.c=.o)

main: $(OBJS)
	$(CC) -o main $(OBJS) $(LDFLAGS)

$(OBJS): minc.h

test: main
	./test.sh

clean:
	rm -f main *.o *~ tmp*

.PHONY: test clean