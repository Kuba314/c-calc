#include <stdio.h>
#include <stdlib.h>

#include "token_list.h"
#include "token.h"

token_list_t *tl_init() {
    token_list_t *tl = malloc(sizeof(token_list_t));
    if(tl == NULL)
        return NULL;

    tl->size = 0;
    tl->begin = NULL;
    return tl;
}

static token_list_node_t *_init_empty_node() {
    token_list_node_t *node = malloc(sizeof(token_list_node_t));
    if(node == NULL)
        return NULL;

    node->next = NULL;
    return node;
}
static token_list_node_t *_init_node(token_t token) {
    token_list_node_t *node = _init_empty_node();
    if(node == NULL)
        return NULL;

    node->token = token;
    return node;
}
static void _free_node(token_list_node_t *node) {
    if(node->token.type == TT_EXPRESSION)
        tl_free(node->token.data.expr);
    else if(node->token.type == TT_FUNCTION)
        for(uint8_t i = 0; i < MAX_FUNC_ARGS; i++)
            if(node->token.data.f.args[i] != NULL)
                tl_free(node->token.data.f.args[i]);

    free(node);
}

token_list_node_t *tl_append(token_list_t *tl, token_t token) {

    token_list_node_t *node = _init_node(token);
    if(node == NULL)
        return NULL;

    if(tl->begin == NULL) {
        tl->begin = node;
    } else {
        token_list_node_t *tmp = tl->begin;
        while(tmp->next != NULL)
            tmp = tmp->next;

        tmp->next = node;
    }
    tl->size++;
    return node;
}

token_list_node_t *tl_evalop(token_list_t *tl, uint32_t index) {

    token_list_node_t *first = tl->begin;
    token_list_node_t *last = NULL;
    token_list_node_t *last2 = NULL;

    if(first == NULL)
        return NULL;

    // find operator index
    for(uint32_t i = 0; i < index; i++) {
        last2 = last;
        last = first;
        if((first = first->next) == NULL)
            return NULL;
    }
    if(first->token.type != TT_OPERATOR)
        return NULL;

    // check if operator has sufficient arguments
    operator_t op = first->token.data.op;
    if((index == 0 && op.type != OP_LUNARY) || (index == tl->size-1 && op.type != OP_RUNARY))
        return NULL;

    token_list_node_t *new_node = _init_empty_node();
    if(new_node == NULL)
        return NULL;

    token_t *new_token = &new_node->token;
    if(op.type == OP_LUNARY) {
        if(first->next->token.type == TT_REAL) {
            new_token->type = TT_REAL;
            new_token->data.d = op.fd.d1(first->next->token.data.d);
        } else {
            new_token->type = TT_COMPLEX;
            new_token->data.c = op.fc.c1(first->next->token.data.c);
        }
        _free_node(first);
        _free_node(first->next);
        if(last == NULL)
            tl->begin = new_node;
        else
            last->next = new_node;
        new_node->next = first->next->next;
    } else if(op.type == OP_RUNARY) {
        if(last->token.type == TT_REAL) {
            new_token->type = TT_REAL;
            new_token->data.d = op.fd.d1(last->token.data.d);
        } else {
            new_token->type = TT_COMPLEX;
            new_token->data.c = op.fc.c1(last->token.data.c);
        }
        _free_node(last);
        _free_node(first);
        if(last2 == NULL)
            tl->begin = new_node;
        else
            last2->next = new_node;
        new_node->next = first->next;
    } else {
        token_t *left_token = &last->token;
        token_t *right_token = &first->next->token;
        enum token_type_t tt = TT_REAL;
        if(left_token->type == TT_COMPLEX && right_token->type == TT_REAL) {
            tt = TT_COMPLEX;
            right_token->data.c = mk_complex(right_token->data.d, 0.0F);
            // right_token->type == TT_COMPLEX;
        } else if(left_token->type == TT_REAL && right_token->type == TT_COMPLEX) {
            tt = TT_COMPLEX;
            left_token->data.c = mk_complex(left_token->data.d, 0.0F);
            // left_token->type == TT_COMPLEX;
        }
        if(tt == TT_REAL) {
            new_token->type = TT_REAL;
            new_token->data.d = op.fd.d2(left_token->data.d, right_token->data.d);
        } else {
            new_token->type = TT_COMPLEX;
            new_token->data.c = op.fc.c2(left_token->data.c, right_token->data.c);
        }
        new_node->next = first->next->next;
        _free_node(last);
        _free_node(first->next);
        _free_node(first);
        if(last2 == NULL)
            tl->begin = new_node;
        else
            last2->next = new_node;
    }
    tl->size -= 1 + (op.type == OP_BINARY);
    return new_node;
}

