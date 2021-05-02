#include <stdio.h>
#include <math.h>
#include <string.h>
#include <stdlib.h>

#include "function.h"
#include "token_list.h"
#include "token.h"
#include "_complex.h"


complex_t _cabsl(complex_t a) {
    return cabsl(a);
}
long double frand() {
    return (long double) rand() / RAND_MAX;
}
complex_t crand() {
    long double real = rand();
    long double imag = rand();
    long double denom = 1.F / sqrtl(real*real + imag*imag);
    return real * denom + I * imag * denom;
}

static struct func_def_t FUNCTIONS[] = {
    {.name="sqrt" , .nargmap=NARG_1, .d1=sqrtl, .c1=csqrtl},
    {.name="abs"  , .nargmap=NARG_1, .d1=fabsl, .c1=_cabsl},
    {.name="sin"  , .nargmap=NARG_1, .d1=sinl,  .c1=csinl },
    {.name="cos"  , .nargmap=NARG_1, .d1=cosl,  .c1=ccosl },
    {.name="tan"  , .nargmap=NARG_1, .d1=tanl,  .c1=ctanl },
    {.name="asin" , .nargmap=NARG_1, .d1=asinl, .c1=casinl},
    {.name="acos" , .nargmap=NARG_1, .d1=acosl, .c1=cacosl},
    {.name="atan" , .nargmap=NARG_1, .d1=atanl, .c1=catanl},
    {.name="power", .nargmap=NARG_2, .d2=powl,  .c2=cpowl },
    {.name="rand" , .nargmap=NARG_0, .d0=frand, .c0=NULL  },
    {.name="crand", .nargmap=NARG_0, .d0=NULL,  .c0=crand },
};

const struct func_def_t *get_function(const char *fname, uint8_t nargs) {
    for(uint32_t i = 0; i < sizeof(FUNCTIONS) / sizeof(FUNCTIONS[0]); i++) {
        if(strcmp(FUNCTIONS[i].name, fname) == 0 && (FUNCTIONS[i].nargmap & (1 << nargs)))
            return &FUNCTIONS[i];
    }
    return NULL;
}
