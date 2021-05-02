#include <stddef.h>
#include <stdbool.h>
#include <stdlib.h>
#include <time.h>

#include "token_list.h"
#include "token.h"
#include "function.h"
#include "symbol.h"
#include "error.h"

static int call_function(const struct func_def_t *func, const token_list_t *args, uint8_t nargs, enum token_type_t ttype, token_t *result) {

    const token_list_node_t *curr = args->begin;

    // handle missing functions
    if(ttype == TT_REAL && (
           (nargs == 0 && func->d0 == NULL)
        || (nargs == 1 && func->d1 == NULL)
        || (nargs == 2 && func->d2 == NULL)
        || (nargs == 3 && func->d3 == NULL)))
        ttype = TT_COMPLEX;
    if(ttype == TT_COMPLEX && (
           (nargs == 0 && func->c0 == NULL)
        || (nargs == 1 && func->c1 == NULL)
        || (nargs == 2 && func->c2 == NULL)
        || (nargs == 3 && func->c3 == NULL)))
        return NOFUNC_ERROR;

    if(ttype == TT_REAL) {
        if(nargs == 0) {
            *result = (token_t) {.type=TT_REAL, .data.d=func->d0()};
            return 0;
        }
        long double a = curr->token.data.d;
        curr = curr->next;
        if(nargs == 1) {
            *result = (token_t) {.type=TT_REAL, .data.d=func->d1(a)};
            return 0;
        }
        long double b = curr->token.data.d;
        curr = curr->next;
        if(nargs == 2) {
            *result = (token_t) {.type=TT_REAL, .data.d=func->d2(a, b)};
            return 0;
        }
        long double c = curr->token.data.d;
        if(nargs == 3) {
            *result = (token_t) {.type=TT_REAL, .data.d=func->d3(a, b, c)};
            return 0;
        }
    } else {
        if(nargs == 0) {
            *result = (token_t) {.type=TT_COMPLEX, .data.c=func->c0()};
            return 0;
        }
        complex_t a = curr->token.data.c;
        curr = curr->next;
        if(nargs == 1) {
            *result = (token_t) {.type=TT_COMPLEX, .data.c=func->c1(a)};
            return 0;
        }
        complex_t b = curr->token.data.c;
        curr = curr->next;
        if(nargs == 2) {
            *result = (token_t) {.type=TT_COMPLEX, .data.c=func->c2(a, b)};
            return 0;
        }
        complex_t c = curr->token.data.c;
        if(nargs == 3) {
            *result = (token_t) {.type=TT_COMPLEX, .data.c=func->c3(a, b, c)};
            return 0;
        }
    }
    return ERROR;
}

int evaluate_tokenized(token_list_t *tokens, token_t *token) {

    srand(time(NULL));

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
                const struct func_def_t *f = get_function(func_data->name, func_data->nargs);
                if(f == NULL)
                    return NOFUNC_ERROR;

                // evaluate arguments and load them into argument token list
                enum token_type_t ttype = TT_REAL;
                for(uint8_t i = 0; i < func_data->nargs; i++) {
                    if((err = evaluate_tokenized(func_data->args[i], &ev_expr))) {
                        tl_free(args);
                        return err;
                    }
                    if(ev_expr.type == TT_COMPLEX)
                        ttype = TT_COMPLEX;
                    tl_append(args, ev_expr);
                }

                if((err = call_function(f, args, func_data->nargs, ttype, &ev_expr))) {
                    tl_free(args);
                    return err;
                }

                // evaluate function with arguments and replace current token
                curr = tl_replace(tokens, index, ev_expr);
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
