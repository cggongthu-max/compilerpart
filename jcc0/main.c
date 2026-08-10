#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "lexer.h"
#include "ast.h"
#include "parser.h"
#include "symtab.h"
#include "codegen.h"

/* 从文件读入全部源码 */
static char *read_file(const char *path) {
    FILE *fp = fopen(path, "r");
    if (!fp) { perror(path); exit(1); }
    fseek(fp, 0, SEEK_END);
    long len = ftell(fp);
    fseek(fp, 0, SEEK_SET);
    char *buf = malloc(len + 1);
    fread(buf, 1, len, fp);
    buf[len] = '\0';
    fclose(fp);
    return buf;
}

/* 遍历 AST，将所有 ND_DECL 注册进符号表 */
static void register_decls(ASTNode *prog, SymTab *st) {
    for (int i = 0; i < prog->child_count; i++) {
        ASTNode *child = prog->children[i];
        if (child->type == ND_DECL) {
            if (symtab_add(st, child->name) < 0) {
                fprintf(stderr, "Error: too many variables or duplicate '%s'\n",
                        child->name);
                exit(1);
            }
        }
    }
}

int main(int argc, char **argv) {
    const char *source;

    if (argc >= 2) {
        source = read_file(argv[1]);
    } else {
        /* 内置测试用例 */
        source =
            "int s; int i;\n"
            "s = 0; i = 1;\n"
            "while (i <= 100) {\n"
            "    s = s + i;\n"
            "    i = i + 1;\n"
            "}\n";
    }

    /* 1. 词法 + 语法 */
    lexer_init(source);
    ASTNode *program = parse_program();

    /* 2. 语义：注册变量 */
    SymTab symtab;
    symtab_init(&symtab);
    register_decls(program, &symtab);

    /* 3. 代码生成 */
    codegen(program, &symtab);

    /* 4. 清理 */
    ast_free(program);

    return 0;
}
