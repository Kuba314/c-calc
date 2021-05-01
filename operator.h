#include <stdint.h>
#include <complex.h>

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
        long double complex (*c1)(long double complex a);
        long double complex (*c2)(long double complex a, long double complex b);
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
