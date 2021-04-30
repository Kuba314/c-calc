#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include <ctype.h>
#include "token_list.h"
#include "token.h"
#include "operator.h"
#include "function.h"
#include "error.h"

enum state_t {
    AWAITING_LUNARY,
    AWAITING_TOKEN,
    READING_SYMBOL,
    READING_NUMBER,
    READING_NUMBER_DECIMAL,
    AWAITING_RUNARY,
    AWAITING_BINARY
};

void prints(const char *s, uint32_t s_len) {
    printf("s = \"");
    for(uint32_t i = 0; i < s_len; i++)
        printf("%c", s[i]);
    printf("\"\n");
}

#define MAX_BRACKET_LVL 10

#define IS_OPENING_BRACKET(c) (c == '(' || c == '[' || c == '{')
#define IS_CLOSING_BRACKET(c) (c == ')' || c == ']' || c == '}')
#define GET_CLOSING_BRACKET(c) (c == '(' ? ')' : c == '[' ? ']' : '}')

static inline bool is_symchar(char c, bool first) {
    if(first && c == '_')
        return true;
    return (!first && c >= '0' && c <= '9') || (c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z');
}
int _find_closing_bracket_index(const char *s, uint32_t index) {
    char brackets[MAX_BRACKET_LVL+1] = {s[index], '\0'};
    uint32_t b_ix = 1;

    if(!IS_OPENING_BRACKET(brackets[0]))
        return -1;

    index++;
    size_t str_size = strlen(s);
    for(unsigned i = index; i < str_size; i++) {
        if(s[i] == GET_CLOSING_BRACKET(brackets[b_ix-1])) {
            if(--b_ix == 0)
                return i;
        } else if(IS_CLOSING_BRACKET(s[i])) {
            return -2;
        } else if(IS_OPENING_BRACKET(s[i])) {
            brackets[b_ix++] = s[i];
        }
    }

    return -2;
}
int tokenize(const char *expr, size_t expr_len, token_list_t *tokens);
int _par_safe_split(const char *s, uint32_t s_len, token_list_t **args, uint8_t *nargs) {

    // printf("splitting params %d\n", s_len);
    // prints(s, s_len);

    const char *curr = s;
    uint32_t last_i = 0;
    int arg_i = 0;
    int err;
    for(uint32_t i = 0; i < s_len; i++) {

        // found splitting char, start new token
        if(s[i] == ',') {
            // printf("found , on %d\n", i);
            if(++arg_i == MAX_FUNC_ARGS - 1)
                return MAXARGS_ERROR(MAX_FUNC_ARGS);

            args[arg_i] = tl_init();
            if(args[arg_i] == NULL)
                return ALLOC_ERROR;

            if((err = tokenize(curr, i - last_i, args[arg_i++])))
                return err;
            last_i = ++i;
        }
        // found bracket, skip until closing bracket
        else if(IS_OPENING_BRACKET(s[i])) {
            uint32_t closing = _find_closing_bracket_index(s, i);
            i = closing;
        }
    }

    // add last token
    args[arg_i] = tl_init();
    if(args[arg_i] == NULL)
        return ALLOC_ERROR;

    if((err = tokenize(curr, s_len - last_i, args[arg_i])))
        return err;
    *nargs = arg_i + 1;
    return 0;
}

static inline token_t mk_treal(long double real) {
    return (token_t) {.type = TT_REAL, .data.d = real};
}
static inline token_t mk_tcomplex(long double real, long double imag) {
    return (token_t) {.type = TT_COMPLEX, .data.c = (complex_t) {.real = real, .imag = imag}};
}
static inline token_t mk_tsymbol(const char *sym) {
    token_t ret = {.type = TT_SYMBOL};
    strcpy(ret.data.sym, sym);
    return ret;
}

static inline long double add_decimal_digit(long double decimal, char digit, uint8_t nth_decimal) {
    long double new_decimal = digit - '0';
    for(uint8_t i = 0; i < nth_decimal; i++)
        new_decimal /= 10.F;
    return decimal + new_decimal;
}

int tokenize(const char *expr, size_t expr_len, token_list_t *tokens) {

    // prints(expr, expr_len);

    enum state_t state = AWAITING_LUNARY;
    unsigned long ix = 0;
    bool need_more = false;
    char c;

    char symbol[MAX_SYM_LENGTH+1];
    uint32_t sym_ix = 0;
    long long number;
    long double decimal;
    uint8_t nth_decimal;

    int err;
    while(ix < expr_len) {
        c = expr[ix];

        // ignore spaces
        if isspace(c) {
            ix++;
            continue;
        }

        need_more = false;

        // might find a left unary operator
        if(state == AWAITING_LUNARY) {
            operator_t *op = get_operator(c, OP_LUNARY);
            if(op != NULL) {
                tl_append(tokens, (token_t) {.type = TT_OPERATOR, .data.op = *op});
                ix++;
                need_more = true;
            }
            state = AWAITING_TOKEN;
            continue;
        }
        // might find a right unary operator
        else if(state == AWAITING_RUNARY) {
            operator_t *op = get_operator(c, OP_RUNARY);
            if(op != NULL) {
                tl_append(tokens, (token_t) {.type = TT_OPERATOR, .data.op = *op});
                ix++;
            }
            state = AWAITING_BINARY;
            continue;
        }

        // waiting for any token
        if(state == AWAITING_TOKEN) {
            if(is_symchar(c, true)) {
                if(sym_ix == MAX_SYM_LENGTH)
                    return MAXSYM_ERROR(MAX_SYM_LENGTH);

                symbol[sym_ix++] = tolower(c);
                state = READING_SYMBOL;
            } else if(IS_OPENING_BRACKET(c)) {
                int closing_ix = _find_closing_bracket_index(expr, ix);
                token_list_t *new_expr = tl_init();
                if(new_expr == NULL)
                    return ALLOC_ERROR;

                if((err = tokenize(expr + ix + 1, closing_ix - ix - 1, new_expr))) {
                    tl_free(new_expr);
                    return (err & EMASK) != ERR_SYNTAX ? err : SYNTAX_ERROR((err << ESHIFT) + (signed) ix + 1);
                }
                tl_append(tokens, (token_t) {.type = TT_EXPRESSION, .data.expr = new_expr});
                ix = closing_ix;
                state = AWAITING_RUNARY;
            } else if(isdigit(c)) {
                number = c - '0';
                state = READING_NUMBER;
            } else if(c == '.') {
                number = 0LL;
                decimal = 0.F;
                nth_decimal = 1;
                state = READING_NUMBER_DECIMAL;
            } else {
                return SYNTAX_ERROR(ix);
            }
        }
        // reading symbol or function
        else if(state == READING_SYMBOL) {
            if(is_symchar(c, false)) {
                if(sym_ix == MAX_SYM_LENGTH)
                    return MAXSYM_ERROR(MAX_SYM_LENGTH);
                symbol[sym_ix++] = tolower(c);
            } else if(c == '(') {
                int closing_ix = _find_closing_bracket_index(expr, ix);
                func_t f = (func_t) {.args = {NULL}};
                strncpy(f.name, symbol, sym_ix);
                f.name[sym_ix] = '\0';
                if((err = _par_safe_split(expr + ix + 1, closing_ix - ix - 1, f.args, &f.nargs))) {
                    for(uint8_t i = 0; i < MAX_FUNC_ARGS; i++)
                        if(f.args[i] != NULL)
                            tl_free(f.args[i]);
                    return (err & EMASK) != ERR_SYNTAX ? err : SYNTAX_ERROR((err << ESHIFT) + (signed) ix + 1);
                }
                printf("should be %d\n", f.nargs);
                // strcpy(f.name, symbol);
                // uint8_t i;
                // const char *arg_begin = expr + ix + 1;
                // uint32_t arg_length;
                // for(i = 0; i < MAX_FUNC_ARGS; i++) {
                //     f.args[i] = tl_init();
                //     if(f.args[i] == NULL) {
                //         for(uint32_t j = 0; j < i; j++)
                //             tl_free(f.args[j]);
                //         return ALLOC_ERROR;
                //     }
                //     bool last = _find_next_param(arg_begin, closing_ix - (arg_begin - s), &arg_length);
                //     if((err = tokenize(arg_begin, arg_length, fl.args[i]))) {
                //         for(uint32_t j = 0; j < i; j++)
                //             tl_free(f.args[j]);
                //         return err_type != ERR_SYNTAX ? err : SYNTAX_ERROR(err << ESHIFT + ix + 1);
                //     }
                //     arg_begin += arg_length;

                //     if(last)
                //         break;
                // }
                // if(i == MAX_FUNC_ARGS)
                //     return MAXARGS_ERROR(MAX_FUNC_ARGS);

                tl_append(tokens, (token_t) {.type = TT_FUNCTION, .data.f = f});
                ix = closing_ix;
                state = AWAITING_RUNARY;
                // try:
                //     arguments = _par_safe_split(expr[ix + 1:closing_ix], ',')
                //     tokens += [(symbol, tuple(tokenize(part) for part in arguments if part))]
                // except EvalSyntaxError as e:
                //     raise EvalSyntaxError(ix + e.char_ix + 1)
                // ix = closing_ix
                // state = AWAITING_RUNARY
            } else {
                tl_append(tokens, mk_tsymbol(symbol));
                state = AWAITING_RUNARY;
                continue;    // do not advance ix
            }
        }
        // reading whole number part
        else if(state == READING_NUMBER) {
            if(isdigit(c)) {
                number = number * 10 + c - '0';
            } else if(c == '.') {
                decimal = 0.F;
                nth_decimal = 1;
                state = READING_NUMBER_DECIMAL;
            } else if(c == '_' || isalpha(c)) {
                tl_append(tokens, mk_treal(number));
                tl_append(tokens, (token_t) {.type = TT_OPERATOR, .data.op = *get_operator('*', OP_BINARY)});
                symbol[0] = c;
                sym_ix = 1;
                state = READING_SYMBOL;
            } else {
                tl_append(tokens, mk_treal(number));
                state = AWAITING_RUNARY;
                continue;    // do not advance ix
            }
        }
        // reading decimal number part
        else if(state == READING_NUMBER_DECIMAL) {
            if(isdigit(c)) {
                decimal = add_decimal_digit(decimal, c, nth_decimal++);
            } else if(c == '_' || isalpha(c)) {
                tl_append(tokens, mk_treal(number + decimal));
                tl_append(tokens, (token_t) {.type = TT_OPERATOR, .data.op = *get_operator('*', OP_BINARY)});
                symbol[0] = c;
                sym_ix = 1;
                state = READING_SYMBOL;
            } else {
                state = AWAITING_RUNARY;
                tl_append(tokens, mk_treal(number + decimal));
                continue;    // do not advance ix
            }
        }
        // waiting for a binary operator
        else if(state == AWAITING_BINARY) {
            operator_t *op = get_operator(c, OP_BINARY);
            if(op != NULL) {
                tl_append(tokens, (token_t) {.type = TT_OPERATOR, .data.op = *op});
                state = AWAITING_LUNARY;
                need_more = true;
            } else {
                return SYNTAX_ERROR(ix);
            }
        }
        ix++;
    }

    // if last character was left unary or binary operator
    if(need_more)
        return SYNTAX_ERROR(ix - 1);

    // add last token depending on current state
    if(state == READING_SYMBOL) {
        tl_append(tokens, mk_tsymbol(symbol));
    } else if(state == READING_NUMBER) {
        tl_append(tokens, mk_treal(number));
    } else if(state == READING_NUMBER_DECIMAL) {
        tl_append(tokens, mk_treal(number + decimal));
    }

    if(tokens->size == 0)
        return SYNTAX_ERROR(0);
    printf("tokenized:\n");
    tl_print(tokens);
    return 0;
}
