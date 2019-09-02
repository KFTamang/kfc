CFLAGS=-std=c11 -g -static
SRCS=container.c parse.c tokenizer.c codegen.c foo.c hoge.c symtagen.c
MAIN=main.c
TEST=symtagen_test.c
OBJS=$(SRCS:.c=.o)


kfc: $(OBJS)
	$(CC) -o kfc $(MAIN) $(OBJS) $(LDFLAGS)

$(OBJS): kfc.h

test: kfc
	./test.sh

symtab_test: $(OBJS)
	$(CC) -o test $(TEST) $(OBJS) $(LDFLAGS)
	./test

clean:
	rm -f kfc *.o *~ tmp*

.PHONY: test clean
