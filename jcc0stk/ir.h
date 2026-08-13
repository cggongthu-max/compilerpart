#ifndef IR_H
#define IR_H

typedef enum {
    IR_NUM, IR_IDENT,
    IR_ADD, IR_SUB, IR_MUL, IR_DIV,
    IR_GT, IR_LT, IR_GE, IR_LE, IR_EQ, IR_NE,
    IR_ASSIGN, IR_DECL,
    IR_LABEL, IR_JFALSE, IR_JUMP
} IROp;

typedef struct {
    IROp op;
    char text[64];
} IRToken;

#endif
