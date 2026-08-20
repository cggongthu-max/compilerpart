#ifndef TOKENBUF_H
#define TOKENBUF_H

#include "lexer.h"

/* ----------------------------------------------------------
 * BlockBuf: Ms. Sakura's sparse brace-level metadata
 *   level > 0  → opening brace '{'  (1..3, max nesting)
 *   level < 0  → closing brace '}'  (-1..-3)
 * ---------------------------------------------------------- */
#define MAX_BRACES 256

typedef struct {
    int level;      /* 1..3 or -1..-3 */
    int token_idx;  /* index into tokens[] */
} BlockEntry;

typedef struct {
    BlockEntry entries[MAX_BRACES];
    int count;
} BlockBuf;

/* ----------------------------------------------------------
 * FuncDecl: flat function metadata (no AST nodes)
 * ---------------------------------------------------------- */
#define MAX_FUNCS  16
#define MAX_PARAMS 4

typedef struct {
    char name[64];
    char params[MAX_PARAMS][64];
    int  param_count;
    int  body_start;   /* token idx of '{' */
    int  body_end;     /* token idx of '}' */
} FuncDecl;

typedef struct {
    FuncDecl funcs[MAX_FUNCS];
    int count;
} FuncTable;

/* ----------------------------------------------------------
 * TokenBuf: flat token stream + structural metadata
 * ---------------------------------------------------------- */
typedef struct {
    Token *tokens;
    int    count;
} TokenBuf;

void  tokenbuf_init(const char *source);
void  tokenbuf_free(void);
Token *tokenbuf_get(int idx);
int   tokenbuf_count(void);

/* helper: is this token index outside any function body? */
int   tokenbuf_is_global_scope(int idx);

extern int       *token_match;   /* parallel array: matching () / {} */
extern BlockBuf   block_buf;     /* brace nesting levels */
extern FuncTable  func_table;    /* function declarations */

#endif
