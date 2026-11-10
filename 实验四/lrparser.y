%{
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <limits.h>
#include "ast.h"
#include "lrparser.tab.h"

int yylex(void);
void yyerror(const char *s);

past ast_root = NULL;
static past normalize_array_initializer(past dims, past init);

/* 复制字符串，避免依赖非标准的 strdup */
static char *copy_text(const char *s)
{
    size_t len;
    char *p;

    if (s == NULL)
        return NULL;

    len = strlen(s);
    p = (char *)malloc(len + 1);

    if (p == NULL) {
        fprintf(stderr, "Memory allocation failed\n");
        exit(EXIT_FAILURE);
    }

    memcpy(p, s, len + 1);
    return p;
}

/* 构造二元运算节点 */
static past make_binary(int op, past left, past right)
{
    past node = newAstNode(BINARY_OPERATOR);
    node->ivalue = op;
    node->left = left;
    node->right = right;
    return node;
}

/* 构造一元运算节点 */
static past make_unary(int op, past child)
{
    past node = newAstNode(UNARY_OPERATOR);
    node->ivalue = op;
    node->left = child;
    return node;
}

/* 构造变量声明节点 */
static past make_var_decl(char *name, past dims, past init)
{
    past var = newAstNode(VAR_DECL);
    var->svalue = copy_text(name);
    /* Array dimensions belong to the declared type, not to the AST children
       shown for a VarDecl. Keep the initializer as its first AST child. */
    var->left = normalize_array_initializer(dims, init);
    return var;
}

/* 构造声明语句节点 */
static past make_decl_stmt(past declarations, int is_const)
{
    past node = newAstNode(DECL_STMT);
    node->ivalue = is_const;
    node->left = declarations;
    return node;
}

/* 构造函数节点 */
static past make_func(char *name, past params, past body)
{
    past node = newAstNode(FUNCTION_DECL);
    node->svalue = copy_text(name);
    node->left = params;
    node->right = body;
    return node;
}

/* 构造函数参数节点 */
static past make_param(char *name, past dims)
{
    past node = newAstNode(PARM_DECL);
    past bounds = NULL;
    node->svalue = copy_text(name);
    /* Match the accepted lab output for a parameter written as a[][N].
       Only bounds following the empty first dimension are passed here. */
    while (dims != NULL) {
        past next = dims->next;
        dims->next = NULL;
        dims->left = bounds;
        bounds = dims;
        dims = next;
    }
    node->left = bounds;
    return node;
}

/* 构造数组下标节点 */
static past make_array_subscript(past base, past index)
{
    past node = newAstNode(ARRAY_SUBSCRIPT_EXPR);
    node->left = base;
    node->right = index;
    return node;
}

/* 构造初始化列表 */
static past make_init_list(past values)
{
    past node = newAstNode(INIT_LIST_EXPR);
    node->left = values;
    return node;
}

static int eval_const_int(past node, int *value)
{
    long long result;
    int left;
    int right;

    if (node == NULL)
        return 0;

    if (node->nodeType == INTEGER_LITERAL) {
        *value = node->ivalue;
        return 1;
    }
    if (node->nodeType == PAREN_EXPR)
        return eval_const_int(node->left, value);
    if (node->nodeType == UNARY_OPERATOR) {
        if (!eval_const_int(node->left, &left))
            return 0;
        if (node->ivalue == Y_ADD)
            result = left;
        else if (node->ivalue == Y_SUB)
            result = -(long long)left;
        else if (node->ivalue == Y_NOT)
            result = !left;
        else
            return 0;
    } else if (node->nodeType == BINARY_OPERATOR) {
        if (!eval_const_int(node->left, &left) ||
            !eval_const_int(node->right, &right))
            return 0;
        switch (node->ivalue) {
        case Y_ADD: result = (long long)left + right; break;
        case Y_SUB: result = (long long)left - right; break;
        case Y_MUL: result = (long long)left * right; break;
        case Y_DIV:
            if (right == 0) return 0;
            result = (long long)left / right;
            break;
        case Y_MODULO:
            if (right == 0) return 0;
            result = (long long)left % right;
            break;
        default:
            return 0;
        }
    } else {
        return 0;
    }

    if (result < INT_MIN || result > INT_MAX)
        return 0;
    *value = (int)result;
    return 1;
}

/* Match the accepted lab implementation's VarDef initializer layout:
   preserve explicit lists and group complete scalar runs by the last bound.
   This rule also applies to one-dimensional non-const arrays. ConstDef keeps
   its explicitly written initializer structure and does not call this rule. */
static past normalize_array_initializer(past dims, past init)
{
    int width;
    past values;
    past result = NULL;

    if (dims == NULL || init == NULL || init->nodeType != INIT_LIST_EXPR)
        return init;

    while (dims->next != NULL)
        dims = dims->next;
    if (!eval_const_int(dims->right, &width) || width <= 0)
        return init;

    values = init->left;
    while (values != NULL) {
        past first = values;
        past last = NULL;
        int count = 0;

        if (values->nodeType == INIT_LIST_EXPR) {
            values = values->next;
            first->next = NULL;
            result = appendAst(result, first);
            continue;
        }

        while (values != NULL && values->nodeType != INIT_LIST_EXPR &&
               count < width) {
            last = values;
            values = values->next;
            ++count;
        }
        last->next = NULL;
        if (count == width)
            result = appendAst(result, make_init_list(first));
        else
            /* Preserve incomplete runs instead of losing source expressions. */
            result = appendAst(result, first);
    }
    init->left = result;
    return init;
}

/* 构造复合语句 */
static past make_block(past items)
{
    past node = newAstNode(COMPOUND_STMT);
    node->left = items;
    return node;
}

/* 构造函数调用 */
static past make_call(char *name, past args)
{
    past id = newID(name);
    past node = newAstNode(CALL_EXPR);
    node->left = id;
    node->right = args;
    return node;
}
%}

