#ifndef CODEGEN_H
#define CODEGEN_H

#include "ast.h"
#include "symtab.h"

/*
 * 生成 MMIX 汇编代码，写入 stdout。
 * 调用前须已完成 parse 和 symtab 注册。
 */
void codegen(ASTNode *program, SymTab *st);

#endif
