CFLAGS=-std=c11 -g # -static
SRCS=container.c parse.c tokenizer.c codegen.c foo.c hoge.c symtagen.c
MAIN=main.c
TEST=symtagen_test.c
OBJS=$(SRCS:.c=.o)

main.c:main.o
symtegen.c:symtegen.o

kfc: $(OBJS)
	$(CC) -o kfc $(MAIN) $(OBJS) $(CFLAGS)

$(OBJS): kfc.h

test: kfc
	./test.sh

symtab_test: $(OBJS)
	$(CC) -o test $(TEST) $(OBJS) $(CFLAGS)
	./test

clean:
	rm -f kfc *.o *~ tmp*

.PHONY: test clean
