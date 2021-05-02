#include <stddef.h>
#include <string.h>

#include "_complex.h"
#include "symbol.h"
#include "token.h"


struct sym_def_t {
    char sym[MAX_SYM_LENGTH+1];
    token_t token;
};


static struct sym_def_t SYMBOLS[] = {
    {.sym = "pi", .token = {.type = TT_COMPLEX, .data.c = 3.14159265358979323846264F}},
    {.sym = "e",  .token = {.type = TT_COMPLEX, .data.c = 2.718281828459045F}},
    {.sym = "i",  .token = {.type = TT_COMPLEX, .data.c = I}},
};

token_t *get_symbol(const char *sym) {
    for(uint32_t i = 0; i < sizeof(SYMBOLS) / sizeof(struct sym_def_t); i++)
        if(strcmp(SYMBOLS[i].sym, sym) == 0)
            return &SYMBOLS[i].token;

    return NULL;
}
