CC = gcc
CFLAGS = -Werror -Wall -Wextra -pedantic -std=c99 -g
LDLIBS = -lm

TARGETS = eval

.PHONY: all clean

all: $(TARGETS)

eval: eval.o token_list.o error.o tokenize.o evaluate.o operator.o function.o symbol.o


eval.o: eval.c tokenize.h evaluate.h token_list.h token.h error.h complex.h

tokenize.o: tokenize.c tokenize.h operator.h token_list.h token.h error.h
evaluate.o: evaluate.c evaluate.h token_list.h token.h symbol.h error.h
token_list.o: token_list.c token_list.h token.h
operator.o: operator.c operator.h complex.h
function.o: function.c function.h token_list.h token.h complex.h
symbol.o: symbol.c symbol.h token.h complex.h
error.o: error.c error.h


clean:
	rm -f *.o $(TARGETS)
