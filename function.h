// #include "token.h"

#ifndef __FUNCTION_H__
#define __FUNCTION_H__

#define MAX_FUNC_ARGS 3

struct token_list_t;
struct token_t;
typedef struct {
    char name[MAX_SYM_LENGTH+1];
    struct token_t (*f)(struct token_list_t *args);
    struct token_list_t *args[MAX_FUNC_ARGS];
    uint8_t nargs;
} func_t;

func_t *get_func(const char *fname);

#endif // __FUNCTION_H__