// token_list_node_t *tl_squash(token_list_t *tl, uint32_t from, uint32_t to, token_t token) {
//     if(from >= to)
//         return NULL;

//     token_list_node_t *first = tl->begin;
//     token_list_node_t *last = NULL;

//     if(first == NULL)
//         return NULL;

//     for(uint32_t i = 0; i < from; i++) {
//         last = first;
//         if((first = first->next) == NULL)
//             return NULL;
//     }

//     token_list_node_t *tmp;
//     for(uint32_t i = from; i < to; i++) {
//         tmp = first->next;
//         _free_node(first);
//         first = tmp;
//     }

//     token_list_node_t *node = _init_node(token);
//     if(node == NULL)
//         return NULL;

//     node->next = first;

//     if(last == NULL)
//         tl->begin = node;
//     else
//         last->next = node;
//     tl->size -= to - from - 1;
//     return node;
// }

token_list_node_t *tl_replace(token_list_t *tl, uint32_t index, token_t token) {

    token_list_node_t *curr = tl->begin;
    token_list_node_t *last = NULL;

    if(curr == NULL)
        return NULL;

    for(uint32_t i = 0; i < index; i++) {
        last = curr;
        if((curr = curr->next) == NULL)
            return NULL;
    }
    

    token_list_node_t *node = _init_node(token);
    if(node == NULL)
        return NULL;

    node->next = curr->next;
    if(last == NULL)
        tl->begin = node;
    else
        last->next = node;
    _free_node(curr);
    return node;
}

bool tl_remove(token_list_t *tl, uint32_t index) {

    if(tl->begin == NULL)
        return false;

    token_list_node_t *curr = tl->begin;
    token_list_node_t *last = NULL;

    while(index--) {
        if(curr->next == NULL)
            return false;

        last = curr;
        curr = curr->next;
    }

    if(last == NULL)
        tl->begin = curr->next;
    else
        last->next = curr->next;

    _free_node(curr);
    tl->size--;
    return true;
}

void tl_print(token_list_t *tl) {
    token_list_node_t *curr = tl->begin;

    static int depth = 0;
    static bool is_exp = false;
    depth++;

    printf("tokenlist[%u]\n", tl->size);
    uint32_t i = 0;
    while(curr != NULL) {
        for(int i = 0; i < depth; i++)
            printf("  ");

        token_t *t = &curr->token;
        switch(t->type) {
        case TT_REAL:
            printf("[%u] Real(%.02Lf)", i++, t->data.d);
            break;
        case TT_COMPLEX:
            printf("[%u] Complex(%.02Lf, %.02Lf)", i++, t->data.c.real, t->data.c.imag);
            break;
        case TT_OPERATOR:
            printf("[%u] Op(%c)", i++, t->data.op.sym);
            break;
        case TT_SYMBOL:
            printf("[%u] Sym(%s)", i++, t->data.sym);
            break;
        case TT_FUNCTION:
            printf("[%u] Function(%s, %u)", i++, t->data.f.name, t->data.f.nargs);
            break;
        case TT_EXPRESSION:
            is_exp = true;
            printf("Expression(");
            tl_print(t->data.expr);
            printf(")\n");
            is_exp = false;
            break;
        }
        printf(is_exp ? ", " : "\n");
        curr = curr->next;
    }
    depth--;
}

void tl_free(token_list_t *tl) {
    token_list_node_t *curr = tl->begin;
    token_list_node_t *tmp = NULL;

    while(curr != NULL) {
        tmp = curr->next;
        _free_node(curr);
        curr = tmp;
    }
    free(tl);
}
