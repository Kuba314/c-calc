#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include <ctype.h>

#include "operator.h"
#include "token_list.h"
#include "token.h"
#include "error.h"

// tokenizer state enum
enum state_t {
    AWAITING_LUNARY,
    AWAITING_TOKEN,
    READING_SYMBOL,
    READING_NUMBER,
    READING_NUMBER_DECIMAL,
    AWAITING_RUNARY,
    AWAITING_BINARY
};

#define MAX_BRACKET_LVL 10

// bracket macros
#define IS_OPENING_BRACKET(c) (c == '(' || c == '[' || c == '{')
#define IS_CLOSING_BRACKET(c) (c == ')' || c == ']' || c == '}')
#define GET_CLOSING_BRACKET(c) (c == '(' ? ')' : c == '[' ? ']' : '}')

// returns true if char can be part of symbol
static inline bool is_symchar(char c, bool first) {
    if(first && c == '_')
        return true;
    return (!first && c >= '0' && c <= '9') || (c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z');
}
// return closing bracket to the bracket on index
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

// split arguments in function by ','
int _par_safe_split(const char *s, uint32_t s_len, token_list_t **args, uint8_t *nargs) {

    const char *curr = s;
    uint32_t last_i = 0;
    int arg_i = 0;
    int err;
    bool something = false;
    for(uint32_t i = 0; i < s_len; i++) {

        if(!something && !isspace(s[i]))
            something = true;

        // found splitting char, start new token
        if(s[i] == ',') {
            if(arg_i+1 == MAX_FUNC_ARGS - 1)
                return MAXARGS_ERROR(MAX_FUNC_ARGS);

            args[arg_i] = tl_init();
            if(args[arg_i] == NULL)
                return ALLOC_ERROR;

            // tokenize 1 arg
            if((err = tokenize(curr, i - last_i, args[arg_i++])))
                return err;
            curr += i - last_i + 1;
            last_i = ++i;
        }

        // found bracket, skip until closing bracket
        else if(IS_OPENING_BRACKET(s[i])) {
            uint32_t closing = _find_closing_bracket_index(s, i);
            i = closing;
        }
    }

    // init last arg
    args[arg_i] = tl_init();
    if(args[arg_i] == NULL)
        return ALLOC_ERROR;

    // tokenize last arg
    if(something && (err = tokenize(curr, s_len - last_i, args[arg_i])))
        return err;

    // set n args
    *nargs = arg_i + something;
    return 0;
}

// functions that make tokens
static inline token_t mk_treal(long double real) {
    return (token_t) {.type = TT_COMPLEX, .data.c = (complex_t) real};
}
static inline token_t mk_tsymbol(const char *sym, uint32_t length) {
    token_t ret = {.type = TT_SYMBOL};
    strncpy(ret.data.sym, sym, length);
    ret.data.sym[length] = '\0';
    return ret;
}

// function that adds a decimal digit on a certain decimal index
static inline long double add_decimal_digit(long double decimal, char digit, uint8_t nth_decimal) {
    long double new_decimal = digit - '0';
    for(uint8_t i = 0; i < nth_decimal; i++)
        new_decimal /= 10.F;
    return decimal + new_decimal;
}


int tokenize(const char *expr, size_t expr_len, token_list_t *tokens) {

    // state control variables
    enum state_t state = AWAITING_LUNARY;
    unsigned long ix = 0;
    bool need_more = false;
    char c;

    // accumulating data
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

            // if symbol starts
            if(is_symchar(c, true)) {
                if(sym_ix == MAX_SYM_LENGTH)
                    return MAXSYM_ERROR(MAX_SYM_LENGTH);

                symbol[sym_ix++] = tolower(c);
                state = READING_SYMBOL;

            // else if nested expression starts
            } else if(IS_OPENING_BRACKET(c)) {
                int closing_ix = _find_closing_bracket_index(expr, ix);
                token_list_t *new_expr = tl_init();
                if(new_expr == NULL)
                    return ALLOC_ERROR;

                // recursively tokenize nested expression
                if((err = tokenize(expr + ix + 1, closing_ix - ix - 1, new_expr))) {
                    tl_free(new_expr);
                    return (err & EMASK) != ERR_SYNTAX ? err : SYNTAX_ERROR((err << ESHIFT) + (signed) ix + 1);
                }
                tl_append(tokens, (token_t) {.type = TT_EXPRESSION, .data.expr = new_expr});
                ix = closing_ix;
                state = AWAITING_RUNARY;

            // else if number starts
            } else if(isdigit(c)) {
                number = c - '0';
                state = READING_NUMBER;

            // else if decimal starts
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
            // if symbol continues...
            if(is_symchar(c, false)) {
                if(sym_ix == MAX_SYM_LENGTH)
                    return MAXSYM_ERROR(MAX_SYM_LENGTH);
                symbol[sym_ix++] = tolower(c);
            // if '('' found (function)
            } else if(c == '(') {
                int closing_ix = _find_closing_bracket_index(expr, ix);
                function_t f = (function_t) {.args = {NULL}};
                strncpy(f.name, symbol, sym_ix);
                f.name[sym_ix] = '\0';
                sym_ix = 0;

                // tokenize function arguments
                if((err = _par_safe_split(expr + ix + 1, closing_ix - ix - 1, f.args, &f.nargs))) {
                    for(uint8_t i = 0; i < MAX_FUNC_ARGS; i++)
                        if(f.args[i] != NULL)
                            tl_free(f.args[i]);
                    return (err & EMASK) != ERR_SYNTAX ? err : SYNTAX_ERROR((err << ESHIFT) + (signed) ix + 1);
                }

                tl_append(tokens, (token_t) {.type = TT_FUNCTION, .data.f = f});
                ix = closing_ix;
                state = AWAITING_RUNARY;

            // symbol ended, maybe there's a right unary operator next
            } else {
                tl_append(tokens, mk_tsymbol(symbol, sym_ix));
                sym_ix = 0;
                state = AWAITING_RUNARY;
                continue;    // do not advance ix
            }
        }
        // reading whole number part
        else if(state == READING_NUMBER) {
            // if number continues
            if(isdigit(c)) {
                number = number * 10 + c - '0';
            // if whole number just changed to decimal
            } else if(c == '.') {
                decimal = 0.F;
                nth_decimal = 1;
                state = READING_NUMBER_DECIMAL;
            // if symbol right after number (example: 3x)
            } else if(c == '_' || isalpha(c)) {
                tl_append(tokens, mk_treal(number));
                tl_append(tokens, (token_t) {.type = TT_OPERATOR, .data.op = *get_operator('*', OP_BINARY)});
                symbol[0] = c;
                sym_ix = 1;
                state = READING_SYMBOL;
            // number ended, maybe there's a right unary operator next
            } else {
                tl_append(tokens, mk_treal(number));
                state = AWAITING_RUNARY;
                continue;    // do not advance ix
            }
        }
        // reading decimal number part
        else if(state == READING_NUMBER_DECIMAL) {
            // if decimal number continues
            if(isdigit(c)) {
                decimal = add_decimal_digit(decimal, c, nth_decimal++);
            // if symbol right after decimal (example: 3.1x)
            } else if(c == '_' || isalpha(c)) {
                tl_append(tokens, mk_treal(number + decimal));
                tl_append(tokens, (token_t) {.type = TT_OPERATOR, .data.op = *get_operator('*', OP_BINARY)});
                symbol[0] = c;
                sym_ix = 1;
                state = READING_SYMBOL;
            // number ended, maybe there's a right unary operator next
            } else {
                state = AWAITING_RUNARY;
                tl_append(tokens, mk_treal(number + decimal));
                continue;    // do not advance ix
            }
        }
        // waiting for a binary operator
        else if(state == AWAITING_BINARY) {
            operator_t *op = get_operator(c, OP_BINARY);

            // if operator exists
            if(op != NULL) {
                tl_append(tokens, (token_t) {.type = TT_OPERATOR, .data.op = *op});
                state = AWAITING_LUNARY;
                need_more = true;
            } else {
                return SYNTAX_ERROR(ix);
            }
        }

        // move onto next char
        ix++;
    }

    // if last character was left unary or binary operator
    if(need_more)
        return SYNTAX_ERROR(ix - 1);

    // add last token depending on current state
    if(state == READING_SYMBOL) {
        tl_append(tokens, mk_tsymbol(symbol, sym_ix));
    } else if(state == READING_NUMBER) {
        tl_append(tokens, mk_treal(number));
    } else if(state == READING_NUMBER_DECIMAL) {
        tl_append(tokens, mk_treal(number + decimal));
    }

    // if no tokens were found (all spaces)
    if(tokens->size == 0)
        return SYNTAX_ERROR(0);

    return 0;
}
