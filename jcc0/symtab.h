#ifndef SYMTAB_H
#define SYMTAB_H

#define SYMTAB_MAX 64

typedef struct {
    char name[64];
    int  reg;          /* 分配的 MMIX 寄存器编号 */
} Symbol;

typedef struct {
    Symbol entries[SYMTAB_MAX];
    int    count;
} SymTab;

void symtab_init(SymTab *st);
int  symtab_add(SymTab *st, const char *name);   /* 返回寄存器号 */
int  symtab_lookup(SymTab *st, const char *name); /* 未找到返回 -1 */

#endif
