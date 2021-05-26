#include <stdio.h>
#include <math.h>
#include <string.h>
#include <stdlib.h>

#include "function.h"
#include "token_list.h"
#include "token.h"
#include "_complex.h"


static complex_t _cabsl(complex_t a)                   { return (complex_t) cabsl(a); }
static complex_t crootl(complex_t a, complex_t b)      { return cpowl(b, 1 / a); }
static complex_t frand()                             { return (complex_t) rand() / RAND_MAX; }
static complex_t crand() {
    register long double real = rand() - RAND_MAX / 2;
    register long double imag = rand() - RAND_MAX / 2;
    register long double denom = 1.F / sqrtl(real*real + imag*imag);
    return real * denom + imag * denom * I;
}
static complex_t gamma_complex(complex_t a) {
    return tgammal(cabsl(a));
}

static struct func_def_t FUNCTIONS[] = {
    {.name="abs"  , .nargmap=NARG_1, .c1=_cabsl       },
    {.name="sqrt" , .nargmap=NARG_1, .c1=csqrtl       },
    {.name="root" , .nargmap=NARG_2, .c2=crootl       },
    {.name="power", .nargmap=NARG_2, .c2=cpowl        },
    {.name="sin"  , .nargmap=NARG_1, .c1=csinl        },
    {.name="cos"  , .nargmap=NARG_1, .c1=ccosl        },
    {.name="tan"  , .nargmap=NARG_1, .c1=ctanl        },
    {.name="asin" , .nargmap=NARG_1, .c1=casinl       },
    {.name="acos" , .nargmap=NARG_1, .c1=cacosl       },
    {.name="atan" , .nargmap=NARG_1, .c1=catanl       },
    {.name="rand" , .nargmap=NARG_0, .c0=frand        },
    {.name="crand", .nargmap=NARG_0, .c0=crand        },
    {.name="gamma", .nargmap=NARG_1, .c1=gamma_complex},
};

const struct func_def_t *get_function(const char *fname, uint8_t nargs) {
    for(uint32_t i = 0; i < sizeof(FUNCTIONS) / sizeof(FUNCTIONS[0]); i++) {
        if(strcmp(FUNCTIONS[i].name, fname) == 0 && (FUNCTIONS[i].nargmap & (1 << nargs)))
            return &FUNCTIONS[i];
    }
    return NULL;
}
