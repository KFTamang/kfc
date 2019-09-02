CFLAGS=-std=c11 -g # -static
SRCS=container.c parse.c tokenizer.c codegen.c foo.c hoge.c symtagen.c
MAIN=main.c
#SYMTAB_TEST=symtagen_test.c
SYMTAB_TEST_O=symtagen_test.c
MAIN_TEST=main_test.c
OBJS=$(SRCS:.c=.o)

.SUFFIX: .o .c

kfc: $(OBJS) $(MAIN)
main.c:main.o
symtegen.c:symtegen.o

kfc: $(OBJS)
	$(CC) -o kfc $(MAIN) $(OBJS) $(CFLAGS)

$(OBJS): kfc.h
main.o: kfc.h
symtegen.o: kfc.h

test: kfc
	./test.sh

symtab_test: $(OBJS) $(SYMTAB_TEST_O)
	$(CC) -o symtab_test $(SYMTAB_TEST_O) $(OBJS) $(CFLAGS)
	./symtab_test

main_test: $(MAIN_TEST) $(OBJS)
	$(CC) -o main_test $(MAIN_TEST) $(OBJS) $(CFLAGS)
	./main_test > main_test.s
	gcc -o tmp main_test.s
	./tmp

clean:
	rm -f kfc *.o *~ tmp*

.PHONY: test clean
