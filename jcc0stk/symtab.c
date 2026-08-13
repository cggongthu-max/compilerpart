/* symtab.c */
#include <string.h>

typedef struct {
    char name[64];
    int reg;          /* MMIX register number, e.g. 1 means $1 */
} VarInfo;

static VarInfo symtab[64];
static int symtab_count = 0;
static int next_reg = 1;   /* $0 is often avoided; $255 is max */

/* Find existing variable, or -1 */
int find_var(const char *name) {
    for (int i = 0; i < symtab_count; i++)
        if (strcmp(symtab[i].name, name) == 0)
            return symtab[i].reg;
    return -1;
}

/* Allocate a new register for a variable */
int alloc_var(const char *name) {
    int r = next_reg++;
    strcpy(symtab[symtab_count].name, name);
    symtab[symtab_count].reg = r;
    symtab_count++;
    return r;
}
