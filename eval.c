#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "tokenize.h"
#include "evaluate.h"

#include "token_list.h"
#include "token.h"
#include "error.h"



int main(int argc, char *argv[]) {

    if(argc != 2)
        fatal("Usage: %s expression", argv[0]);

    fprintf(stderr, "Evaluating: '%s'\n", argv[1]);

    token_list_t *tokens = tl_init();
    if(tokens == NULL)
        fatal("Couldn't allocate tokenlist\n");

    int error = tokenize(argv[1], strlen(argv[1]), tokens);
    if(error) {
        tl_free(tokens);
        math_perror(error, "Could not tokenize");
        exit(0);
    }

    // tl_print(tokens);

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

    if(result.type == TT_REAL) {
        printf("result = %Lf\n", result.data.d);
    } else if(result.type == TT_COMPLEX) {
        printf("result = %Lf %c %Lfi\n", result.data.c.real, (result.data.c.imag < 0) ? '-' : '+', result.data.c.imag);
    } else {
        fatal("Mathlib returned nan result: %d\n", result.type);
    }
    return 0;
}
