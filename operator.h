#include "op_functions.h"

#ifndef __OPERATOR_H__
#define __OPERATOR_H__

enum op_type_t {
    OP_LUNARY,
    OP_RUNARY,
    OP_BINARY
};

typedef struct {
    char sym;
    union {
        long double (*d1)(long double);
        long double (*d2)(long double, long double);
    } fd;
    union {
        complex_t (*c1)(complex_t);
        complex_t (*c2)(complex_t, complex_t);
    } fc;
    enum op_type_t type;
    uint8_t priority;
} operator_t;

#define N_RUNARY 1
#define N_LUNARY 3
#define N_BINARY 7
#define MAX_PRIORITY 5

operator_t *get_operator(char sym, enum op_type_t type);

#endif // __OPERATOR_H__
