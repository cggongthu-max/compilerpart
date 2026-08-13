/* codegen.c */
#include "ir.h"
#include <stdio.h>
#include <string.h>

/* Register stack for expression temporaries */
static int rstack[128];
static int rsp = 0;
static int reg_pool = 1;   /* next free MMIX register */

static int push_reg(void) { return reg_pool++; }
static int pop_reg(void) { return rstack[--rsp]; }
static void push_val(int r) { rstack[rsp++] = r; }

/* Look up variable register; error if undeclared */
static int var_reg(const char *name) {
    extern int find_var(const char *);   /* from symtab.c */
    int r = find_var(name);
    if (r < 0) {
        fprintf(stderr, "Undefined variable: %s\n", name);
        return 255;
    }
    return r;
}

void generate_mmix(IRToken *ir, int n) {
    printf("\tLOC\t#100\nMain\n");

    for (int i = 0; i < n; i++) {
        IRToken *t = &ir[i];
        int a, b, r, v;

        switch (t->op) {
            case IR_NUM:
                r = push_reg();
                printf("\tSET\t$%d,%s\n", r, t->text);
                push_val(r);
                break;

            case IR_IDENT:
                v = var_reg(t->text);
                r = push_reg();
                printf("\tSET\t$%d,$%d\n", r, v);
                push_val(r);
                break;

            case IR_ADD: b = pop_reg(); a = pop_reg(); r = push_reg();
                printf("\tADD\t$%d,$%d,$%d\n", r, a, b); push_val(r); break;
            case IR_SUB: b = pop_reg(); a = pop_reg(); r = push_reg();
                printf("\tSUB\t$%d,$%d,$%d\n", r, a, b); push_val(r); break;
            case IR_MUL: b = pop_reg(); a = pop_reg(); r = push_reg();
                printf("\tMUL\t$%d,$%d,$%d\n", r, a, b); push_val(r); break;
            case IR_DIV: b = pop_reg(); a = pop_reg(); r = push_reg();
                printf("\tDIV\t$%d,$%d,$%d\n", r, a, b); push_val(r); break;

            /* Comparisons: CMP → normalize to 0/1 using conditional sets */
            case IR_GT:
                b = pop_reg(); a = pop_reg(); r = push_reg();
                printf("\tCMP\t$%d,$%d,$%d\n", r, a, b);
                printf("\tCSP\t$%d,$%d,1\n", r, r);
                printf("\tCSNP\t$%d,$%d,0\n", r, r);
                push_val(r); break;

            case IR_LT:
                b = pop_reg(); a = pop_reg(); r = push_reg();
                printf("\tCMP\t$%d,$%d,$%d\n", r, a, b);
                printf("\tCSN\t$%d,$%d,1\n", r, r);
                printf("\tCSNN\t$%d,$%d,0\n", r, r);
                push_val(r); break;

            case IR_EQ:
                b = pop_reg(); a = pop_reg(); r = push_reg();
                printf("\tCMP\t$%d,$%d,$%d\n", r, a, b);
                printf("\tCSZ\t$%d,$%d,1\n", r, r);
                printf("\tCSNZ\t$%d,$%d,0\n", r, r);
                push_val(r); break;

            case IR_GE:
                b = pop_reg(); a = pop_reg(); r = push_reg();
                printf("\tCMP\t$%d,$%d,$%d\n", r, a, b);
                printf("\tCSNN\t$%d,$%d,1\n", r, r);
                printf("\tCSN\t$%d,$%d,0\n", r, r);
                push_val(r); break;

            case IR_LE:
                b = pop_reg(); a = pop_reg(); r = push_reg();
                printf("\tCMP\t$%d,$%d,$%d\n", r, a, b);
                printf("\tCSNP\t$%d,$%d,1\n", r, r);
                printf("\tCSP\t$%d,$%d,0\n", r, r);
                push_val(r); break;

            case IR_NE:
                b = pop_reg(); a = pop_reg(); r = push_reg();
                printf("\tCMP\t$%d,$%d,$%d\n", r, a, b);
                printf("\tCSNZ\t$%d,$%d,1\n", r, r);
                printf("\tCSZ\t$%d,$%d,0\n", r, r);
                push_val(r); break;

            case IR_DECL: {
                extern int alloc_var(const char *);
                int vr = alloc_var(t->text);
                printf("\tSET\t$%d,0\n", vr);
                break;
            }

            case IR_ASSIGN:
                b = pop_reg();   /* value */
                v = var_reg(t->text);
                printf("\tSET\t$%d,$%d\n", v, b);
                break;

            case IR_LABEL:
                printf("%s\n", t->text);
                break;

            case IR_JFALSE:
                a = pop_reg();
                printf("\tBZ\t$%d,%s\n", a, t->text);
                break;

            case IR_JUMP:
                printf("\tJMP\t%s\n", t->text);
                break;
        }
    }
    printf("\tTRAP\t0,0,0\n");
}
