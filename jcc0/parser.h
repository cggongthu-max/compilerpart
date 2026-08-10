#ifndef PARSER_H
#define PARSER_H

#include "ast.h"

/* 解析整个程序，返回根节点（ND_PROGRAM） */
ASTNode *parse_program(void);

#endif
