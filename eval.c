#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#include "tokenize.h"
#include "evaluate.h"

#include "token_list.h"
#include "token.h"
#include "error.h"
#include "complex.h"

void replace_smaller(char *string, const char *seq, const char *new_seq) {
    size_t len = strlen(seq);
    size_t new_len = strlen(new_seq);
    size_t delta = len - new_len;
    if(new_len > len)
        return;

    size_t shift = 0;
    do {
        if(strncmp(string, seq, len) == 0) {
            strncpy(string, new_seq, new_len);
            shift += delta;
            string += delta;
        } else if(shift) {
            string[-shift] = string[0];
        }
    } while(*++string);
    string[-shift] = '\0';
}

int main(int argc, char *argv[]) {

    // check args
    if(argc != 2)
        fatal("Usage: %s expression", argv[0]);

    fprintf(stderr, "Evaluating: '%s'\n", argv[1]);

    // replace √ (3 bytes) with # (1 byte) for easier calculations
    replace_smaller(argv[1], "√", "#");

    // init tokens
    token_list_t *tokens = tl_init();
    if(tokens == NULL)
        fatal("Couldn't allocate tokenlist\n");

    // tokenize
    int error = tokenize(argv[1], strlen(argv[1]), tokens);
    if(error) {
        tl_free(tokens);
        math_perror(error, "Could not tokenize");
        exit(0);
    }

    // evaluate
    token_t result;
    error = evaluate_tokenized(tokens, &result);
    if(error) {
        math_perror(error, "Could not evaluate expression");
        if((error & EMASK) == ERR_REDUCE)
            tl_print(tokens);
        tl_free(tokens);
        exit(0);
    }
    tl_free(tokens);

    // print result
    if(result.type == TT_REAL) {
        printf("result = %Lf\n", result.data.d);
    } else if(result.type == TT_COMPLEX) {
        printf("result = %Lf %c %Lfi\n", creall(result.data.c), (cimagl(result.data.c) < 0) ? '-' : '+', fabsl(cimagl(result.data.c)));
    } else {
        fatal("Mathlib returned nan result: %d\n", result.type);
    }

    return 0;
}
