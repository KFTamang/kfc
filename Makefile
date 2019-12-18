CFLAGS=-std=c11 -g # -static
SRCS=container.c parse.c tokenizer.c codegen.c test_helper.c # foo.c hoge.c visualize_ast.c
MAIN=main.c
#SYMTAB_TEST=symtagen_test.c
SYMTAB_TEST_O=symtabgen_test.c
MAIN_TEST=main_test.c
OBJS=$(SRCS:%.c=%.o)

#.SUFFIX: .o .c

.c.o: kfc.h
	$(CC) -c $< $(CFLAGS)

kfc: $(OBJS) main.o
	$(CC) -o kfc $(MAIN) $(OBJS) $(CFLAGS)

#$(OBJS): kfc.h
#main.o: kfc.h
#symtegen.o: kfc.h

test: kfc tests.c stubs.c
	./test.sh

test_alloc: test_alloc.c test_helper.o
	$(CC) -o test_alloc test_alloc.c test_helper.o
	./test_alloc

symtab_test: $(OBJS) $(SYMTAB_TEST_O)
	$(CC) -o symtab_test $(SYMTAB_TEST_O) $(OBJS) $(CFLAGS)
	./symtab_test

main_test: $(MAIN_TEST) $(OBJS)
	$(CC) -o main_test $(MAIN_TEST) $(OBJS) $(CFLAGS)
	./main_test > main_test.s
	gcc -o tmp main_test.s
	./tmp

clean:
	rm -f kfc *.o *~ tmp* main_test symtab_test test test_alloc 

.PHONY: test clean
