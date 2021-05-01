#include <complex.h>
#include "operator.h"
#include "function.h"
#include "symbol.h"

#ifndef __TOKEN_H__
#define __TOKEN_H__

enum token_type_t {
    TT_REAL,
    TT_COMPLEX,
    TT_OPERATOR,
    TT_SYMBOL,
    TT_FUNCTION,
    TT_EXPRESSION
};

typedef struct token_t token_t;
struct token_t {
    enum token_type_t type;
    union {
        long double d;
        long double complex c;
        operator_t op;
        char sym[MAX_SYM_LENGTH+1];
        function_t f;
        struct token_list_t *expr;
    } data;
};

#endif // __TOKEN_H__
