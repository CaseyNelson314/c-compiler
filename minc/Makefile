CFLAGS=-std=c11 -g -static

main: main.c parser.c tokenizer.c codegen.c

test: main
	./test.sh

clean:
	rm -f main *.o *~ tmp*

.PHONY: test clean
