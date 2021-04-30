#include <stdint.h>

#ifndef __FUNCTION_H__
#define __FUNCTION_H__

#define MAX_FUNC_ARGS 3
#define MAX_FUNC_LENGTH 20

struct token_list_t;
struct token_t;
typedef struct {
    char name[MAX_FUNC_LENGTH+1];
    struct token_list_t *args[MAX_FUNC_ARGS];
    uint8_t nargs;
} function_t;

typedef struct token_t (common_func_t)(struct token_list_t *);

common_func_t *get_function(const char *fname);


#endif // __FUNCTION_H__
