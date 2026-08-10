#include "ast.h"
#include <stdlib.h>
#include <string.h>

ASTNode *ast_new(NodeType type) {
    ASTNode *n = calloc(1, sizeof(ASTNode));
    n->type = type;
    return n;
}

void ast_add_child(ASTNode *parent, ASTNode *child) {
    if (parent->child_count >= parent->child_cap) {
        parent->child_cap = parent->child_cap ? parent->child_cap * 2 : 8;
        parent->children = realloc(parent->children,
                                   parent->child_cap * sizeof(ASTNode *));
    }
    parent->children[parent->child_count++] = child;
}

void ast_free(ASTNode *node) {
    if (!node) return;
    for (int i = 0; i < node->child_count; i++)
        ast_free(node->children[i]);
    free(node->children);
    ast_free(node->rhs);
    ast_free(node->cond);
    ast_free(node->body);
    ast_free(node->left);
    ast_free(node->right);
    free(node);
}
