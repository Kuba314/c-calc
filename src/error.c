#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <stdint.h>

#include "error.h"

void fatal(const char *fmt, ...) {
    va_list args;
    va_start(args, fmt);

    vfprintf(stderr, fmt, args);
    fputc('\n', stderr);
    va_end(args);

    exit(1);
}


void math_perror(int err, const char *err_msg) {
    uint8_t err_type = err & EMASK;

    fprintf(stderr, "%s: ", err_msg);

    if(err_type == ERROR) {
        fprintf(stderr, "unknown error\n");
    } else if(err_type == ERR_ALLOC) {
        fprintf(stderr, "not enough memory\n");
    } else if(err_type == ERR_SYNTAX) {
        fprintf(stderr, "syntax error at index %d\n", err >> ESHIFT);
    } else if(err_type == ERR_REDUCE) {
        fprintf(stderr, "couldn't reduce tokens\n");
    } else if(err_type == ERR_MAXSYM) {
        fprintf(stderr, "symbol exceeded max symbol size %d\n", err >> ESHIFT);
    } else if(err_type == ERR_MAXARGS) {
        fprintf(stderr, "exceeded max function args %d\n", err >> ESHIFT);
    } else if(err_type == ERR_NOSYM) {
        fprintf(stderr, "undefined symbol\n");
    } else if(err_type == ERR_NOFUNC) {
        fprintf(stderr, "undefined function\n");
    } else if(err_type == ERR_OP) {
        fprintf(stderr, "invalid operator(%c) for operands\n", err >> ESHIFT);
    }
}
