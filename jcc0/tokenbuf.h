#ifndef TOKENBUF_H
#define TOKENBUF_H

#include "lexer.h"

/* Flat token array with a match table.
 * match[i] = index of the matching ) or } for a ( or { at i.
 * This is the "sparse tree" metadata: it tells us the span of
 * every parenthesized expression and every block without building nodes. */
typedef struct {
    Token *tokens;
    int    count;
} TokenBuf;

void  tokenbuf_init(const char *source);
void  tokenbuf_free(void);
Token *tokenbuf_get(int idx);   /* NULL if out of bounds */
int   tokenbuf_count(void);

extern int *token_match;        /* parallel array: structural jumps */

#endif
