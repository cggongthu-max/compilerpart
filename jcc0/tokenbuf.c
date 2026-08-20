#include "tokenbuf.h"
#include <stdlib.h>

static TokenBuf tb;
int *token_match;

/* In JCC0, instead of using ASTs, we use a flat buffer tokenbuf
   to store information for parsing. To think about tokenbuf,
   simply think what information you need while iterating on
   ASTs in later phases, and store that information in a flat
   buffer. For example, you need to know that a block contains
   5 statements, then you would need to store that flatly, while
   using ASTs you would put it as a list. Storing information
   flatly is easier and more efficient representation. Source
   code for compilation would usually be not too large, and a
   tokenbuf stores information compactly.
*/

void tokenbuf_init(const char *source)
{
    lexer_init(source);

    /* ---- Pass 1a: lex everything into a flat array ---- */
    tb.count = 0;
    int cap  = 256;
    tb.tokens = malloc(cap * sizeof(Token));

    Token t;
    do {
        t = next_token();
        if (tb.count >= cap) {
            cap *= 2;
            tb.tokens = realloc(tb.tokens, cap * sizeof(Token));
        }
        tb.tokens[tb.count++] = t;
    } while (t.type != TK_EOF);

    /* ---- Pass 1b: build match table (sparse tree metadata) ---- */
    token_match = calloc(tb.count, sizeof(int));
    for (int i = 0; i < tb.count; i++) token_match[i] = -1;

    int *stack = malloc(tb.count * sizeof(int));
    int sp = 0;

    for (int i = 0; i < tb.count; i++) {
        if (tb.tokens[i].type == TK_LPAREN || tb.tokens[i].type == TK_LBRACE) {
            stack[sp++] = i;
        }
        else if (tb.tokens[i].type == TK_RPAREN || tb.tokens[i].type == TK_RBRACE) {
            if (sp > 0) {
                int open = stack[--sp];
                token_match[open] = i;   /* forward link */
                token_match[i]   = open; /* backward link */
            }
        }
    }
    free(stack);
}

Token *tokenbuf_get(int idx)
{
    if (idx >= 0 && idx < tb.count) return &tb.tokens[idx];
    return NULL;
}

int tokenbuf_count(void) { return tb.count; }

void tokenbuf_free(void)
{
    free(tb.tokens);
    free(token_match);
}
