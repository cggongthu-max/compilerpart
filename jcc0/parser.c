#include "parser.h"
#include "lexer.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* ---- 内部状态 ---- */
static Token cur;

static void advance(void) {
    cur = next_token();
}

static void expect(TokenType type) {
    if (cur.type != type) {
        fprintf(stderr, "Parser error: expected token %d, got %d ('%s')\n",
                type, cur.type, cur.text);
        exit(1);
    }
    advance();
}

/* ---- 前向声明 ---- */
static ASTNode *parse_declaration(void);
static ASTNode *parse_statement(void);
static ASTNode *parse_assignment(void);
static ASTNode *parse_while(void);
static ASTNode *parse_block(void);
static ASTNode *parse_expression(void);
static ASTNode *parse_comparison(void);
static ASTNode *parse_additive(void);
static ASTNode *parse_multiplicative(void);
static ASTNode *parse_primary(void);

/* ---- Program → Declaration* Statement* ---- */
ASTNode *parse_program(void) {
    ASTNode *prog = ast_new(ND_PROGRAM);
    advance(); /* 读入第一个 token */

    /* 解析所有声明 */
    while (cur.type == TK_INT) {
        ast_add_child(prog, parse_declaration());
    }

    /* 解析所有语句 */
    while (cur.type != TK_EOF) {
        ast_add_child(prog, parse_statement());
    }

    return prog;
}

/* ---- Declaration → 'int' IDENT ';' ---- */
static ASTNode *parse_declaration(void) {
    expect(TK_INT);
    ASTNode *n = ast_new(ND_DECL);
    strncpy(n->name, cur.text, 63);
    expect(TK_IDENT);
    expect(TK_SEMI);
    return n;
}

/* ---- Statement → Assignment | While | Block ---- */
static ASTNode *parse_statement(void) {
    if (cur.type == TK_WHILE)  return parse_while();
    if (cur.type == TK_LBRACE) return parse_block();
    return parse_assignment();
}

/* ---- Assignment → IDENT '=' Expression ';' ---- */
static ASTNode *parse_assignment(void) {
    ASTNode *n = ast_new(ND_ASSIGN);
    strncpy(n->name, cur.text, 63);
    expect(TK_IDENT);
    expect(TK_ASSIGN);
    n->rhs = parse_expression();
    expect(TK_SEMI);
    return n;
}

/* ---- While → 'while' '(' Expression ')' Statement ---- */
static ASTNode *parse_while(void) {
    ASTNode *n = ast_new(ND_WHILE);
    expect(TK_WHILE);
    expect(TK_LPAREN);
    n->cond = parse_expression();
    expect(TK_RPAREN);
    n->body = parse_statement();
    return n;
}

/* ---- Block → '{' Statement* '}' ---- */
static ASTNode *parse_block(void) {
    ASTNode *n = ast_new(ND_BLOCK);
    expect(TK_LBRACE);
    while (cur.type != TK_RBRACE && cur.type != TK_EOF) {
        ast_add_child(n, parse_statement());
    }
    expect(TK_RBRACE);
    return n;
}

/* ---- Expression → Comparison ---- */
static ASTNode *parse_expression(void) {
    return parse_comparison();
}

/* ---- Comparison → Additive (CompOp Additive)? ---- */
static ASTNode *parse_comparison(void) {
    ASTNode *left = parse_additive();

    BinOpType op;
    switch (cur.type) {
        case TK_LT: op = OP_LT; break;
        case TK_GT: op = OP_GT; break;
        case TK_LE: op = OP_LE; break;
        case TK_GE: op = OP_GE; break;
        case TK_EQ: op = OP_EQ; break;
        case TK_NE: op = OP_NE; break;
        default:    return left;
    }

    advance();
    ASTNode *n = ast_new(ND_BINOP);
    n->op    = op;
    n->left  = left;
    n->right = parse_additive();
    return n;
}

/* ---- Additive → Multiplicative (('+'|'-') Multiplicative)* ---- */
static ASTNode *parse_additive(void) {
    ASTNode *left = parse_multiplicative();

    while (cur.type == TK_PLUS || cur.type == TK_MINUS) {
        BinOpType op = (cur.type == TK_PLUS) ? OP_ADD : OP_SUB;
        advance();
        ASTNode *n = ast_new(ND_BINOP);
        n->op    = op;
        n->left  = left;
        n->right = parse_multiplicative();
        left = n;
    }
    return left;
}

/* ---- Multiplicative → Primary (('*'|'/') Primary)* ---- */
static ASTNode *parse_multiplicative(void) {
    ASTNode *left = parse_primary();

    while (cur.type == TK_MUL || cur.type == TK_DIV) {
        BinOpType op = (cur.type == TK_MUL) ? OP_MUL : OP_DIV;
        advance();
        ASTNode *n = ast_new(ND_BINOP);
        n->op    = op;
        n->left  = left;
        n->right = parse_primary();
        left = n;
    }
    return left;
}

/* ---- Primary → IDENT | NUM | '(' Expression ')' ---- */
static ASTNode *parse_primary(void) {
    if (cur.type == TK_IDENT) {
        ASTNode *n = ast_new(ND_IDENT);
        strncpy(n->name, cur.text, 63);
        advance();
        return n;
    }
    if (cur.type == TK_NUM) {
        ASTNode *n = ast_new(ND_NUM);
        n->value = atoi(cur.text);
        advance();
        return n;
    }
    if (cur.type == TK_LPAREN) {
        advance();
        ASTNode *n = parse_expression();
        expect(TK_RPAREN);
        return n;
    }

    fprintf(stderr, "Parser error: unexpected token '%s'\n", cur.text);
    exit(1);
}
