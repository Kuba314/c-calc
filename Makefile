CC = gcc
CFLAGS = -Werror -Wall -Wextra -pedantic -std=c99
LDLIBS = -lm

TARGETS = test

.PHONY: all clean

all: test eval

test: test.o token.o token_list.o error.o
eval: eval.o token.o token_list.o error.o tokenize.o evaluate.o


eval.o: eval.c token_list.o
test.o: test.c token_list.o

tokenize.o: tokenize.c tokenize.h
evaluate.o: evaluate.c evaluate.h
token_list.o: token_list.c token_list.h token.h error.h
token.o: token.c token.h
error.o: error.c error.h


clean:
	rm -f *.o $(TARGETS)
