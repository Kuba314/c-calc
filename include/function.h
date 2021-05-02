#include <stdint.h>
#include "_complex.h"

#ifndef __FUNCTION_H__
#define __FUNCTION_H__

#define MAX_FUNC_ARGS 3
#define MAX_FUNC_LENGTH 20

#define NARG_0 1
#define NARG_1 2
#define NARG_2 4
#define NARG_3 8

struct token_list_t;
struct token_t;
typedef struct {
    char name[MAX_FUNC_LENGTH+1];
    struct token_list_t *args[MAX_FUNC_ARGS];
    uint8_t nargs;
} function_t;

struct func_def_t {
    const char *name;
    complex_t (*c0)();
    complex_t (*c1)(complex_t);
    complex_t (*c2)(complex_t, complex_t);
    complex_t (*c3)(complex_t, complex_t, complex_t);
    uint8_t nargmap;
};

// typedef struct token_t (common_func_t)(struct token_list_t *);

const struct func_def_t *get_function(const char *fname, uint8_t nargs);


#endif // __FUNCTION_H__
