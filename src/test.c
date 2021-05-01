#include <stdio.h>
#include "token_list.h"
#include "token.h"
#include "error.h"

long add_long(long a, long b) {
    return a + b;
}
long mul_long(long a, long b) {
    return a * b;
}
long double add_double(long double a, long double b) {
    return a + b;
}
long double mul_double(long double a, long double b) {
    return a * b;
}
complex_t add_complex(complex_t a, complex_t b) {
    return mk_complex(a.real + b.real, a.imag + b.imag);
}
complex_t mul_complex(complex_t a, complex_t b) {
    return mk_complex(a.real*b.real - a.imag*b.imag, a.imag*b.real + a.real*b.imag);
}

int main(int argc, char *argv[]) {
    (void) argc;
    (void) argv;

    token_list_t *tl = tl_init();
    if(tl == NULL)
        fatal("Couldn't init tokenlist");


    tl_append(tl, (token_t) {.type = TT_COMPLEX, .data.c = mk_complex(1.5F, 1.F)});
    tl_append(tl, (token_t) {.type = TT_OPERATOR, .data.op = (operator_t) {.sym = '+', .type = OP_BINARY, .fd.d2 = add_double, .fc.c2 = add_complex}});
    tl_append(tl, (token_t) {.type = TT_REAL, .data.d = 2.5});
    tl_append(tl, (token_t) {.type = TT_OPERATOR, .data.op = (operator_t) {.sym = '*', .type = OP_BINARY, .fd.d2 = mul_double, .fc.c2 = mul_complex}});
    tl_append(tl, (token_t) {.type = TT_REAL, .data.d = 2.5});

    tl_print(tl);
    printf("\n");

    tl_evalop(tl, 3);

    tl_print(tl);
    printf("\n");
    tl_evalop(tl, 1);

    tl_print(tl);

    tl_free(tl);
}