%code requires {
#include "ast.h"
}

%union {
    int ivalue;
    float fvalue;
    char *svalue;
    past node;
}

/* 类型与关键字 */
%token Y_INT Y_FLOAT Y_VOID Y_CONST
%token Y_IF Y_ELSE Y_WHILE Y_BREAK Y_CONTINUE Y_RETURN

/* 标识符与数值 */
%token <svalue> Y_ID
%token <ivalue> num_INT
%token <fvalue> num_FLOAT

/* 运算符 */
%token <ivalue> Y_ADD Y_SUB Y_MUL Y_DIV Y_MODULO
%token <ivalue> Y_LESS Y_LESSEQ Y_GREAT Y_GREATEQ
%token <ivalue> Y_NOTEQ Y_EQ Y_NOT Y_AND Y_OR Y_ASSIGN

/* 括号、逗号与分号 */
%token Y_LPAR Y_RPAR
%token Y_LBRACKET Y_RBRACKET
%token Y_LSQUARE Y_RSQUARE
%token Y_COMMA Y_SEMICOLON

/* 非终结符的语义值 */
%type <node> CompUnits CompUnit
%type <node> Decl ConstDecl ConstDefs ConstDef
%type <node> ConstExps ConstInitVal ConstInitVals
%type <node> VarDecl VarDecls VarDef InitVal InitVals
%type <node> FuncDef FuncParams FuncParam
%type <node> Block BlockItems BlockItem Stmt
%type <node> Exp Cond LVal ArraySubscripts
%type <node> PrimaryExp UnaryExp FuncCall CallParams
%type <node> MulExp AddExp RelExp EqExp LAndExp LOrExp
%type <node> Type

%nonassoc LOWER_THAN_ELSE
%nonassoc Y_ELSE

%start CompUnits

%%

