#include <stdint.h>
#include <stdbool.h>
#include "token.h"

#ifndef __TOKEN_LIST_H__
#define __TOKEN_LIST_H__


typedef struct token_list_node_s token_list_node_t;
struct token_list_node_s {
    token_list_node_t *next;
    token_t token;
};

typedef struct token_list_t token_list_t;
struct token_list_t {
    uint32_t size;
    token_list_node_t *begin;
};


token_list_t *tl_init();
token_list_node_t *tl_append(token_list_t *tl, token_t token);
token_list_node_t *tl_replace(token_list_t *tl, uint32_t index, token_t token);
bool tl_remove(token_list_t *tl, uint32_t index);
void tl_print(token_list_t *tl);
void tl_free(token_list_t *tl);

token_list_node_t *tl_evalop(token_list_t *tl, uint32_t index);

#endif // __TOKEN_LIST_H__
