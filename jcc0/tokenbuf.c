#include "tokenbuf.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static TokenBuf tb;
int       *token_match;
BlockBuf   block_buf;
FuncTable  func_table;

void tokenbuf_init(const char *source)
{
    /* ---- Pass 1a: lex everything into a flat array ---- */
    lexer_init(source);
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
                token_match[open] = i;
                token_match[i]   = open;
            }
        }
    }
    free(stack);

    /* ---- Pass 1c: build BlockBuf (brace levels, max depth 3) ---- */
    memset(&block_buf, 0, sizeof(block_buf));
    int cur_level = 0;
    for (int i = 0; i < tb.count; i++) {
        if (tb.tokens[i].type == TK_LBRACE) {
            if (cur_level < 3 && block_buf.count < MAX_BRACES) {
                cur_level++;
                block_buf.entries[block_buf.count].level = cur_level;
                block_buf.entries[block_buf.count].token_idx = i;
                block_buf.count++;
            }
        }
        else if (tb.tokens[i].type == TK_RBRACE) {
            if (cur_level > 0 && block_buf.count < MAX_BRACES) {
                block_buf.entries[block_buf.count].level = -cur_level;
                block_buf.entries[block_buf.count].token_idx = i;
                block_buf.count++;
                cur_level--;
            }
        }
    }

    /* ---- Pass 1d: identify function declarations ----
     * Pattern: int ident ( [int param [, ...]] ) { ... }
     * Anything else at global scope is a global variable.
     */
    memset(&func_table, 0, sizeof(func_table));
    int p = 0;
    while (p < tb.count) {
        Token *t0 = tokenbuf_get(p);
        if (!t0 || t0->type == TK_EOF) break;

        if (t0->type == TK_INT) {
            Token *t1 = tokenbuf_get(p + 1);
            Token *t2 = tokenbuf_get(p + 2);

            /* Function declaration? */
            if (t1 && t1->type == TK_IDENT && t2 && t2->type == TK_LPAREN) {
                if (func_table.count >= MAX_FUNCS) {
                    fprintf(stderr, "Error: too many functions (max %d)\n", MAX_FUNCS);
                    exit(1);
                }
                FuncDecl *f = &func_table.funcs[func_table.count++];
                strncpy(f->name, t1->text, 63);
                f->name[63] = '\0';

                /* Parse parameter list: int param1, int param2, ... */
                int q = p + 3; /* skip past '(' */
                f->param_count = 0;
                while (q < tb.count) {
                    Token *pt = tokenbuf_get(q);
                    if (!pt || pt->type == TK_RPAREN) break;
                    if (pt->type == TK_INT) {
                        Token *pname = tokenbuf_get(q + 1);
                        if (pname && pname->type == TK_IDENT
                            && f->param_count < MAX_PARAMS) {
                            strncpy(f->params[f->param_count++], pname->text, 63);
                        }
                        q += 2; /* skip int ident */
                        Token *comma = tokenbuf_get(q);
                        if (comma && comma->type == TK_COMMA) q++;
                    } else {
                        break;
                    }
                }

                /* Locate '{' after matching ')' */
                int r = p + 3;
                while (r < tb.count && tokenbuf_get(r)->type != TK_RPAREN) r++;
                r++; /* skip ')' */
                if (r < tb.count && tokenbuf_get(r)->type == TK_LBRACE) {
                    f->body_start = r;
                    f->body_end   = token_match[r];
                }

                p = f->body_end + 1; /* jump past entire function body */
                continue;
            }
            /* Global variable: int ident ; */
            else if (t1 && t1->type == TK_IDENT && t2 && t2->type == TK_SEMI) {
                p += 3;
                continue;
            }
        }
        p++;
    }
}

Token *tokenbuf_get(int idx)
{
    if (idx >= 0 && idx < tb.count) return &tb.tokens[idx];
    return NULL;
}

int tokenbuf_count(void) { return tb.count; }

int tokenbuf_is_global_scope(int idx)
{
    for (int f = 0; f < func_table.count; f++) {
        if (idx >= func_table.funcs[f].body_start && idx <= func_table.funcs[f].body_end)
            return 0;
    }
    return 1;
}

void tokenbuf_free(void)
{
    free(tb.tokens);
    free(token_match);
    /* block_buf & func_table live on the stack, no dynamic cleanup */
}
