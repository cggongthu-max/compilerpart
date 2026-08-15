#ifndef GEN_H
#define GEN_H

#include "symtab.h"

/* BinOpType is no longer tied to an AST node; it's just the
 * internal state we carry while emitting comparison code. */
typedef enum {
    OP_ADD, OP_SUB, OP_MUL, OP_DIV,
    OP_LT,  OP_GT,  OP_LE,  OP_GE, OP_EQ, OP_NE
} BinOpType;

void generate(SymTab *st);

#endif
