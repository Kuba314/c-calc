#include <math.h>
#include <stddef.h>
#include "operator.h"
// #include "complex.h"



long double add_real(long double a, long double b) {
    return a + b;
}
long double sub_real(long double a, long double b) {
    return a - b;
}
long double mul_real(long double a, long double b) {
    return a * b;
}
long double div_real(long double a, long double b) {
    return a / b;
}
// long double mod_real(long double a, long double b);
// long double pow_real(long double a, long double b);
// long double fac_real(long double a);
// long double nop_real(long double a);
// long double neg_real(long double a);
// long double sqrt_real(long double a);
// long double root_real(long double a, long double b);

long double complex add_complex(long double complex a, long double complex b) {
    return a + b;
}
long double complex sub_complex(long double complex a, long double complex b) {
    return a - b;
}
long double complex mul_complex(long double complex a, long double complex b) {
    return a * b;
}
long double complex div_complex(long double complex a, long double complex b) {
    return a / b;
}
// complex_t mod_complex(complex_t a, complex_t b);
// complex_t pow_complex(complex_t a, complex_t b);
// complex_t fac_complex(complex_t a);
// complex_t nop_complex(complex_t a);
// complex_t neg_complex(complex_t a);
// complex_t sqrt_complex(complex_t a);
// complex_t root_complex(complex_t a, complex_t b);

static operator_t OPERATORS[] = {
    {.sym='+', .type=OP_BINARY, .priority=0, .fd.d2=add_real, .fc.c2=add_complex},
    {.sym='-', .type=OP_BINARY, .priority=0, .fd.d2=sub_real, .fc.c2=sub_complex},
    {.sym='*', .type=OP_BINARY, .priority=1, .fd.d2=mul_real, .fc.c2=mul_complex},
    {.sym='/', .type=OP_BINARY, .priority=1, .fd.d2=div_real, .fc.c2=div_complex},
    // (operator_t) {.sym='%', .type=OP_BINARY, .priority=1, .fd.d2=mod_real, .fc.c2=mod_complex},
    // (operator_t) {.sym='^', .type=OP_BINARY, .priority=3, .fd.d2=pow_real, .fc.c2=pow_complex},
    // (operator_t) {.sym='!', .type=OP_RUNARY, .priority=5, .fd.d1=fac_real, .fc.c1=fac_complex},
    // (operator_t) {.sym='+', .type=OP_LUNARY, .priority=2, .fd.d1=nop_real, .fc.c1=nop_complex},
    // (operator_t) {.sym='-', .type=OP_LUNARY, .priority=2, .fd.d1=neg_real, .fc.c1=neg_complex},
    // (operator_t) {.sym='#', .type=OP_LUNARY, .priority=4, .fd.d1=sqrt_real, .fc.c1=sqrt_complex},
    // (operator_t) {.sym='#', .type=OP_BINARY, .priority=4, .fd.d2=root_real, .fc.c2=root_complex},
};


operator_t *get_operator(char sym, enum op_type_t type) {
    for(uint32_t i = 0; i < sizeof(OPERATORS) / sizeof(operator_t); i++)
        if(OPERATORS[i].type == type && OPERATORS[i].sym == sym)
            return &OPERATORS[i];

    return NULL;
}
