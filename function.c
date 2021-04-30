#include <math.h>
#include "function.h"
#include "token_list.h"
#include "token.h"

// typedef struct token_t token_t;

token_t f_sqrt(struct token_list_t *args) {
    token_t *arg = args->begin->token;
    if(arg->type == TT_REAL)
        return (token_t) {.type = TT_REAL, .data.d = fsqrt(arg->data.d)};
    else
        return (token_t) {.type = TT_COMPLEX, .data.c = arg->data.c};
}
token_t f_abs(struct token_list_t *args) {
    token_t *arg = args->begin->token;
    if(arg->type == TT_REAL)
        return (token_t) {.type = TT_REAL, .data.d = fabs(arg->data.d)};
    else
        return (token_t) {.type = TT_COMPLEX, .data.c = fsqrt(arg->data.c.real*arg->data.c.real + arg->data.c.imag*arg->data.c.imag)};
}

const static func_t FUNCTIONS[] = {
    ((func_t) {.name="sqrt", .f=f_sqrt, .nargs=1}),
    ((func_t) {.name="abs" , .f=f_abs,  .nargs=1}),
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

func_t *get_func(const char *fname) {
    for(uint32_t i = 0; i < sizeof(FUNCTIONS) / sizeof(func_t); i++) {
        if(strcmp(FUNCTIONS[i].name.sym, fname) == 0)
            return &FUNCTIONS[i];
    }
    return NULL;
}