CompUnits
    : CompUnit
      {
          $$ = $1;
          ast_root = $$;
      }
    | CompUnits CompUnit
      {
          $$ = appendAst($1, $2);
          ast_root = $$;
      }
    ;

CompUnit
    : Decl
      {
          /* At translation-unit scope Clang exposes each VarDecl directly;
             DECL_STMT is used only for declarations inside a compound stmt. */
          $$ = $1 != NULL ? $1->left : NULL;
      }
    | FuncDef
      {
          $$ = $1;
      }
    ;

Decl
    : ConstDecl
      {
          $$ = $1;
      }
    | VarDecl
      {
          $$ = $1;
      }
    ;

/* 常量声明 */

ConstDecl
    : Y_CONST Type ConstDefs Y_SEMICOLON
      {
          $$ = make_decl_stmt($3, 1);
      }
    ;

ConstDefs
    : ConstDef
      {
          $$ = $1;
      }
    | ConstDefs Y_COMMA ConstDef
      {
          $$ = appendAst($1, $3);
      }
    ;

ConstDef
    : Y_ID Y_ASSIGN ConstInitVal
      {
          $$ = make_var_decl($1, NULL, $3);
      }
    | Y_ID ConstExps Y_ASSIGN ConstInitVal
      {
          /* The accepted ConstDef action keeps the written brace structure;
             only VarDef performs scalar-run grouping. Bounds are type data. */
          yyerror("array declarations are outside the scalar int subset"); YYABORT;
      }
    ;

ConstExps
    : Y_LSQUARE Exp Y_RSQUARE
      {
          $$ = newAstNode(ARRAY_SUBSCRIPT_EXPR);
          $$->right = $2;
      }
    | ConstExps Y_LSQUARE Exp Y_RSQUARE
      {
          past dim = newAstNode(ARRAY_SUBSCRIPT_EXPR);
          dim->right = $3;
          $$ = appendAst($1, dim);
      }
    ;

ConstInitVal
    : Exp
      {
          $$ = $1;
      }
    | Y_LBRACKET Y_RBRACKET
      {
          $$ = make_init_list(NULL);
      }
    | Y_LBRACKET ConstInitVals Y_RBRACKET
      {
          $$ = make_init_list($2);
      }
    ;

ConstInitVals
    : ConstInitVal
      {
          $$ = $1;
      }
    | ConstInitVals Y_COMMA ConstInitVal
      {
          $$ = appendAst($1, $3);
      }
    ;

/* 变量声明 */

VarDecl
    : Type VarDecls Y_SEMICOLON
      {
          $$ = make_decl_stmt($2, 0);
      }
    ;

VarDecls
    : VarDef
      {
          $$ = $1;
      }
    | VarDecls Y_COMMA VarDef
      {
          $$ = appendAst($1, $3);
      }
    ;

VarDef
    : Y_ID
      {
          $$ = make_var_decl($1, NULL, NULL);
      }
    | Y_ID Y_ASSIGN InitVal
      {
          $$ = make_var_decl($1, NULL, $3);
      }
    | Y_ID ConstExps
      {
          yyerror("array declarations are outside the scalar int subset"); YYABORT;
      }
    | Y_ID ConstExps Y_ASSIGN InitVal
      {
          yyerror("array declarations are outside the scalar int subset"); YYABORT;
      }
    ;

InitVal
    : Exp
      {
          $$ = $1;
      }
    | Y_LBRACKET Y_RBRACKET
      {
          $$ = make_init_list(NULL);
      }
    | Y_LBRACKET InitVals Y_RBRACKET
      {
          $$ = make_init_list($2);
      }
    ;

InitVals
    : InitVal
      {
          $$ = $1;
      }
    | InitVals Y_COMMA InitVal
      {
          $$ = appendAst($1, $3);
      }
    ;

/* 函数定义 */

FuncDef
    : Type Y_ID Y_LPAR Y_RPAR Block
      {
          $$ = make_func($2, NULL, $5);
      }
    | Type Y_ID Y_LPAR FuncParams Y_RPAR Block
      {
          $$ = make_func($2, $4, $6);
      }
    ;

