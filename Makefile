CC = gcc
CFLAGS = -Werror -Wall -Wextra -pedantic -std=c99 -g
LDLIBS = -lm

TARGETS = eval

.PHONY: all clean

all: $(TARGETS)

test: test.o token_list.o error.o
eval: eval.o token_list.o error.o tokenize.o evaluate.o operator.o op_functions.o function.o complex.o


eval.o: eval.c token_list.o
test.o: test.c token_list.o

tokenize.o: tokenize.c tokenize.h
evaluate.o: evaluate.c evaluate.h
token_list.o: token_list.c token_list.h token.h error.h
operator.o: operator.c operator.h
function.o: function.c function.h
op_functions.o: op_functions.c op_functions.h
complex.o: complex.c complex.h
error.o: error.c error.h


clean:
	rm -f *.o $(TARGETS)
