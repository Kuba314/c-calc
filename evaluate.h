#include "token_list.h"

#ifndef __EVALUATE_H__
#define __EVALUATE_H__

void math_perror(int err, const char *err_msg);
int evaluate_tokenized(token_list_t *tokens, token_t *token);

#endif // __EVALUATE_H__
