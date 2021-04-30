#include <math.h>
#include "operator.h"



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

complex_t add_complex(complex_t a, complex_t b) {
    return mk_complex(a.real + b.real, a.imag + b.imag);
}
complex_t sub_complex(complex_t a, complex_t b) {
    return mk_complex(a.real - b.real, a.imag - b.imag);
}
complex_t mul_complex(complex_t a, complex_t b) {
    return mk_complex(a.real*b.real - a.imag*b.imag, a.imag*b.real + a.real*b.imag);
}
complex_t div_complex(complex_t a, complex_t b) {
    register long double denom = b.real*b.real + b.imag*b.imag;
    return mk_complex((a.real*b.real + a.imag*b.imag) / denom, (a.imag*b.real - a.real*b.imag) / denom);
}
// complex_t mod_complex(complex_t a, complex_t b);
// complex_t pow_complex(complex_t a, complex_t b);
// complex_t fac_complex(complex_t a);
// complex_t nop_complex(complex_t a);
// complex_t neg_complex(complex_t a);
// complex_t sqrt_complex(complex_t a);
// complex_t root_complex(complex_t a, complex_t b);


const static operator_t OPERATORS[] = {
    (operator_t) {.sym='+', .type=OP_BINARY, .priority=0, .fd.d2=add_real, .fc.c2=add_complex},
    (operator_t) {.sym='-', .type=OP_BINARY, .priority=0, .fd.d2=sub_real, .fc.c2=sub_complex},
    (operator_t) {.sym='*', .type=OP_BINARY, .priority=1, .fd.d2=mul_real, .fc.c2=mul_complex},
    (operator_t) {.sym='/', .type=OP_BINARY, .priority=1, .fd.d2=div_real, .fc.c2=div_complex},
    (operator_t) {.sym='%', .type=OP_BINARY, .priority=1, .fd.d2=mod_real, .fc.c2=mod_complex},
    (operator_t) {.sym='^', .type=OP_BINARY, .priority=3, .fd.d2=pow_real, .fc.c2=pow_complex},
    (operator_t) {.sym='!', .type=OP_RUNARY, .priority=5, .fd.d1=fac_real, .fc.c1=fac_complex},
    (operator_t) {.sym='+', .type=OP_LUNARY, .priority=2, .fd.d1=nop_real, .fc.c1=nop_complex},
    (operator_t) {.sym='-', .type=OP_LUNARY, .priority=2, .fd.d1=neg_real, .fc.c1=neg_complex},
    (operator_t) {.sym='#', .type=OP_LUNARY, .priority=4, .fd.d1=sqrt_real, .fc.c1=sqrt_complex},
    (operator_t) {.sym='#', .type=OP_BINARY, .priority=4, .fd.d2=root_real, .fc.c2=root_complex},
};


operator_t *get_operator(char sym, enum op_type_t type) {
    for(uint32_t i = 0; i < sizeof(FUNCTIONS) / sizeof(func_t); i++)
        if(OPERATORS[i].type == type && OPERATORS[i].sym == sym)
            return &OPERATORS[i];

    return NULL;
}
