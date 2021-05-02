#include <math.h>
#include <stddef.h>
#include "_complex.h"
#include "operator.h"



complex_t add_complex(complex_t a, complex_t b) { return a + b; }
complex_t sub_complex(complex_t a, complex_t b) { return a - b; }
complex_t mul_complex(complex_t a, complex_t b) { return a * b; }
complex_t div_complex(complex_t a, complex_t b) { return a / b; }
complex_t nop_complex(complex_t a) { return a; }
complex_t neg_complex(complex_t a) { return -a; }
complex_t root_complex(complex_t a, complex_t b) { return cpowl(b, 1 / a); }

static operator_t OPERATORS[] = {
    {.sym='+', .type=OP_BINARY, .priority=0, .fc.c2=add_complex},
    {.sym='-', .type=OP_BINARY, .priority=0, .fc.c2=sub_complex},
    {.sym='*', .type=OP_BINARY, .priority=1, .fc.c2=mul_complex},
    {.sym='/', .type=OP_BINARY, .priority=1, .fc.c2=div_complex},
    {.sym='%', .type=OP_BINARY, .priority=1, .fc.c2=NULL},
    {.sym='^', .type=OP_BINARY, .priority=3, .fc.c2=cpowl},
    {.sym='!', .type=OP_RUNARY, .priority=5, .fc.c1=NULL},
    {.sym='+', .type=OP_LUNARY, .priority=2, .fc.c1=nop_complex},
    {.sym='-', .type=OP_LUNARY, .priority=2, .fc.c1=neg_complex},
    {.sym='#', .type=OP_LUNARY, .priority=4, .fc.c1=csqrtl},
    {.sym='#', .type=OP_BINARY, .priority=4, .fc.c2=root_complex},
};


operator_t *get_operator(char sym, enum op_type_t type) {
    for(uint32_t i = 0; i < sizeof(OPERATORS) / sizeof(operator_t); i++)
        if(OPERATORS[i].type == type && OPERATORS[i].sym == sym)
            return &OPERATORS[i];

    return NULL;
}
