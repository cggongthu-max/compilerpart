#ifndef AST_H
#define AST_H

/* ---- 节点类型 ---- */
typedef enum {
    ND_PROGRAM,   /* 顶层：声明列表 + 语句列表 */
    ND_DECL,      /* int x;                      */
    ND_ASSIGN,    /* x = expr;                   */
    ND_WHILE,     /* while(cond) body            */
    ND_BLOCK,     /* { stmt* }                   */
    ND_BINOP,     /* 二元运算                    */
    ND_IDENT,     /* 变量引用                    */
    ND_NUM        /* 整数字面量                  */
} NodeType;

/* ---- 二元运算符 ---- */
typedef enum {
    OP_ADD, OP_SUB, OP_MUL, OP_DIV,
    OP_LT,  OP_GT,  OP_LE,  OP_GE, OP_EQ, OP_NE
} BinOpType;

/* ---- AST 节点（统一结构体，各类型使用不同字段） ---- */
typedef struct ASTNode {
    NodeType type;

    /* ND_PROGRAM / ND_BLOCK */
    struct ASTNode **children;
    int              child_count;
    int              child_cap;

    /* ND_DECL / ND_ASSIGN / ND_IDENT */
    char name[64];

    /* ND_ASSIGN */
    struct ASTNode *rhs;

    /* ND_WHILE */
    struct ASTNode *cond;
    struct ASTNode *body;

    /* ND_BINOP */
    BinOpType       op;
    struct ASTNode *left;
    struct ASTNode *right;

    /* ND_NUM */
    int value;
} ASTNode;

/* ---- 构造辅助 ---- */
ASTNode *ast_new(NodeType type);
void     ast_add_child(ASTNode *parent, ASTNode *child);
void     ast_free(ASTNode *node);

#endif
