#include <stddef.h>
#include <stdbool.h>
#include "token_list.h"
#include "token.h"
#include "function.h"
#include "symbol.h"
#include "error.h"

int evaluate_tokenized(token_list_t *tokens, token_t *token) {

    // loop over every operator priority level
    for(int8_t pr = MAX_PRIORITY; pr >= 0; pr--) {

        uint32_t index = 0;
        token_list_node_t *curr = tokens->begin;
        token_list_node_t *tmp;

        token_t ev_expr;
        int err;

        // loop through every token
        while(curr != NULL) {

            // if operator and correct priority level, evaluate it
            if(curr->token.type == TT_OPERATOR && curr->token.data.op.priority == pr) {
                bool binary = curr->token.data.op.type == OP_BINARY;
                tmp = tl_evalop(tokens, index, &err);
                if(err)
                    return OP_ERROR(curr->token.data.op.sym);
                curr = tmp;
                if(binary)
                    index--;

            // if expression, evaluate it
            } else if(curr->token.type == TT_EXPRESSION) {
                if((err = evaluate_tokenized(curr->token.data.expr, &ev_expr))) {
                    return err;
                }
                curr = tl_replace(tokens, index, ev_expr);

            // if symbol, evaluate it
            } else if(curr->token.type == TT_SYMBOL) {

                // check if function exists
                token_t *token = get_symbol(curr->token.data.sym);
                if(token == NULL)
                    return NOSYM_ERROR;

                // evaluate function with arguments and replace current token
                curr = tl_replace(tokens, index, *token);

            // if function, evaluate it!
            } else if(curr->token.type == TT_FUNCTION) {

                // init argument token list
                token_list_t *args = tl_init();
                if(args == NULL)
                    return ALLOC_ERROR;

                function_t *func_data = &curr->token.data.f;

                // check if function exists
                common_func_t *f = get_function(func_data->name);
                if(f == NULL)
                    return NOFUNC_ERROR;

                // evaluate arguments and load them into argument token list
                for(uint8_t i = 0; i < func_data->nargs; i++) {
                    if((err = evaluate_tokenized(func_data->args[i], &ev_expr))) {
                        tl_free(args);
                        return err;
                    }
                    tl_append(args, ev_expr);
                }

                // evaluate function with arguments and replace current token
                curr = tl_replace(tokens, index, f(args));
                tl_free(args);
            }

            if(curr == NULL)
                return ERROR;

            // move to another token
            curr = curr->next;
            index++;
        }
    }

    // if we don't have 1 token in the end, FSM tokenizer is probably not correct
    if(tokens->size != 1)
        return REDUCE_ERROR;

    // set return token to the last token alive
    *token = tokens->begin->token;
    return 0;
}
