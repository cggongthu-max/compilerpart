// This file contains a lex module along with a partial function
// to manage var-decl table from var-decl statements (mainly checks
// for boundary conditions and does not yet do real work). A
// trivial test test_var_decl_simple_00 pokes around and checks the
// code does not obviously crash.
// GOTCHAS:
// Lexer uses global variable as input. It took me some digging to
// find out that I should set input..

#include <stdio.h>
#include <ctype.h>
#include <string.h>
#include <stdlib.h>

// Token类型定义
typedef enum {
    TK_INT,      // int关键字
    TK_WHILE,    // while关键字
    TK_IDENT,    // 标识符
    TK_NUM,      // 整数数字
    TK_ASSIGN,   // =
    TK_LE,       // <=
    TK_GE,       // >=
    TK_EQ,       // ==
    TK_NE,       // !=
    TK_LT,       // <
    TK_GT,       // >
    TK_PLUS,     // +
    TK_MINUS,    // -
    TK_MUL,      // *
    TK_DIV,      // /
    TK_SEMI,     // ;
    TK_LBRACE,   // {
    TK_RBRACE,   // }
    TK_LPAREN,   // (
    TK_RPAREN,   // )
    TK_EOF       // 结束符
} TokenType;

// Token结构体，保存类型和文本内容
typedef struct {
    TokenType type;
    char text[64];  // 足够保存标识符/数字
} Token;

// 全局输入指针，指向当前处理的字符
static const char* input;

// 获取下一个Token（隐式状态机实现）
Token next_token() {
    Token token = {TK_EOF, ""};
    int idx = 0;

    // 跳过空白字符（空格、换行、制表符）
    while (isspace(*input)) input++;

    // 处理标识符/关键字
    if (isalpha(*input) || *input == '_') {
        // 隐式进入"标识符状态"，收集所有字母/数字/下划线
        while (isalnum(*input) || *input == '_') {
            if (idx < 63) token.text[idx++] = *input;
            input++;
        }
        token.text[idx] = '\0';

        // 判断是否是关键字
        if (strcmp(token.text, "int") == 0) token.type = TK_INT;
        else if (strcmp(token.text, "while") == 0) token.type = TK_WHILE;
        else token.type = TK_IDENT;
        return token;
    }

    // 处理数字
    if (isdigit(*input)) {
        // 隐式进入"数字状态"，收集所有数字
        while (isdigit(*input)) {
            if (idx < 63) token.text[idx++] = *input;
            input++;
        }
        token.text[idx] = '\0';
        token.type = TK_NUM;
        return token;
    }

    // 处理多字符运算符（<=、>=、==、!=）
    if (*input == '<') {
        input++;
        if (*input == '=') {
            token.type = TK_LE;
            strcpy(token.text, "<=");
            input++;
        } else {
            token.type = TK_LT;
            strcpy(token.text, "<");
        }
        return token;
    }
    if (*input == '>') {
        input++;
        if (*input == '=') {
            token.type = TK_GE;
            strcpy(token.text, ">=");
            input++;
        } else {
            token.type = TK_GT;
            strcpy(token.text, ">");
        }
        return token;
    }
    if (*input == '=') {
        input++;
        if (*input == '=') {
            token.type = TK_EQ;
            strcpy(token.text, "==");
            input++;
        } else {
            token.type = TK_ASSIGN;
            strcpy(token.text, "=");
        }
        return token;
    }
    if (*input == '!') {
        input++;
        if (*input == '=') {
            token.type = TK_NE;
            strcpy(token.text, "!=");
            input++;
        }
        // 这里可以加错误处理，处理单独的!
        return token;
    }

    // 处理单字符运算符和标点
    switch (*input) {
        case '+': token.type = TK_PLUS; strcpy(token.text, "+"); break;
        case '-': token.type = TK_MINUS; strcpy(token.text, "-"); break;
	  //case '': token.type = TK_MUL; strcpy(token.text, ""); break;
        case '/': token.type = TK_DIV; strcpy(token.text, "/"); break;
        case ';': token.type = TK_SEMI; strcpy(token.text, ";"); break;
        case '{': token.type = TK_LBRACE; strcpy(token.text, "{"); break;
        case '}': token.type = TK_RBRACE; strcpy(token.text, "}"); break;
        case '(': token.type = TK_LPAREN; strcpy(token.text, "("); break;
        case ')': token.type = TK_RPAREN; strcpy(token.text, ")"); break;
        case '\0': token.type = TK_EOF; break;
        default: 
            // 错误处理：未知字符
            token.type = TK_EOF;
            fprintf(stderr, "Unknown character: %c\n", *input);
            break;
    }
    input++;
    return token;
}

// small and long vars limited to 64+32
char var_name_8[8*64]; int var_name_8_n;
char var_name_64[64*32]; int var_name_64_n;

void pr_var_decl(Token *v)
{
  if (var_name_8_n >= 64 || var_name_64_n >= 32) {
    printf("variable number overflow\n"); exit(1);
  }
  int l = strlen(v->text); char *var_name; int chunk_len; int var_name_n;
  if (l <= 8) {
    chunk_len = 8; var_name = var_name_8; var_name_n = var_name_8_n;
  } else {
    chunk_len = 64; var_name = var_name_64; var_name_n = var_name_64_n;
  }
  int i = 0;
  for (i = 0; i < var_name_n; i++) {
    if (!memcmp(var_name+i*chunk_len, v->text, l)) { // identical
	printf("redeclaration of variable %s\n", v->text);
	exit(0);
    }
  }
  // ... unfinished
}

void test_var_decl_simple_00()
{
  const char test_code[] = "int s";
  input = test_code;

  Token toks[2];
  toks[0] = next_token();
  toks[1] = next_token();
  var_name_8_n = 0; var_name_64_n = 0;
  pr_var_decl(&toks[1]);
  if (var_name_8_n == 1 && var_name_8[1] == 's')
    printf("symbol s parse complete");
  else
    printf("symbol s parse fail");
}

// 测试函数
int main() {
    const char test_code[] = "int s";
    input = test_code;

    Token t;
    do {
        t = next_token();
        printf("Token: %d, Text: %s\n", t.type, t.text);
    } while (t.type != TK_EOF);

    test_var_decl_simple_00();

    return 0;
}
