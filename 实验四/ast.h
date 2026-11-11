#ifndef AST_H
#define AST_H

#include <stdbool.h>
#include "node_type.h"

typedef struct _ast ast;
typedef struct _ast *past;

struct _ast {
    int ivalue;
    int is_const;
    int is_float;
    float fvalue;
    char *svalue;
    node_type nodeType;
    past left;
    past right;
    past if_cond;
    past next;
};

past newAstNode(node_type type);
past newInt(int value);
past newFloat(float value);
past newID(const char *value);
past appendAst(past list, past child);
void showAst(past node, int nest, bool use_blank);
void showToken(int token);

#endif

