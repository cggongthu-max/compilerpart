#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "lexer.h"
#include "tokenbuf.h"
#include "symtab.h"
#include "gen.h"

static char *read_file(const char *path)
{
    FILE *fp = fopen(path, "r");
    if (!fp) { perror(path); exit(1); }
    fseek(fp, 0, SEEK_END);
    long len = ftell(fp);
    fseek(fp, 0, SEEK_SET);
    char *buf = malloc(len + 1);
    fread(buf, 1, len, fp);
    buf[len] = '\0';
    fclose(fp);
    return buf;
}

/* First pass over the token stream: collect declarations for register allocation.
 * No AST needed — we just walk the flat token array. */
static void register_decls(SymTab *st)
{
    int p = 0;
    while (tokenbuf_get(p) && tokenbuf_get(p)->type == TK_INT) {
        p++; /* skip 'int' */
        Token *ident = tokenbuf_get(p);
        if (!ident || ident->type != TK_IDENT) {
            fprintf(stderr, "Error: expected identifier after 'int'\n");
            exit(1);
        }
        if (symtab_add(st, ident->text) < 0) {
            fprintf(stderr, "Error: too many variables\n");
            exit(1);
        }
        p++; /* skip ident */
        p++; /* skip ';'  */
    }
}

int main(int argc, char **argv)
{
    const char *source;

    if (argc >= 2) {
        source = read_file(argv[1]);
    } else {
        source =
            "int s; int i;\n"
            "s = 0; i = 1;\n"
            "while (i <= 100) {\n"
            "    s = s + i;\n"
            "    i = i + 1;\n"
            "}\n";
    }

    /* 1. Lex + build structural metadata (match table) */
    tokenbuf_init(source);

    /* 2. Register variables */
    SymTab symtab;
    symtab_init(&symtab);
    register_decls(&symtab);

    /* 3. Parse & generate in one walk — the call stack IS the tree */
    generate(&symtab);

    /* 4. Clean up */
    tokenbuf_free();
    return 0;
}
