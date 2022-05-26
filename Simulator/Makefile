CC = gcc
CFlags = -Wall -c
LDFlags = -lm
Objs = lc3.o memorymanagementunit.o consoleio.o

all: lc3sim

lc3sim: $(Objs)
	$(CC) $(LDFlags) $^ -o $@

%.o: %.c
	$(CC) $(CFlags) $< -o $@

.PHONY: clean
clean:
	rm -f lc3sim $(Objs)