FuncParams
    : FuncParam
      {
          $$ = $1;
      }
    | FuncParams Y_COMMA FuncParam
      {
          $$ = appendAst($1, $3);
      }
    ;

FuncParam
    : Type Y_ID
      {
          $$ = make_param($2, NULL);
      }
    | Type Y_ID Y_LSQUARE Y_RSQUARE
      {
          $$ = make_param($2, NULL);
      }
    | Type Y_ID ArraySubscripts
      {
          $$ = make_param($2, NULL);
      }
    | Type Y_ID Y_LSQUARE Y_RSQUARE ArraySubscripts
      {
          $$ = make_param($2, $5);
      }
    ;

/* 复合语句 */

Block
    : Y_LBRACKET Y_RBRACKET
      {
          $$ = make_block(NULL);
      }
    | Y_LBRACKET BlockItems Y_RBRACKET
      {
          $$ = make_block($2);
      }
    ;

BlockItems
    : BlockItem
      {
          $$ = $1;
      }
    | BlockItems BlockItem
      {
          $$ = appendAst($1, $2);
      }
    ;

BlockItem
    : Decl
      {
          /* The accepted lab implementation emits local const declarations
             directly, while ordinary local declarations retain DECL_STMT. */
          $$ = $1 != NULL && $1->ivalue == 1 ? $1->left : $1;
      }
    | Stmt
      {
          $$ = $1;
      }
    ;

/* 语句 */

Stmt
    : LVal Y_ASSIGN Exp Y_SEMICOLON
      {
          $$ = make_binary(Y_ASSIGN, $1, $3);
      }
    | Y_SEMICOLON
      {
          /* Empty statements are omitted by the accepted lab implementation.
             appendAst safely skips NULL even at the start of a block. */
          $$ = NULL;
      }
    | Exp Y_SEMICOLON
      {
          $$ = $1;
      }
    | Block
      {
          $$ = $1;
      }
    | Y_WHILE Y_LPAR Cond Y_RPAR Stmt
      {
          $$ = newAstNode(WHILE_STMT);
          $$->left = $3;
          $$->right = $5;
      }
    | Y_IF Y_LPAR Cond Y_RPAR Stmt %prec LOWER_THAN_ELSE
      {
          $$ = newAstNode(IF_STMT);
          $$->if_cond = $3;
          $$->left = $5;
      }
    | Y_IF Y_LPAR Cond Y_RPAR Stmt Y_ELSE Stmt
      {
          $$ = newAstNode(IF_STMT);
          $$->if_cond = $3;
          $$->left = $5;
          $$->right = $7;
      }
    | Y_BREAK Y_SEMICOLON
      {
          $$ = newAstNode(BREAK_STMT);
      }
    | Y_CONTINUE Y_SEMICOLON
      {
          $$ = newAstNode(CONTINUE_STMT);
      }
    | Y_RETURN Y_SEMICOLON
      {
          $$ = newAstNode(RETURN_STMT);
      }
    | Y_RETURN Exp Y_SEMICOLON
      {
          $$ = newAstNode(RETURN_STMT);
          $$->left = $2;
      }
    ;

/* 表达式 */

Exp
    : AddExp
      {
          $$ = $1;
      }
    ;

Cond
    : LOrExp
      {
          $$ = $1;
      }
    ;

LVal
    : Y_ID
      {
          $$ = newID($1);
      }
    | Y_ID ArraySubscripts
      {
          past base = newID($1);
          past subs = $2;
          past cur = subs;

          while (cur != NULL) {
              past next = cur->next;
              cur->left = base;
              cur->next = NULL;
              base = cur;
              cur = next;
          }

          $$ = base;
      }
    ;

