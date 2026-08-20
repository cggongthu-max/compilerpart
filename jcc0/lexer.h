#ifndef LEXER_H
#define LEXER_H

typedef enum {
    TK_INT, TK_WHILE, TK_RETURN, TK_IDENT, TK_NUM,
    TK_ASSIGN, TK_LE, TK_GE, TK_EQ, TK_NE, TK_LT, TK_GT,
    TK_PLUS, TK_MINUS, TK_MUL, TK_DIV,
    TK_SEMI, TK_LBRACE, TK_RBRACE, TK_LPAREN, TK_RPAREN,
    TK_EOF
} TokenType;

typedef struct {
    TokenType type;
    char text[64];
} Token;

void  lexer_init(const char *source);
Token next_token(void);

#endif
