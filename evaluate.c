#include <stdio.h>
#include "token.h"
#include "token_list.h"

token_list_node_t *eval_func(token_list_t *tl, uint32_t index) {

    token_list_node_t *first = tl->begin;
    token_list_node_t *last = NULL;

    if(first == NULL)
        return NULL;

    // find function index
    for(uint32_t i = 0; i < index; i++) {
        last = first;
        if((first = first->next) == NULL)
            return NULL;
    }
    if(first->token.type != TT_FUNCTION)
        return NULL;

    // evaluate function
    
    first->token.f();

    token_list_node_t *new_node = _init_empty_node();
    if(new_node == NULL)
        return NULL;

    return new_node;
}

int evaluate_tokenized(token_list_t *tokens, token_t *token) {

    for(uint8_t pr = MAX_PRIORITY; pr >= 0; pr--) {

        uint32_t index = 0;
        token_list_node_t *curr = tokens->begin;

        token_t ev_expr;
        int err;
        while(curr != NULL) {
            if(curr->token.type == TT_OPERATOR && curr->token.priority == pr) {
                curr = tl_evalop(tokens, index);
            } else if(curr->token.type == TT_EXPRESSION) {
                if(err = evaluate_tokenized(tokens, &ev_expr))
                    return err;
                tl_replace(tokens, index, ev_expr);
            } else if(curr->token.type == TT_FUNCTION) {

                token_list_t *args = tl_init();
                if(args == NULL)
                    return ALLOC_ERROR;

                func_t *func = curr->token.data.f;
                for(uint8_t i = 0; i < func->nargs; i++) {
                    if(err = evaluate_tokenized(func->args[i], &ev_expr)) {
                        tl_free(args);
                        return err;
                    }
                    tl_append(tokens, ev_expr);
                }

                tl_replace(tokens, index, func->f(args));
                tl_free(args);
            }
            curr = curr->next;
            index++;
        }
    }

    if(tokens->size != 1)
        return REDUCE_ERROR;

    *token = tokens->begin->token;
    return 0;
}
