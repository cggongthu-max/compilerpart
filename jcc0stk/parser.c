/* parser.c */
#include "lexer.h"
#include "ir.h"
#include <stdio.h>
#include <string.h>

static Token cur;
static IRToken ir[1024];
static int ir_count = 0;
static int while_counter = 0;

static void emit(IROp op, const char *text) {
    ir[ir_count].op = op;
    if (text) strncpy(ir[ir_count].text, text, 63);
    else ir[ir_count].text[0] = '\0';
    ir_count++;
}

static void advance(void) { cur = next_token(); }

static void expect(TokenType t) {
    if (cur.type != t) {
        fprintf(stderr, "Parse error: expected token %d, got %d (%s)\n",
                t, cur.type, cur.text);
    }
    advance();
}

/* forward decls */
static void parse_stmt(void);
static void parse_expr(void);   /* handles + - */
static void parse_term(void);   /* handles * / */
static void parse_comp(void);   /* handles > < >= <= == != */
static void parse_factor(void); /* handles NUM, IDENT, ( ) */

/* --- expression levels --- */

static void parse_factor(void) {
    if (cur.type == TK_NUM) {
        emit(IR_NUM, cur.text);
        advance();
    } else if (cur.type == TK_IDENT) {
        emit(IR_IDENT, cur.text);
        advance();
    } else if (cur.type == TK_LPAREN) {
        advance();
        parse_comp();   /* parse full expression inside parens */
        expect(TK_RPAREN);
    } else {
        fprintf(stderr, "Unexpected factor: %s\n", cur.text);
        advance();
    }
}

static void parse_term(void) {
    parse_factor();
    while (cur.type == TK_MUL || cur.type == TK_DIV) {
        TokenType op = cur.type;
        advance();
        parse_factor();
        emit(op == TK_MUL ? IR_MUL : IR_DIV, NULL);
    }
}

static void parse_expr(void) {
    parse_term();
    while (cur.type == TK_PLUS || cur.type == TK_MINUS) {
        TokenType op = cur.type;
        advance();
        parse_term();
        emit(op == TK_PLUS ? IR_ADD : IR_SUB, NULL);
    }
}

static void parse_comp(void) {
    parse_expr();
    if (cur.type == TK_GT || cur.type == TK_LT ||
        cur.type == TK_GE || cur.type == TK_LE ||
        cur.type == TK_EQ || cur.type == TK_NE) {
        TokenType op = cur.type;
        advance();
        parse_expr();
        switch (op) {
            case TK_GT: emit(IR_GT, NULL); break;
            case TK_LT: emit(IR_LT, NULL); break;
            case TK_GE: emit(IR_GE, NULL); break;
            case TK_LE: emit(IR_LE, NULL); break;
            case TK_EQ: emit(IR_EQ, NULL); break;
            case TK_NE: emit(IR_NE, NULL); break;
            default: break;
        }
    }
}

/* --- statements --- */

static void parse_decl(void) {
    expect(TK_INT);
    emit(IR_DECL, cur.text);
    expect(TK_IDENT);
    expect(TK_SEMI);
}

static void parse_assign(void) {
    char name[64];
    strcpy(name, cur.text);
    expect(TK_IDENT);
    expect(TK_ASSIGN);
    parse_comp();
    expect(TK_SEMI);
    emit(IR_ASSIGN, name);
}

static void parse_while(void) {
    int id = while_counter++;
    char start[32], end[32];
    sprintf(start, "W%d", id);
    sprintf(end,  "W%d_END", id);

    expect(TK_WHILE);
    expect(TK_LPAREN);

    emit(IR_LABEL, start);   /* L_start: */
    parse_comp();            /* condition → leaves 0/1 on stack */

    expect(TK_RPAREN);

    emit(IR_JFALSE, end);    /* jump to end if false */

    expect(TK_LBRACE);
    while (cur.type != TK_RBRACE && cur.type != TK_EOF) {
        parse_stmt();
    }
    expect(TK_RBRACE);

    emit(IR_JUMP, start);    /* loop back */
    emit(IR_LABEL, end);     /* L_end: */
}

static void parse_stmt(void) {
    if (cur.type == TK_INT)       parse_decl();
    else if (cur.type == TK_WHILE) parse_while();
    else if (cur.type == TK_IDENT) parse_assign();
    else {
        fprintf(stderr, "Unexpected statement: %s\n", cur.text);
        advance();
    }
}

/* Entry point */
IRToken *parse_program(const char *src, int *out_count) {
    lexer_init(src);
    advance();  /* load first token */
    while (cur.type != TK_EOF) {
        parse_stmt();
    }
    *out_count = ir_count;
    return ir;
}
