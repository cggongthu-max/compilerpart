#include "symtab.h"
#include <string.h>

/*
 * 寄存器分配方案：
 *   $0       保留
 *   $1       保留
 *   $2 ~     按声明顺序分配给变量
 *   $200 ~   表达式计算的临时寄存器（由 codegen 管理）
 */
#define FIRST_VAR_REG  2

void symtab_init(SymTab *st) {
    st->count = 0;
}

int symtab_add(SymTab *st, const char *name) {
    if (st->count >= SYMTAB_MAX) return -1;
    Symbol *s = &st->entries[st->count];
    strncpy(s->name, name, 63);
    s->name[63] = '\0';
    s->reg = FIRST_VAR_REG + st->count;
    st->count++;
    return s->reg;
}

int symtab_lookup(SymTab *st, const char *name) {
    for (int i = 0; i < st->count; i++) {
        if (strcmp(st->entries[i].name, name) == 0)
            return st->entries[i].reg;
    }
    return -1;
}
