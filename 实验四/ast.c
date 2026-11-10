#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "ast.h"
#include "lrparser.tab.h"

past newAstNode(node_type type)
{
    past node = (past)calloc(1, sizeof(ast));

    if (node == NULL) {
        fprintf(stderr, "Memory allocation failed\n");
        exit(EXIT_FAILURE);
    }

    node->nodeType = type;
    return node;
}

past newInt(int value)
{
    past node = newAstNode(INTEGER_LITERAL);
    node->ivalue = value;
    return node;
}

past newFloat(float value)
{
    past node = newAstNode(FLOATING_LITERAL);
    node->fvalue = value;
    return node;
}

past newID(const char *value)
{
    past node = newAstNode(DECL_REF_EXPR);

    if (value != NULL) {
        size_t len = strlen(value);
        node->svalue = (char *)malloc(len + 1);

        if (node->svalue == NULL) {
            fprintf(stderr, "Memory allocation failed\n");
            exit(EXIT_FAILURE);
        }

        memcpy(node->svalue, value, len + 1);
    }

    return node;
}

past appendAst(past list, past child)
{
    if (child == NULL)
        return list;

    if (list == NULL)
        return child;

    past tail = list;

    while (tail->next != NULL)
        tail = tail->next;

    tail->next = child;

    return list;
}

void showToken(int token)
{
    switch (token) {
    case Y_INT:
        printf("INTEGER");
        break;
    case Y_FLOAT:
        printf("FLOAT");
        break;
    case Y_CONST:
        printf("CONST");
        break;
    case Y_ADD:
        printf("'+'");
        break;
    case Y_SUB:
        printf("'-'");
        break;
    case Y_MUL:
        printf("'*'");
        break;
    case Y_DIV:
        printf("'/'");
        break;
    case Y_MODULO:
        printf("'%%'");
        break;
    case Y_LESS:
        printf("'<'");
        break;
    case Y_LESSEQ:
        printf("'<='");
        break;
    case Y_GREAT:
        printf("'>'");
        break;
    case Y_GREATEQ:
        printf("'>='");
        break;
    case Y_NOTEQ:
        printf("'!='");
        break;
    case Y_EQ:
        printf("'=='");
        break;
    case Y_NOT:
        printf("'!'");
        break;
    case Y_AND:
        printf("'&&'");
        break;
    case Y_OR:
        printf("'||'");
        break;
    case Y_ASSIGN:
        printf("'='");
        break;
    default:
        printf("UNKNOWN_TOKEN");
        break;
    }
}

void showAst(past node, int nest, bool use_blank)
{
    if (node == NULL)
        return;

    if (use_blank) {
        for (int i = 0; i < nest; i++)
            printf("    ");
    } else {
        printf("%d", nest);
    }

    switch (node->nodeType) {
    case INTEGER_LITERAL:
        printf("INTEGER_LITERAL %d\n", node->ivalue);
        break;

    case BINARY_OPERATOR:
        printf("BINARY_OPERATOR ");
        showToken(node->ivalue);
        printf("\n");
        break;

    case UNARY_OPERATOR:
        printf("UNARY_OPERATOR ");
        showToken(node->ivalue);
        printf("\n");
        break;

    case FUNCTION_DECL:
        printf("FUNCTION_DECL '%s'\n",
               node->svalue != NULL ? node->svalue : "");
        break;

    case CALL_EXPR:
        printf("CALL_EXPR\n");
        break;

    case COMPOUND_STMT:
        printf("COMPOUND_STMT\n");
        break;

    case DECL_STMT:
        printf("DECL_STMT\n");
        break;

    case VARIABLE_REF:
        printf("VARIABLE_REF\n");
        break;

    case PARM_DECL:
        printf("PARM_DECL '%s'\n",
               node->svalue != NULL ? node->svalue : "");
        break;

    case VAR_DECL:
        printf("VAR_DECL: '%s'\n",
               node->svalue != NULL ? node->svalue : "");
        break;

    case DECL_REF_EXPR:
        printf("DECL_REF_EXPR: '%s'\n",
               node->svalue != NULL ? node->svalue : "");
        break;

    case RETURN_STMT:
        printf("RETURN_STMT\n");
        break;

    case WHILE_STMT:
        printf("WHILE_STMT\n");
        break;

    case BREAK_STMT:
        printf("BREAK_STMT\n");
        break;

    case CONTINUE_STMT:
        printf("CONTINUE_STMT\n");
        break;

    case IF_STMT:
        printf("IF_STMT\n");
        showAst(node->if_cond, nest + 1, use_blank);
        break;

    case INIT_LIST_EXPR:
        printf("INIT_LIST_EXPR\n");
        break;

    case ARRAY_SUBSCRIPT_EXPR:
        printf("ARRAY_SUBSCRIPT_EXPR\n");
        break;

    case PAREN_EXPR:
        printf("PAREN_EXPR\n");
        break;

    default:
        /* Keep the exact fallback spelling used by the supplied showAst. */
        printf("NOT IMPlemented\n");
        break;
    }

    showAst(node->left, nest + 1, use_blank);
    showAst(node->right, nest + 1, use_blank);
    showAst(node->next, nest, use_blank);
}