ArraySubscripts
    : Y_LSQUARE Exp Y_RSQUARE
      {
          $$ = newAstNode(ARRAY_SUBSCRIPT_EXPR);
          $$->right = $2;
      }
    | ArraySubscripts Y_LSQUARE Exp Y_RSQUARE
      {
          past dim = newAstNode(ARRAY_SUBSCRIPT_EXPR);
          dim->right = $3;
          $$ = appendAst($1, dim);
      }
    ;

PrimaryExp
    : Y_LPAR Exp Y_RPAR
      {
          $$ = newAstNode(PAREN_EXPR);
          $$->left = $2;
      }
    | LVal
      {
          $$ = $1;
      }
    | num_INT
      {
          $$ = newInt($1);
      }
    | num_FLOAT
      {
          $$ = newFloat($1);
      }
    ;

UnaryExp
    : PrimaryExp
      {
          $$ = $1;
      }
    | FuncCall
      {
          $$ = $1;
      }
    | Y_ADD UnaryExp
      {
          $$ = make_unary(Y_ADD, $2);
      }
    | Y_SUB UnaryExp
      {
          $$ = make_unary(Y_SUB, $2);
      }
    | Y_NOT UnaryExp
      {
          $$ = make_unary(Y_NOT, $2);
      }
    ;

FuncCall
    : Y_ID Y_LPAR Y_RPAR
      {
          $$ = make_call($1, NULL);
      }
    | Y_ID Y_LPAR CallParams Y_RPAR
      {
          $$ = make_call($1, $3);
      }
    ;

CallParams
    : Exp
      {
          $$ = $1;
      }
    | CallParams Y_COMMA Exp
      {
          $$ = appendAst($1, $3);
      }
    ;

MulExp
    : UnaryExp
      {
          $$ = $1;
      }
    | MulExp Y_MUL UnaryExp
      {
          $$ = make_binary(Y_MUL, $1, $3);
      }
    | MulExp Y_DIV UnaryExp
      {
          $$ = make_binary(Y_DIV, $1, $3);
      }
    | MulExp Y_MODULO UnaryExp
      {
          $$ = make_binary(Y_MODULO, $1, $3);
      }
    ;

AddExp
    : MulExp
      {
          $$ = $1;
      }
    | AddExp Y_ADD MulExp
      {
          $$ = make_binary(Y_ADD, $1, $3);
      }
    | AddExp Y_SUB MulExp
      {
          $$ = make_binary(Y_SUB, $1, $3);
      }
    ;

RelExp
    : AddExp
      {
          $$ = $1;
      }
    | RelExp Y_LESS AddExp
      {
          $$ = make_binary(Y_LESS, $1, $3);
      }
    | RelExp Y_LESSEQ AddExp
      {
          $$ = make_binary(Y_LESSEQ, $1, $3);
      }
    | RelExp Y_GREAT AddExp
      {
          $$ = make_binary(Y_GREAT, $1, $3);
      }
    | RelExp Y_GREATEQ AddExp
      {
          $$ = make_binary(Y_GREATEQ, $1, $3);
      }
    ;

EqExp
    : RelExp
      {
          $$ = $1;
      }
    | EqExp Y_EQ RelExp
      {
          $$ = make_binary(Y_EQ, $1, $3);
      }
    | EqExp Y_NOTEQ RelExp
      {
          $$ = make_binary(Y_NOTEQ, $1, $3);
      }
    ;

LAndExp
    : EqExp
      {
          $$ = $1;
      }
    | EqExp Y_AND LAndExp
      {
          $$ = make_binary(Y_AND, $1, $3);
      }
    ;

LOrExp
    : LAndExp
      {
          $$ = $1;
      }
    | LAndExp Y_OR LOrExp
      {
          $$ = make_binary(Y_OR, $1, $3);
      }
    ;

Type
    : Y_INT
      {
          $$ = NULL;
      }
    ;

%%

void yyerror(const char *s)
{
    fprintf(stderr, "Syntax error: %s\n", s);
}


