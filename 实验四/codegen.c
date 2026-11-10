#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "codegen.h"
#include "lrparser.tab.h"

typedef struct {
    char operand[32];
    int boolean;
} Value;

typedef struct Binding {
    const char *name;
    Value address;
    struct Binding *next;
} Binding;

static Binding *locals;
static int temporary, label, current_block, terminated;
static int break_target, continue_target;
static Value last_value;

static void fail(const char *message)
{
    fprintf(stderr, "Code generation: %s\n", message);
    exit(EXIT_FAILURE);
}

static void emit(char *result, const char *format, ...)
{
    size_t used = strlen(result);
    if (used >= IR_CAPACITY) fail("IR buffer exhausted");
    va_list args;
    va_start(args, format);
    int count = vsnprintf(result + used, IR_CAPACITY - used, format, args);
    va_end(args);
    if (count < 0 || (size_t)count >= IR_CAPACITY - used)
        fail("IR buffer exhausted");
}

static Value fresh(int boolean)
{
    Value value = {{0}, boolean};
    snprintf(value.operand, sizeof value.operand, "%%t%d", ++temporary);
    return value;
}

static Value address(past node)
{
    if (!node || node->nodeType != DECL_REF_EXPR)
        fail("expected a scalar variable");
    for (Binding *binding = locals; binding; binding = binding->next)
        if (strcmp(binding->name, node->svalue) == 0)
            return binding->address;
    fail(node->svalue);
    return (Value){0};
}

static void start_block(int block, char *result)
{
    emit(result, "bb%d:\n", block);
    current_block = block;
    terminated = 0;
}

static void jump(int target, char *result)
{
    emit(result, "  br label %%bb%d\n", target);
    terminated = 1;
}

static void branch(Value condition, int yes, int no, char *result)
{
    emit(result, "  br i1 %s, label %%bb%d, label %%bb%d\n",
         condition.operand, yes, no);
    terminated = 1;
}

static Value as_int(Value value, char *result)
{
    if (!value.boolean) return value;
    Value converted = fresh(0);
    emit(result, "  %s = zext i1 %s to i32\n", converted.operand, value.operand);
    return converted;
}

static Value as_bool(Value value, char *result)
{
    if (value.boolean) return value;
    Value converted = fresh(1);
    emit(result, "  %s = icmp ne i32 %s, 0\n", converted.operand, value.operand);
    return converted;
}

static const char *arithmetic(int op)
{
    switch (op) {
    case Y_ADD: return "add";
    case Y_SUB: return "sub";
    case Y_MUL: return "mul";
    case Y_DIV: return "sdiv";
    case Y_MODULO: return "srem";
    default: return NULL;
    }
}

static const char *predicate(int op)
{
    switch (op) {
    case Y_LESS: return "slt";
    case Y_LESSEQ: return "sle";
    case Y_GREAT: return "sgt";
    case Y_GREATEQ: return "sge";
    case Y_EQ: return "eq";
    case Y_NOTEQ: return "ne";
    default: fail("unsupported comparison"); return NULL;
    }
}

static Value expression(past node, char *result)
{
    if (!node) fail("missing expression");
    switch (node->nodeType) {
    case BINARY_OPERATOR:
        if (arithmetic(node->ivalue)) genArithmeticExpr(node, result);
        else genLogicExpr(node, result);
        return last_value;
    case UNARY_OPERATOR:
        if (node->ivalue == Y_NOT) genLogicExpr(node, result);
        else genArithmeticExpr(node, result);
        return last_value;
    default:
        genArithmeticExpr(node, result);
        return last_value;
    }
}

void genArithmeticExpr(past node, char *result)
{
    Value value = {{0}, 0};
    switch (node->nodeType) {
    case INTEGER_LITERAL:
        snprintf(value.operand, sizeof value.operand, "%d", node->ivalue);
        break;
    case DECL_REF_EXPR: {
        Value ptr = address(node);
        value = fresh(0);
        emit(result, "  %s = load i32, ptr %s, align 4\n", value.operand, ptr.operand);
        break;
    }
    case PAREN_EXPR:
        value = expression(node->left, result);
        break;
    case UNARY_OPERATOR: {
        Value operand = as_int(expression(node->left, result), result);
        value = operand;
        if (node->ivalue == Y_SUB) {
            value = fresh(0);
            emit(result, "  %s = sub i32 0, %s\n", value.operand, operand.operand);
        } else if (node->ivalue != Y_ADD) fail("unsupported unary operator");
        break;
    }
    case BINARY_OPERATOR: {
        const char *instruction = arithmetic(node->ivalue);
        if (!instruction) fail("unsupported arithmetic operator");
        Value left = as_int(expression(node->left, result), result);
        Value right = as_int(expression(node->right, result), result);
        value = fresh(0);
        emit(result, "  %s = %s i32 %s, %s\n",
             value.operand, instruction, left.operand, right.operand);
        break;
    }
    default:
        fail("expected an integer scalar expression");
    }
    last_value = value;
}

