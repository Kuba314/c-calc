#include <math.h>
#include <string.h>
#include <complex.h>

#include "function.h"
#include "token_list.h"
#include "token.h"


token_t f_sqrt(struct token_list_t *args) {
    token_t *arg = &args->begin->token;
    if(arg->type == TT_REAL)
        return (token_t) {.type = TT_REAL, .data.d = sqrtl(arg->data.d)};
    else
        return (token_t) {.type = TT_COMPLEX, .data.c = csqrtl(arg->data.c)};
}
token_t f_abs(struct token_list_t *args) {
    token_t *arg = &args->begin->token;
    if(arg->type == TT_REAL)
        return (token_t) {.type = TT_REAL, .data.d = fabsl(arg->data.d)};
    else
        return (token_t) {.type = TT_REAL, .data.d = cabsl(arg->data.c)};
}

struct func_def {
    const char *name;
    common_func_t *f;
};

static struct func_def FUNCTIONS[] = {
    {.name="sqrt", .f=f_sqrt},
    {.name="abs" , .f=f_abs },
    // ((func_t) {.name=(sym_t) {.sym="sin",  .length=0}, .f=f_sin,  .nargs=1}),
    // ((func_t) {.name=(sym_t) {.sym="cos",  .length=0}, .f=f_cos,  .nargs=1}),
    // ((func_t) {.name=(sym_t) {.sym="tan",  .length=0}, .f=f_tan,  .nargs=1}),
    // ((func_t) {.name=(sym_t) {.sym="atan", .length=0}, .f=f_atan, .nargs=1}),
    // ((func_t) {.name=(sym_t) {.sym="", .length=0}, .nargs=0});
    // ((func_t) {.name=(sym_t) {.sym="", .length=0}, .nargs=0});
    // ((func_t) {.name=(sym_t) {.sym="", .length=0}, .nargs=0});
    // ((func_t) {.name=(sym_t) {.sym="", .length=0}, .nargs=0});
    // ((func_t) {.name=(sym_t) {.sym="", .length=0}, .nargs=0});
};

common_func_t *get_function(const char *fname) {
    for(uint32_t i = 0; i < sizeof(FUNCTIONS) / sizeof(FUNCTIONS[0]); i++) {
        if(strcmp(FUNCTIONS[i].name, fname) == 0)
            return FUNCTIONS[i].f;
    }
    return NULL;
}
