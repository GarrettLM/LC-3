CC = gcc
CFlags = -Wall -c
LDFlags = -lm
Objs = assembler.o tokenizer.o parser.o opcodetable.o symboltable.o

all: assembler

assembler: $(Objs)
	$(CC) $(LDFlags) $^ -o $@

%.o: %.c
	$(CC) $(CFlags) $< -o $@

.PHONY: clean
clean:
	rm -f assembler $(Objs)
