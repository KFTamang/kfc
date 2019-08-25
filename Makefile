CFLAGS=-std=c11 -g -static
SRCS=$(wildcard *.c)
OBJS=$(SRCS:.c=.o)


kfc: $(OBJS)
	$(CC) -o kfc $(OBJS) $(LDFLAGS)

$(OBJS): kfc.h

test: kfc
	./test.sh

clean:
	rm -f kfc *.o *~ tmp*

.PHONY: test clean