void genLogicExpr(past node, char *result)
{
    int op = node->ivalue;
    if (node->nodeType == UNARY_OPERATOR && op == Y_NOT) {
        Value operand = as_bool(expression(node->left, result), result);
        last_value = fresh(1);
        emit(result, "  %s = xor i1 %s, true\n", last_value.operand, operand.operand);
    } else if (node->nodeType != BINARY_OPERATOR || arithmetic(op)) {
        last_value = as_bool(expression(node, result), result);
    } else if (op == Y_AND || op == Y_OR) {
        Value left = as_bool(expression(node->left, result), result);
        int from = current_block;
        int rhs = ++label;
        int merge = ++label;
        if (op == Y_AND) branch(left, rhs, merge, result);
        else branch(left, merge, rhs, result);

        start_block(rhs, result);
        Value right = as_bool(expression(node->right, result), result);
        int rhs_exit = current_block;
        jump(merge, result);

        start_block(merge, result);
        last_value = fresh(1);
        emit(result, "  %s = phi i1 [ %s, %%bb%d ], [ %s, %%bb%d ]\n",
             last_value.operand, op == Y_AND ? "false" : "true",
             from, right.operand, rhs_exit);
    } else {
        const char *comparison = predicate(op);
        Value left = as_int(expression(node->left, result), result);
        Value right = as_int(expression(node->right, result), result);
        last_value = fresh(1);
        emit(result, "  %s = icmp %s i32 %s, %s\n",
             last_value.operand, comparison, left.operand, right.operand);
    }
}

static void store(Value value, Value ptr, char *result)
{
    value = as_int(value, result);
    emit(result, "  store i32 %s, ptr %s, align 4\n", value.operand, ptr.operand);
}

void genAssignStmt(past node, char *result)
{
    Value ptr = address(node->left);
    store(expression(node->right, result), ptr, result);
}

static void statement(past node, char *result);

void genIfStmt(past node, char *result)
{
    int yes = ++label;
    int done = ++label;
    int no = node->right ? ++label : done;
    genLogicExpr(node->if_cond, result);
    branch(last_value, yes, no, result);
    start_block(yes, result);
    statement(node->left, result);
    int yes_terminated = terminated;
    if (!terminated) jump(done, result);
    if (node->right) {
        start_block(no, result);
        statement(node->right, result);
        if (yes_terminated && terminated) return;
        if (!terminated) jump(done, result);
    }
    start_block(done, result);
}

void genWhileStmt(past node, char *result)
{
    int test = ++label;
    int body = ++label;
    int done = ++label;
    jump(test, result);
    start_block(test, result);
    genLogicExpr(node->left, result);
    branch(last_value, body, done, result);
    start_block(body, result);
    int old_break = break_target;
    int old_continue = continue_target;
    break_target = done;
    continue_target = test;
    statement(node->right, result);
    break_target = old_break;
    continue_target = old_continue;
    if (!terminated) jump(test, result);
    start_block(done, result);
}

static void declare(past node, char *result)
{
    Binding *binding = malloc(sizeof *binding);
    if (!binding) fail("out of memory");
    binding->name = node->svalue;
    snprintf(binding->address.operand, sizeof binding->address.operand,
             "%%t%d", node->ivalue);
    binding->address.boolean = 0;
    binding->next = locals;
    locals = binding;

    if (node->left)
        store(expression(node->left, result), binding->address, result);
}

static void statement(past node, char *result)
{
    if (!node || terminated) return;
    switch (node->nodeType) {
    case COMPOUND_STMT: {
        Binding *outer = locals;
        for (past item = node->left; item && !terminated; item = item->next)
            statement(item, result);
        while (locals != outer) {
            Binding *binding = locals;
            locals = binding->next;
            free(binding);
        }
        break;
    }
    case DECL_STMT:
        for (past var = node->left; var; var = var->next)
            declare(var, result);
        break;
    case VAR_DECL: declare(node, result); break;
    case IF_STMT: genIfStmt(node, result); break;
    case WHILE_STMT: genWhileStmt(node, result); break;
    case BREAK_STMT:
        if (!break_target) fail("break outside a loop");
        jump(break_target, result);
        break;
    case CONTINUE_STMT:
        if (!continue_target) fail("continue outside a loop");
        jump(continue_target, result);
        break;
    case RETURN_STMT: {
        Value value = as_int(expression(node->left, result), result);
        emit(result, "  ret i32 %s\n", value.operand);
        terminated = 1;
        break;
    }
    case BINARY_OPERATOR:
        if (node->ivalue == Y_ASSIGN) {
            genAssignStmt(node, result);
            break;
        }
        /* An expression statement can also have a binary operator. */
        /* fall through */
    default: expression(node, result); break;
    }
}

/* Allocate local slots once, even when a declaration is inside a loop. */
static void allocate_locals(past node, char *result)
{
    for (; node; node = node->next) {
        if (node->nodeType == VAR_DECL) {
            node->ivalue = ++temporary;
            emit(result, "  %%t%d = alloca i32, align 4\n", node->ivalue);
        }
        allocate_locals(node->left, result);
        allocate_locals(node->right, result);
        allocate_locals(node->if_cond, result);
    }
}

void genProgram(past node, char *result)
{
    if (!node || node->next || node->nodeType != FUNCTION_DECL ||
        node->left || strcmp(node->svalue, "main") != 0)
        fail("expected exactly one parameterless int main function");
    result[0] = '\0';
    temporary = label = break_target = continue_target = 0;
    emit(result, "define i32 @main() {\n");
    start_block(0, result);
    allocate_locals(node->right, result);
    statement(node->right, result);
    if (!terminated) emit(result, "  ret i32 0\n");
    emit(result, "}\n");
}

