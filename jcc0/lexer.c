#include "lexer.h"
#include "jcc0_lex.h"          /* + new include */
#include <stdio.h>
#include <ctype.h>
#include <string.h>

static const char *input;

void lexer_init(const char *source) {
    input = source;
}

Token next_token(void) {
    Token tok = {TK_EOF, ""};
    int idx = 0;

    /* before: while (isspace((unsigned char)*input)) input++; */
    struct jcc0_lex_ctl ctl = { input };
    jcc0_lex_skipspaces(&ctl);
    input = ctl.input;

    /* 标识符 / 关键字 */
    if (isalpha((unsigned char)*input) || *input == '_') {
        while (isalnum((unsigned char)*input) || *input == '_') {
            if (idx < 63) tok.text[idx++] = *input;
            input++;
        }
        tok.text[idx] = '\0';
        if (strcmp(tok.text, "int") == 0)       tok.type = TK_INT;
        else if (strcmp(tok.text, "while") == 0) tok.type = TK_WHILE;
	else if (strcmp(tok.text, "return") == 0) tok.type = TK_RETURN;
        else                                     tok.type = TK_IDENT;
        return tok;
    }

    /* 数字 */
    if (isdigit((unsigned char)*input)) {
        while (isdigit((unsigned char)*input)) {
            if (idx < 63) tok.text[idx++] = *input;
            input++;
        }
        tok.text[idx] = '\0';
        tok.type = TK_NUM;
        return tok;
    }

    /* 双字符运算符 */
    if (*input == '<') {
        input++;
        if (*input == '=') { input++; tok.type = TK_LE; strcpy(tok.text, "<="); }
        else               {          tok.type = TK_LT; strcpy(tok.text, "<");  }
        return tok;
    }
    if (*input == '>') {
        input++;
        if (*input == '=') { input++; tok.type = TK_GE; strcpy(tok.text, ">="); }
        else               {          tok.type = TK_GT; strcpy(tok.text, ">");  }
        return tok;
    }
    if (*input == '=') {
        input++;
        if (*input == '=') { input++; tok.type = TK_EQ;     strcpy(tok.text, "=="); }
        else               {          tok.type = TK_ASSIGN; strcpy(tok.text, "=");  }
        return tok;
    }
    if (*input == '!') {
        input++;
        if (*input == '=') { input++; tok.type = TK_NE; strcpy(tok.text, "!="); }
        return tok;
    }

    /* 单字符 */
    switch (*input) {
        case '+': tok.type = TK_PLUS;   strcpy(tok.text, "+"); break;
        case '-': tok.type = TK_MINUS;  strcpy(tok.text, "-"); break;
        case '*': tok.type = TK_MUL;    strcpy(tok.text, "*"); break;
        case '/': tok.type = TK_DIV;    strcpy(tok.text, "/"); break;
        case ';': tok.type = TK_SEMI;   strcpy(tok.text, ";"); break;
        case '{': tok.type = TK_LBRACE; strcpy(tok.text, "{"); break;
        case '}': tok.type = TK_RBRACE; strcpy(tok.text, "}"); break;
        case '(': tok.type = TK_LPAREN; strcpy(tok.text, "("); break;
        case ')': tok.type = TK_RPAREN; strcpy(tok.text, ")"); break;
        case ',': tok.type = TK_COMMA;  strcpy(tok.text, ","); break;
        case '\0': return tok;
        default:
            fprintf(stderr, "Lexer error: unknown character '%c'\n", *input);
            return tok;
    }
    input++;
    return tok;
}
