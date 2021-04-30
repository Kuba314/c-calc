#include <stdint.h>
#include "operator.h"
#include "function.h"
#include "complex.h"

#ifndef __TOKEN_H__
#define __TOKEN_H__

#define MAX_SYM_LENGTH 20

enum token_type_t {
    TT_REAL,
    TT_COMPLEX,
    TT_OPERATOR,
    TT_SYMBOL,
    TT_FUNCTION,
    TT_EXPRESSION
};

typedef struct {
    enum token_type_t type;
    union {
        long double d;
        complex_t c;
        operator_t op;
        char sym[MAX_SYM_LENGTH+1];
        func_t f;
        struct token_list_t *expr;
    } data;
} token_t;

complex_t mk_complex(long double real, long double imag);

#endif // __TOKEN_H__
