#include <stdio.h>
#include <math.h>
#include <string.h>
#include <stdlib.h>

#include "function.h"
#include "token_list.h"
#include "token.h"
#include "_complex.h"


static long double rootl(long double a, long double b) { return powl(b, 1 / a); }
static complex_t _cabsl(complex_t a)                   { return cabsl(a); }
static complex_t crootl(complex_t a, complex_t b)      { return cpowl(b, 1 / a); }
static long double frand()                             { return (long double) rand() / RAND_MAX; }
static complex_t crand() {
    register long double real = rand();
    register long double imag = rand();
    register long double denom = 1.F / sqrtl(real*real + imag*imag);
    return real * denom + imag * denom * I;
}

static struct func_def_t FUNCTIONS[] = {
    {.name="abs"  , .nargmap=NARG_1, .d1=fabsl, .c1=_cabsl},

    {.name="sqrt" , .nargmap=NARG_1, .d1=sqrtl, .c1=csqrtl},
    {.name="root" , .nargmap=NARG_2, .d2=rootl, .c2=crootl},
    {.name="power", .nargmap=NARG_2, .d2=powl,  .c2=cpowl },

    {.name="sin"  , .nargmap=NARG_1, .d1=sinl,  .c1=csinl },
    {.name="cos"  , .nargmap=NARG_1, .d1=cosl,  .c1=ccosl },
    {.name="tan"  , .nargmap=NARG_1, .d1=tanl,  .c1=ctanl },
    {.name="asin" , .nargmap=NARG_1, .d1=asinl, .c1=casinl},
    {.name="acos" , .nargmap=NARG_1, .d1=acosl, .c1=cacosl},
    {.name="atan" , .nargmap=NARG_1, .d1=atanl, .c1=catanl},

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
