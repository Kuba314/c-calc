

#ifndef __SYMBOL_H__
#define __SYMBOL_H__

#define MAX_SYM_LENGTH 10

typedef struct {
    char sym[MAX_SYM_LENGTH];
    uint32_t length;
} sym_t;

#endif // __SYMBOL_H__
