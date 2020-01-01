TARGET=tictoctoe
SRC=$(TARGET).c
ASM=$(TARGET).s
CC=gcc
FLAGS=-no-pie

all:$(TARGET)

$(TARGET):$(ASM)
	$(CC) $(FLAGS) -o $(TARGET) $(ASM)
$(ASM):$(SRC)
	../kfc $(SRC) > $(ASM)
gmake:$(SRC)
	$(CC) -o $(TARGET) $(SRC)

test:$(TARGET)
	./$(TARGET)
clean:
	rm -f $(TARGET) $(ASM)

.PHONY: test clean
