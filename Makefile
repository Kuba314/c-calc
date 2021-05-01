CC = gcc
CFLAGS = -Werror -Wall -Wextra -pedantic -std=c99 -fPIC -I include -g
LDLIBS = -lm

TARGETS = eval libmath.a libmath.so calculator
LIBOBJS = token_list.o error.o tokenize.o evaluate.o operator.o function.o symbol.o

vpath %.c src
vpath %.h include

.PHONY: all clean

all: $(TARGETS)

libmath.so: $(LIBOBJS)
	$(CC) -o $@ $^ -shared -fPIC

libmath.a: $(LIBOBJS)
	ar rcs $@ $^

eval: eval.o token_list.o error.o tokenize.o evaluate.o operator.o function.o symbol.o
# calculator: calculator.o libmath.a
# 	$(CC) $(LDFLAGS) $(shell pkg-config --ldflags gtk+-3.0) -o $@ $^ $(LDLIBS)



calculator: calculator.c
	$(CC) $(CFLAGS) $(shell pkg-config --cflags gtk+-3.0) -o $@ $< $(shell pkg-config --libs gtk+-3.0) -L. -lmath
eval.o: eval.c tokenize.h evaluate.h token_list.h token.h error.h _complex.h

tokenize.o: tokenize.c tokenize.h operator.h token_list.h token.h error.h
evaluate.o: evaluate.c evaluate.h token_list.h token.h symbol.h error.h
token_list.o: token_list.c token_list.h token.h
operator.o: operator.c operator.h _complex.h
function.o: function.c function.h token_list.h token.h _complex.h
symbol.o: symbol.c symbol.h token.h _complex.h
error.o: error.c error.h


clean: clean-objs
	rm -f $(TARGETS)

clean-objs:
	rm -f *.o
