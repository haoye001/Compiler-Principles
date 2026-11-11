#include <limits.h>
#include <stdint.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "codegen.h"
#include "lrparser.tab.h"

typedef struct {
    char operand[32];
    int boolean;
    int is_void;
    int is_float;
    const char *global_name;
    past constant_subscript;
    past dimensions;
    int alignment;
} Value;

typedef struct Binding {
    const char *name;
    Value address;
    int has_constant;
    int constant;
    past dimensions;
    struct Binding *next;
} Binding;

static Binding *locals;
static int temporary, label, current_block, terminated;
static int break_target, continue_target;
static Value last_value, return_slot;
static int return_target, entry_number, returns_void, returns_float;
static past functions;
static const char *current_function;
static char *constant_data;
static int needs_memcpy, needs_memset;

typedef struct ArrayConstant {
    char *name;
    struct ArrayConstant *next;
} ArrayConstant;

static ArrayConstant *array_constants;

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
    Value value = {.boolean = boolean};
    snprintf(value.operand, sizeof value.operand, "%%%d", ++temporary);
    return value;
}

static Binding *find_local(const char *name)
{
    for (Binding *binding = locals; binding; binding = binding->next)
        if (strcmp(binding->name, name) == 0)
            return binding;
    return NULL;
}

static Binding *push_binding(past declaration)
{
    Binding *binding = calloc(1, sizeof *binding);
    if (!binding) fail("out of memory");
    binding->name = declaration->svalue;
    binding->dimensions = declaration->right;
    binding->address.is_float = declaration->is_float;
    binding->next = locals;
    locals = binding;
    return binding;
}

static void pop_bindings(Binding *outer)
{
    while (locals != outer) {
        Binding *binding = locals;
        locals = binding->next;
        free(binding);
    }
}

static past find_global(const char *name)
{
    for (past node = functions; node; node = node->next)
        if (node->nodeType == VAR_DECL && strcmp(node->svalue, name) == 0)
            return node;
    return NULL;
}

static Value variable_address(past node)
{
    if (!node || node->nodeType != DECL_REF_EXPR)
        fail("expected a scalar variable");
    Binding *binding = find_local(node->svalue);
    if (binding) return binding->address;
    if (find_global(node->svalue))
        return (Value){.global_name = node->svalue, .is_float = find_global(node->svalue)->is_float};
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


static const char *value_type(Value value)
{
    return value.is_float ? "float" : "i32";
}

static Value float_value(float number)
{
    Value value = {.is_float = 1};
    snprintf(value.operand, sizeof value.operand, "%.6e", (double)number);
    if (strtod(value.operand, NULL) != (double)number) {
        double wide = number;
        uint64_t bits;
        memcpy(&bits, &wide, sizeof bits);
        snprintf(value.operand, sizeof value.operand, "0x%016llX", (unsigned long long)bits);
    }
    return value;
}

static double literal_float(Value value)
{
    if (value.operand[0] == '0' && value.operand[1] == 'x') {
        uint64_t bits = strtoull(value.operand + 2, NULL, 16);
        double number;
        memcpy(&number, &bits, sizeof number);
        return number;
    }
    return strtod(value.operand, NULL);
}

static Value as_int(Value value, char *result)
{
    if (value.is_void) fail("void function has no value");

    if (value.is_float) {
        Value converted = fresh(0);
        if (value.operand[0] != '%') {
            double number = literal_float(value);
            if (!(number >= INT_MIN && number <= INT_MAX)) fail("float-to-int conversion out of range");
            --temporary;
            snprintf(converted.operand, sizeof converted.operand, "%d", (int)number);
        } else {
            emit(result, "  %s = fptosi float %s to i32\n", converted.operand, value.operand);
        }
        return converted;
    }
    if (!value.boolean) return value;
    if (value.operand[0] != '%') {
        strcpy(value.operand, value.operand[0] == 't' ? "1" : "0");
        value.boolean = 0;
        return value;
    }
    Value converted = fresh(0);
    emit(result, "  %s = zext i1 %s to i32\n", converted.operand, value.operand);
    return converted;
}


static Value as_float(Value value, char *result)
{
    if (value.is_float) return value;
    value = as_int(value, result);
    if (value.operand[0] != '%') return float_value((float)atoi(value.operand));
    Value converted = fresh(0);
    converted.is_float = 1;
    emit(result, "  %s = sitofp i32 %s to float\n", converted.operand, value.operand);
    return converted;
}

static Value convert_value(Value value, int to_float, char *result)
{
    return to_float ? as_float(value, result) : as_int(value, result);
}

static Value as_bool(Value value, char *result)
{
    if (value.is_void) fail("void function has no value");
    if (value.boolean) return value;
    if (value.operand[0] != '%') {
        int truth = value.is_float ? literal_float(value) != 0 : atoi(value.operand) != 0;
        strcpy(value.operand, truth ? "true" : "false");
        value.boolean = 1;
        value.is_float = 0;
        return value;
    }
    Value converted = fresh(1);

    if (value.is_float)
        emit(result, "  %s = fcmp une float %s, 0.000000e+00\n", converted.operand, value.operand);
    else
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

static int constant_int(past node, int *result)
{
    int left, right;
    int64_t value;
    switch (node->nodeType) {
    case INTEGER_LITERAL:
        *result = node->ivalue;
        return 1;
    case PAREN_EXPR:
        return constant_int(node->left, result);
    case UNARY_OPERATOR:
        if (!constant_int(node->left, &left)) return 0;
        switch (node->ivalue) {
        case Y_ADD: value = left; break;
        case Y_SUB: value = -(int64_t)left; break;
        case Y_NOT: value = !left; break;
        default: return 0;
        }
        break;
    case BINARY_OPERATOR:
        if (!constant_int(node->left, &left)) return 0;
        if ((node->ivalue == Y_AND && !left) || (node->ivalue == Y_OR && left)) {
            *result = !!left;
            return 1;
        }
        if (!constant_int(node->right, &right)) return 0;
        switch (node->ivalue) {
        case Y_ADD: value = (int64_t)left + right; break;
        case Y_SUB: value = (int64_t)left - right; break;
        case Y_MUL: value = (int64_t)left * right; break;
        case Y_LESS: value = left < right; break;
        case Y_LESSEQ: value = left <= right; break;
        case Y_GREAT: value = left > right; break;
        case Y_GREATEQ: value = left >= right; break;
        case Y_EQ: value = left == right; break;
        case Y_NOTEQ: value = left != right; break;
        case Y_AND: value = left && right; break;
        case Y_OR: value = left || right; break;
        case Y_DIV:
            if (right == 0 || (left == INT_MIN && right == -1)) return 0;
            value = left / right;
            break;
        case Y_MODULO:
            if (right == 0 || (left == INT_MIN && right == -1)) return 0;
            value = left % right;
            break;
        default: return 0;
        }
        break;
    default:
        return 0;
    }

    if (value < INT_MIN || value > INT_MAX) return 0;
    *result = (int)value;
    return 1;
}

static void resolve_constants(past node);

/* Resolve references in a writable location's indices, keeping its base. */
static void resolve_indices(past node)
{
    if (node->nodeType != ARRAY_SUBSCRIPT_EXPR) return;
    resolve_indices(node->left);
    resolve_constants(node->right);
}

static void resolve_constants(past node)
{
    for (; node; node = node->next) {
        switch (node->nodeType) {
        case FUNCTION_DECL: {
            Binding *outer = locals;
            for (past param = node->left; param; param = param->next) {
                resolve_constants(param->right);
                push_binding(param);
            }
            resolve_constants(node->right);
            pop_bindings(outer);
            break;
        }
        case COMPOUND_STMT: {
            Binding *outer = locals;
            resolve_constants(node->left);
            pop_bindings(outer);
            break;
        }
        case VAR_DECL: {
            resolve_constants(node->right);
            Binding *binding = push_binding(node);
            resolve_constants(node->left);
    binding->has_constant = node->is_const && !node->is_float && !node->right && node->left &&
                                    constant_int(node->left, &binding->constant);
            break;
        }
        case DECL_REF_EXPR: {
            Binding *binding = find_local(node->svalue);
            if (binding && binding->has_constant) {
                node->nodeType = INTEGER_LITERAL;
                node->ivalue = binding->constant;
            }
            break;
        }
        case CALL_EXPR:
            resolve_constants(node->right);
            break;
        case BINARY_OPERATOR:
            if (node->ivalue == Y_ASSIGN) {
                resolve_indices(node->left);
                resolve_constants(node->right);
                break;
            }
            /* fall through */
        default:
            resolve_constants(node->left);
            resolve_constants(node->right);
            resolve_constants(node->if_cond);
            break;
        }
    }
}

static int is_zero_initializer(past node)
{
    for (; node; node = node->next) {
        if (node->nodeType == INIT_LIST_EXPR) {
            if (!is_zero_initializer(node->left)) return 0;
        } else {
            int value;
            if (!constant_int(node, &value) || value) return 0;
        }
    }
    return 1;
}

static Value expression(past node, char *result);

enum { TYPE_CAPACITY = 256 };

static int array_bound(past dimension)
{
    int size;
    if (!constant_int(dimension->right, &size) || size <= 0)
        fail("array bounds must be positive integer constants");
    return size;
}

static void array_type(past dimension, char type[TYPE_CAPACITY])
{
    size_t used = 0, depth = 0;
    for (; dimension; dimension = dimension->next) {
        int written = snprintf(type + used, TYPE_CAPACITY - used,
                               "[%d x ", array_bound(dimension));
        if (written < 0 || (size_t)written >= TYPE_CAPACITY - used)
            fail("array type is too long");
        used += (size_t)written;
        ++depth;
    }
    if (used + depth + 4 > TYPE_CAPACITY) fail("array type is too long");
    memcpy(type + used, "i32", 3);
    used += 3;
    memset(type + used, ']', depth);
    type[used + depth] = '\0';
}

static int array_alignment(past dimensions)
{
    int elements = 1;
    for (; dimensions; dimensions = dimensions->next) {
        int bound = array_bound(dimensions);
        if (bound >= 4 || elements * bound >= 4) return 16;
        elements *= bound;
    }
    return 4;
}

static Value array_element(Value base, past dimensions, Value index, char *result)
{
    char type[TYPE_CAPACITY];
    int is_parameter = !dimensions->right;
    array_type(is_parameter ? dimensions->next : dimensions, type);
    index = as_int(index, result);
    int stride = 4;
    for (past dim = dimensions->next; dim; dim = dim->next)
        stride = (stride * (array_bound(dim) % 16)) % 16;
    int offset = stride;
    if (index.operand[0] != '%')
        offset = ((atoi(index.operand) % 16) * stride) % 16;
    int alignment = base.alignment ? base.alignment : 4;
    while (alignment > 4 && offset % alignment) alignment /= 2;
    if (index.operand[0] == '%') {
        Value extended = fresh(0);
        emit(result, "  %s = sext i32 %s to i64\n", extended.operand, index.operand);
        index = extended;
    }
    Value pointer = fresh(0);
    pointer.alignment = alignment;
    emit(result, "  %s = getelementptr inbounds %s, %s* %s%s, %si64 %s\n",
         pointer.operand, type, type, base.global_name ? "@" : "",
         base.global_name ? base.global_name : base.operand,
         is_parameter ? "" : "i64 0, ", index.operand);
    return pointer;
}

static Value indexed_address(past node, past *dimensions, char *result)
{
    if (node->nodeType == ARRAY_SUBSCRIPT_EXPR) {
        Value base = indexed_address(node->left, dimensions, result);
        if (!*dimensions) fail("too many array subscripts");
        Value index = expression(node->right, result);
        Value element = array_element(base, *dimensions, index, result);
        *dimensions = (*dimensions)->next;
        return element;
    }

    Value base = variable_address(node);
    Binding *local = find_local(node->svalue);
    *dimensions = local ? local->dimensions : find_global(node->svalue)->right;
    if (*dimensions && (*dimensions)->right)
        base.alignment = array_alignment(*dimensions);
    if (*dimensions && !(*dimensions)->right) {
        char type[TYPE_CAPACITY];
        array_type((*dimensions)->next, type);
        Value pointer = fresh(0);
        emit(result, "  %s = load %s*, %s** %s, align 8\n",
             pointer.operand, type, type, base.operand);
        return pointer;
    }
    return base;
}

/* Match the LLVM constant address syntax for fully indexed global arrays. */
static Value constant_array_address(past node)
{
    past base = node;
    int subscripts = 0;
    while (base->nodeType == ARRAY_SUBSCRIPT_EXPR) {
        int index;
        if (!constant_int(base->right, &index)) return (Value){0};
        ++subscripts;
        base = base->left;
    }
    if (base->nodeType != DECL_REF_EXPR || find_local(base->svalue))
        return (Value){0};
    past global = find_global(base->svalue);
    if (!global) return (Value){0};

    int rank = 0;
    for (past dim = global->right; dim; dim = dim->next) ++rank;
    if (rank != subscripts) return (Value){0};

    /* Only the offset modulo four elements affects alignment up to 16. */
    int offset = 0, stride = 1;
    past subscript = node;
    for (int axis = rank; axis > 0; --axis) {
        int index = 0;
        constant_int(subscript->right, &index);
        offset = (offset + (index % 4) * stride) % 4;
        past dim = global->right;
        for (int i = 1; i < axis; ++i) dim = dim->next;
        stride = (stride * (array_bound(dim) % 4)) % 4;
        subscript = subscript->left;
    }

    int alignment = array_alignment(global->right);
    if (offset % 2) alignment = 4;
    else if (offset && alignment > 8) alignment = 8;
    return (Value){.global_name = global->svalue, .constant_subscript = node,
                   .dimensions = global->right, .alignment = alignment};
}

static void emit_constant_address(past subscript, past dimensions, int depth,
                                  const char *name, char *result)
{
    past dimension = dimensions;
    for (int i = 1; i < depth; ++i) dimension = dimension->next;
    char type[TYPE_CAPACITY];
    int index = 0;
    array_type(dimension, type);
    constant_int(subscript->right, &index);
    emit(result, "getelementptr inbounds (%s, %s* ", type, type);
    if (depth > 1)
        emit_constant_address(subscript->left, dimensions, depth - 1, name, result);
    else
        emit(result, "@%s", name);
    emit(result, ", i64 0, i64 %d)", index);
}

static void emit_pointer(Value pointer, char *result)
{
    if (pointer.constant_subscript) {
        int depth = 0;
        for (past dim = pointer.dimensions; dim; dim = dim->next) ++depth;
        emit_constant_address(pointer.constant_subscript, pointer.dimensions,
                              depth, pointer.global_name, result);
    } else if (pointer.global_name) {
        emit(result, "@%s", pointer.global_name);
    } else {
        emit(result, "%s", pointer.operand);
    }
}

static Value address(past node, char *result)
{
    past dimensions;
    if (node && node->nodeType == ARRAY_SUBSCRIPT_EXPR) {
        Value constant = constant_array_address(node);
        if (constant.constant_subscript) return constant;
    }
    Value pointer = indexed_address(node, &dimensions, result);
    if (dimensions) fail("expected an integer array element");
    return pointer;
}

static Value array_argument(past node, past parameter, char *result)
{
    past dimensions;
    while (node->nodeType == PAREN_EXPR) node = node->left;
    Value base = indexed_address(node, &dimensions, result);
    if (!dimensions) fail("expected an array argument");

    char actual[TYPE_CAPACITY], expected[TYPE_CAPACITY];
    array_type(dimensions->next, actual);
    array_type(parameter->right->next, expected);
    if (strcmp(actual, expected) != 0) fail("array argument type mismatch");

    if (!dimensions->right) return base;
    return array_element(base, dimensions, (Value){.operand = "0"}, result);
}

static Value call(past node, char *result)
{
    const char *name = node->left->svalue;
    past function = functions;
    while (function && (function->nodeType != FUNCTION_DECL ||
                        strcmp(function->svalue, name) != 0))
        function = function->next;
    if (!function) fail("call to an undefined function");

    size_t count = 0;
    for (past arg = node->right; arg; arg = arg->next) ++count;
    size_t parameters = 0;
    for (past param = function->left; param; param = param->next) ++parameters;
    if (count != parameters) fail("wrong number of arguments");

    Value *arguments = count ? malloc(count * sizeof *arguments) : NULL;
    if (count && !arguments) fail("out of memory");
    size_t index = 0;
    past param = function->left;
    for (past arg = node->right; arg; arg = arg->next, param = param->next) {
        arguments[index++] = param->right ? array_argument(arg, param, result)
                                          : convert_value(expression(arg, result), param->is_float, result);
    }

    Value value = {.is_void = function->ivalue == Y_VOID};
    if (value.is_void) {
        emit(result, "  call void @%s(", name);
    } else {
        value = fresh(0);
        value.is_float = function->is_float;
        emit(result, "  %s = call %s @%s(", value.operand, value_type(value), name);
    }
    param = function->left;
    for (index = 0; index < count; ++index, param = param->next) {
        char type[TYPE_CAPACITY];
        array_type(param->right ? param->right->next : NULL, type);
        if (param->is_float) strcpy(type, "float");
        emit(result, "%s%s%s noundef %s", index ? ", " : "", type,
             param->right ? "*" : "", arguments[index].operand);
    }
    emit(result, ")\n");
    free(arguments);
    return value;
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


static int constant_float(past node, float *value);

static int expression_is_float(past node)
{
    switch (node->nodeType) {
    case FLOATING_LITERAL: return 1;
    case PAREN_EXPR: return expression_is_float(node->left);
    case UNARY_OPERATOR:
        return node->ivalue != Y_NOT && expression_is_float(node->left);
    case BINARY_OPERATOR:
        return arithmetic(node->ivalue) &&
               (expression_is_float(node->left) || expression_is_float(node->right));
    case DECL_REF_EXPR: {
        Binding *local = find_local(node->svalue);
        past global = local ? NULL : find_global(node->svalue);
        return local ? local->address.is_float : global && global->is_float;
    }
    case CALL_EXPR:
        for (past function = functions; function; function = function->next)
            if (function->nodeType == FUNCTION_DECL &&
                strcmp(function->svalue, node->left->svalue) == 0)
                return function->is_float;
        return 0;
    default: return 0;
    }
}

int genArithmeticExpr(past node, char *result)
{
    Value value = {0};
    float floating_constant;
    if (expression_is_float(node) && constant_float(node, &floating_constant)) {
        last_value = float_value(floating_constant);
        return 0;
    }
    int immediate;
    if (constant_int(node, &immediate)) {
        snprintf(value.operand, sizeof value.operand, "%d", immediate);
        last_value = value;
        return 0;
    }
    switch (node->nodeType) {
    case FLOATING_LITERAL:
        value = float_value(node->fvalue);
        break;
    case CALL_EXPR:
        value = call(node, result);
        break;
    case ARRAY_SUBSCRIPT_EXPR:
    case DECL_REF_EXPR: {
        Binding *binding = node->nodeType == DECL_REF_EXPR ? find_local(node->svalue) : NULL;
        past global = binding || node->nodeType != DECL_REF_EXPR ? NULL : find_global(node->svalue);
        int immediate;
        if ((binding && binding->has_constant) ||
            (global && global->is_const && !global->is_float && !global->right && global->left && constant_int(global->left, &immediate))) {
            if (binding) immediate = binding->constant;
            snprintf(value.operand, sizeof value.operand, "%d", immediate);
            break;
        }
        Value ptr = address(node, result);
        value = fresh(0);
        value.is_float = ptr.is_float;
        emit(result, "  %s = load %s, %s* ", value.operand, value_type(value), value_type(value));
        emit_pointer(ptr, result);
        emit(result, ", align %d\n", ptr.alignment ? ptr.alignment : 4);
        break;
    }
    case PAREN_EXPR:
        value = expression(node->left, result);
        break;
    case UNARY_OPERATOR: {
        Value operand = expression(node->left, result);
        if (!operand.is_float) operand = as_int(operand, result);
        value = operand;
        if (node->ivalue == Y_SUB) {
            value = fresh(0);

            value.is_float = operand.is_float;
            if (operand.is_float)
                emit(result, "  %s = fneg float %s\n", value.operand, operand.operand);
            else
                emit(result, "  %s = sub nsw i32 0, %s\n", value.operand, operand.operand);

        } else if (node->ivalue != Y_ADD) fail("unsupported unary operator");
        break;
    }
    case BINARY_OPERATOR: {
        const char *instruction = arithmetic(node->ivalue);
        if (!instruction) fail("unsupported arithmetic operator");

        int floating = expression_is_float(node->left) || expression_is_float(node->right);
        Value left = convert_value(expression(node->left, result), floating, result);
        Value right = convert_value(expression(node->right, result), floating, result);

        value = fresh(0);
        value.is_float = floating;
        if (floating) {
            if (node->ivalue == Y_MODULO) fail("remainder requires integer operands");
            const char *operation = node->ivalue == Y_ADD ? "fadd" :
                                    node->ivalue == Y_SUB ? "fsub" :
                                    node->ivalue == Y_MUL ? "fmul" : "fdiv";
            emit(result, "  %s = %s float %s, %s\n", value.operand, operation, left.operand, right.operand);
            break;
        }
        const char *flags = node->ivalue == Y_DIV || node->ivalue == Y_MODULO
            ? "" : " nsw";
        emit(result, "  %s = %s%s i32 %s, %s\n",
             value.operand, instruction, flags, left.operand, right.operand);
        break;
    }
    default:
        fail("expected an integer scalar expression");
    }
    last_value = value;
    return value.operand[0] == '%' ? atoi(value.operand + 1) : 0;
}

typedef struct PhiPredecessor {
    int block;
    struct PhiPredecessor *next;
} PhiPredecessor;

static void branch_on_collect(past node, int yes, int no, int merge,
                              PhiPredecessor **predecessors, char *result);

int genLogicExpr(past node, char *result)
{
    int immediate;
    if (constant_int(node, &immediate)) {
        last_value = (Value){.boolean = 1};
        strcpy(last_value.operand, immediate ? "true" : "false");
        return 0;
    }
    int op = node->ivalue;
    if (node->nodeType == UNARY_OPERATOR && op == Y_NOT) {
        Value operand = as_bool(expression(node->left, result), result);
        last_value = fresh(1);
        emit(result, "  %s = xor i1 %s, true\n", last_value.operand, operand.operand);
    } else if (node->nodeType != BINARY_OPERATOR || arithmetic(op)) {
        last_value = as_bool(expression(node, result), result);
    } else if (op == Y_AND || op == Y_OR) {
        if (constant_int(node->left, &immediate)) {
            last_value = as_bool(expression(node->right, result), result);
            return 0;
        }
        int rhs = ++label;
        int merge = ++label;
        PhiPredecessor *predecessors = NULL;
        if (op == Y_AND)
            branch_on_collect(node->left, rhs, merge, merge, &predecessors, result);
        else
            branch_on_collect(node->left, merge, rhs, merge, &predecessors, result);

        start_block(rhs, result);
        Value right = as_bool(expression(node->right, result), result);
        int rhs_exit = current_block;
        jump(merge, result);
        start_block(merge, result);
        last_value = fresh(1);
        emit(result, "  %s = phi i1 ", last_value.operand);
        while (predecessors) {
            PhiPredecessor *input = predecessors;
            if (input->block == 0)
                emit(result, "[ %s, %%%d ], ", op == Y_AND ? "false" : "true", entry_number);
            else
                emit(result, "[ %s, %%bb%d ], ", op == Y_AND ? "false" : "true", input->block);
            predecessors = input->next;
            free(input);
        }
        emit(result, "[ %s, %%bb%d ]\n", right.operand, rhs_exit);
    } else {
        const char *comparison = predicate(op);

        int floating = expression_is_float(node->left) || expression_is_float(node->right);
        Value left = convert_value(expression(node->left, result), floating, result);
        Value right = convert_value(expression(node->right, result), floating, result);
        last_value = fresh(1);

        if (floating) {
            const char *test = op == Y_LESS ? "olt" : op == Y_LESSEQ ? "ole" :
                               op == Y_GREAT ? "ogt" : op == Y_GREATEQ ? "oge" :
                               op == Y_EQ ? "oeq" : "une";
            emit(result, "  %s = fcmp %s float %s, %s\n",
                 last_value.operand, test, left.operand, right.operand);
        } else {
            emit(result, "  %s = icmp %s i32 %s, %s\n",
                 last_value.operand, comparison, left.operand, right.operand);
        }

    }
    return 0;
}

static void store(Value value, Value ptr, char *result)
{
    value = convert_value(value, ptr.is_float, result);
    emit(result, "  store %s %s, %s* ", value_type(value), value.operand, value_type(value));
    emit_pointer(ptr, result);
    emit(result, ", align %d\n", ptr.alignment ? ptr.alignment : 4);
}

int genAssignStmt(past node, char *result)
{
    Value value = expression(node->right, result);
    Value ptr = address(node->left, result);
    store(value, ptr, result);
    return 0;
}

static void statement(past node, char *result);

/* In a condition the branch itself carries the logical result. */
static void branch_on_collect(past node, int yes, int no, int merge,
                              PhiPredecessor **predecessors, char *result)
{
    if (node->nodeType == PAREN_EXPR) {
        branch_on_collect(node->left, yes, no, merge, predecessors, result);
    } else if (node->nodeType == UNARY_OPERATOR && node->ivalue == Y_NOT) {
        branch_on_collect(node->left, no, yes, merge, predecessors, result);
    } else if (node->nodeType == BINARY_OPERATOR && node->ivalue == Y_AND) {
        int rhs = ++label;
        branch_on_collect(node->left, rhs, no, merge, predecessors, result);
        start_block(rhs, result);
        branch_on_collect(node->right, yes, no, merge, predecessors, result);
    } else if (node->nodeType == BINARY_OPERATOR && node->ivalue == Y_OR) {
        int rhs = ++label;
        branch_on_collect(node->left, yes, rhs, merge, predecessors, result);
        start_block(rhs, result);
        branch_on_collect(node->right, yes, no, merge, predecessors, result);
    } else {
        genLogicExpr(node, result);
        if (predecessors && (yes == merge || no == merge)) {
            PhiPredecessor *input = malloc(sizeof *input);
            if (!input) fail("out of memory");
            input->block = current_block;
            input->next = *predecessors;
            *predecessors = input;
        }
        branch(last_value, yes, no, result);
    }
}

static void branch_on(past node, int yes, int no, char *result)
{
    branch_on_collect(node, yes, no, 0, NULL, result);
}

int genIfStmt(past node, char *result)
{
    int immediate;
    if (constant_int(node->if_cond, &immediate)) {
        statement(immediate ? node->left : node->right, result);
        return 0;
    }
    int yes = ++label;
    int done = ++label;
    int no = node->right ? ++label : done;
    branch_on(node->if_cond, yes, no, result);
    start_block(yes, result);
    statement(node->left, result);
    int yes_terminated = terminated;
    if (!terminated) jump(done, result);
    if (node->right) {
        start_block(no, result);
        statement(node->right, result);
        if (yes_terminated && terminated) return 0;
        if (!terminated) jump(done, result);
    }
    start_block(done, result);
    return 0;
}

int genWhileStmt(past node, char *result)
{
    int test = ++label;
    int body = ++label;
    int done = ++label;
    jump(test, result);
    start_block(test, result);
    int immediate;
    if (!constant_int(node->left, &immediate) || !immediate) {
        branch_on(node->left, body, done, result);
        start_block(body, result);
    }
    int old_break = break_target;
    int old_continue = continue_target;
    break_target = done;
    continue_target = test;
    statement(node->right, result);
    break_target = old_break;
    continue_target = old_continue;
    if (!terminated) jump(test, result);
    start_block(done, result);
    return 0;
}

/* Consume one aggregate. Brace groups have their own initializer cursor. */
static void initialize_array(past dimensions, past *item, Value base,
                             int global, char *result)
{
    if (global) emit(result, "[");
    int size = array_bound(dimensions);
    for (int i = 0; i < size; ++i) {
        if (global && i) emit(result, ", ");
        Value pointer = {0};
        if (!global) {
            Value index = {0};
            snprintf(index.operand, sizeof index.operand, "%d", i);
            pointer = array_element(base, dimensions, index, result);
        }
        if (dimensions->next) {
            if (global) {
                char type[TYPE_CAPACITY];
                array_type(dimensions->next, type);
                emit(result, "%s ", type);
            }
            if (*item && (*item)->nodeType == INIT_LIST_EXPR) {
                past nested = (*item)->left;
                *item = (*item)->next;
                initialize_array(dimensions->next, &nested, pointer, global, result);
                if (nested) fail("too many array initializers");
            } else {
                initialize_array(dimensions->next, item, pointer, global, result);
            }
            continue;
        }

        past scalar = *item;
        if (scalar) *item = scalar->next;
        while (scalar && scalar->nodeType == INIT_LIST_EXPR) {
            scalar = scalar->left;
            if (scalar && scalar->next) fail("too many scalar initializers");
        }
        if (global) {
            int initial = 0;
            if (scalar && !constant_int(scalar, &initial))
                fail("global array initializer must be constant");
            emit(result, "i32 %d", initial);
        } else {
            Value value = {.operand = "0"};
            if (scalar) value = expression(scalar, result);
            store(value, pointer, result);
        }
    }
    if (global) emit(result, "]");
}

static int constant_initializer(past node)
{
    for (; node; node = node->next) {
        if (node->nodeType == INIT_LIST_EXPR) {
            if (!constant_initializer(node->left)) return 0;
        } else {
            int value;
            if (!constant_int(node, &value)) return 0;
        }
    }
    return 1;
}

static char *array_constant_name(const char *variable)
{
    size_t capacity = strlen(current_function) + strlen(variable) + 48;
    char *name = malloc(capacity);
    if (!name) fail("out of memory");
    snprintf(name, capacity, "__const.%s.%s", current_function, variable);
    unsigned suffix = 0;
    for (;;) {
        ArrayConstant *existing = array_constants;
        while (existing && strcmp(existing->name, name) != 0) existing = existing->next;
        if (!existing) break;
        snprintf(name, capacity, "__const.%s.%s.%u", current_function, variable, ++suffix);
    }
    ArrayConstant *constant = malloc(sizeof *constant);
    if (!constant) fail("out of memory");
    constant->name = name;
    constant->next = array_constants;
    array_constants = constant;
    return name;
}

static void initialize_constant_array(past node, Value base, char *result)
{
    char type[TYPE_CAPACITY];
    array_type(node->right, type);
    uint64_t bytes = 4;
    for (past dim = node->right; dim; dim = dim->next) {
        unsigned size = (unsigned)array_bound(dim);
        if (bytes > UINT64_MAX / size) fail("array is too large");
        bytes *= size;
    }
    int alignment = array_alignment(node->right);
    past item = node->left->left;
    char *data = calloc(IR_CAPACITY, 1);
    if (!data) fail("out of memory");
    initialize_array(node->right, &item, (Value){0}, 1, data);
    if (item) fail("too many array initializers");

    Value destination = fresh(0);
    emit(result, "  %s = bitcast %s* %s to i8*\n",
         destination.operand, type, base.operand);
    if (is_zero_initializer(node->left->left)) {
        needs_memset = 1;
        emit(result, "  call void @llvm.memset.p0i8.i64(i8* align %d %s, i8 0, i64 %llu, i1 false)\n",
             alignment, destination.operand, (unsigned long long)bytes);
    } else {
        needs_memcpy = 1;
        const char *name = array_constant_name(node->svalue);
        emit(constant_data, "@%s = private unnamed_addr constant %s %s, align %d\n",
             name, type, data, alignment);
        emit(result, "  call void @llvm.memcpy.p0i8.p0i8.i64(i8* align %d %s, i8* align %d bitcast (%s* @%s to i8*), i64 %llu, i1 false)\n",
             alignment, destination.operand, alignment, type, name, (unsigned long long)bytes);
    }
    free(data);
}

static void declare(past node, char *result)
{
    Binding *binding = push_binding(node);
    snprintf(binding->address.operand, sizeof binding->address.operand,
             "%%%d", node->ivalue);
    binding->address.alignment = node->nodeType == VAR_DECL ? array_alignment(node->right) : 4;
    binding->has_constant = node->is_const && !node->is_float && !binding->dimensions && node->left &&
                            constant_int(node->left, &binding->constant);

    if (node->nodeType == VAR_DECL && node->left) {
        if (binding->dimensions) {
            if (node->left->nodeType != INIT_LIST_EXPR) fail("expected array initializer list");
            if (constant_initializer(node->left->left)) {
                initialize_constant_array(node, binding->address, result);
            } else {
                past item = node->left->left;
                initialize_array(binding->dimensions, &item, binding->address, 0, result);
                if (item) fail("too many array initializers");
            }
        } else {
            store(expression(node->left, result), binding->address, result);
        }
    }
}

static void statement(past node, char *result)
{
    if (!node || terminated) return;
    switch (node->nodeType) {
    case COMPOUND_STMT: {
        Binding *outer = locals;
        for (past item = node->left; item && !terminated; item = item->next)
            statement(item, result);
        pop_bindings(outer);
        break;
    }
    case DECL_STMT:
        for (past var = node->left; var; var = var->next)
            declare(var, result);
        break;
    case NULL_STMT: break;
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
        if (returns_void) {
            if (node->left) fail("void function cannot return a value");
            if (return_target) jump(return_target, result);
            else {
                emit(result, "  ret void\n");
                terminated = 1;
            }
            break;
        }
        Value value = convert_value(expression(node->left, result), returns_float, result);
        if (return_target) {
            store(value, return_slot, result);
            jump(return_target, result);
        } else {
            emit(result, "  ret %s %s\n", value_type(value), value.operand);
            terminated = 1;
        }
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
            char type[TYPE_CAPACITY];
            array_type(node->right, type);
            if (node->is_float) {
                if (node->right) fail("float arrays are not supported");
                strcpy(type, "float");
            }
            emit(result, "  %%%d = alloca %s, align %d\n",
                 node->ivalue, type, array_alignment(node->right));
        }
        int immediate;
        if (node->nodeType == IF_STMT && constant_int(node->if_cond, &immediate)) {
            allocate_locals(immediate ? node->left : node->right, result);
            continue;
        }
        allocate_locals(node->left, result);
        allocate_locals(node->right, result);
        allocate_locals(node->if_cond, result);
    }
}

/* LLVM assigns unnamed blocks and temporaries numbers in definition order. */
static void number_blocks(char *result, int return_alias)
{
    int *blocks = calloc((size_t)label + 1, sizeof *blocks);
    int *values = calloc((size_t)temporary + 1, sizeof *values);
    char *text = malloc(strlen(result) + 1);
    if (!blocks || !values || !text) fail("out of memory");
    strcpy(text, result);

    int slot = entry_number;
    for (int i = 0; i <= entry_number; ++i) values[i] = i;
    for (const char *line = text; line; ) {
        if (strncmp(line, "bb", 2) == 0)
            blocks[atoi(line + 2)] = ++slot;
        else if (strncmp(line, "  %", 3) == 0)
            values[atoi(line + 3)] = ++slot;
        line = strchr(line, '\n');
        if (line) ++line;
    }

    if (return_alias) blocks[return_target] = blocks[return_alias];
    result[0] = '\0';
    const char *segment = text;
    for (const char *p = text; *p; ) {
        const char *number = p;
        int is_block = strncmp(p, "bb", 2) == 0 &&
                       p[2] >= '0' && p[2] <= '9' &&
                       (p == text || p[-1] == '%' || p[-1] == '\n');
        int is_value = *p == '%' && p[1] >= '0' && p[1] <= '9';
        if (!is_block && !is_value) {
            ++p;
            continue;
        }
        number += is_block ? 2 : 1;
        char *end;
        long index = strtol(number, &end, 10);
        emit(result, "%.*s%s%d", (int)(p - segment), segment,
             is_value ? "%" : "", is_block ? blocks[index] : values[index]);
        segment = p = end;
    }
    emit(result, "%s", segment);
    free(text);
    free(values);
    free(blocks);
}

/* Count explicit returns on reachable paths and track implicit fallthrough. */
static int count_returns(past node, int *falls)
{
    int count = 0;
    *falls = 1;
    for (; node && *falls; node = node->next) {
        switch (node->nodeType) {
        case RETURN_STMT: ++count; *falls = 0; break;
        case COMPOUND_STMT: count += count_returns(node->left, falls); break;
        case WHILE_STMT: {
            int body_falls;
            count += count_returns(node->right, &body_falls);
            break;
        }
        case IF_STMT: {
            int immediate, yes_falls, no_falls;
            if (constant_int(node->if_cond, &immediate)) {
                count += count_returns(immediate ? node->left : node->right, falls);
            } else {
                count += count_returns(node->left, &yes_falls);
                count += count_returns(node->right, &no_falls);
                *falls = yes_falls || no_falls;
            }
            break;
        }
        default: break;
        }
    }
    return count;
}

static void gen_function(past node, char *result)
{
    result[0] = '\0';
    entry_number = label = break_target = continue_target = 0;
    current_function = node->svalue;
    returns_void = node->ivalue == Y_VOID;
    returns_float = node->is_float;
    emit(result, "define %s @%s(", returns_void ? "void" : returns_float ? "float" : "i32", node->svalue);
    for (past param = node->left; param; param = param->next) {
        char type[TYPE_CAPACITY];
        array_type(param->right ? param->right->next : NULL, type);
        if (param->is_float) strcpy(type, "float");
        emit(result, "%s%s%s noundef %%%d", entry_number ? ", " : "",
             type, param->right ? "*" : "", entry_number);
        ++entry_number;
    }
    emit(result, ") {\n");
    temporary = entry_number;
    current_block = terminated = 0;
    int falls;
    int returns = count_returns(node->right, &falls);
    return_target = returns + falls > 1 ? ++label : 0;
    int is_main = strcmp(node->svalue, "main") == 0;
    if (!returns_void && (is_main || return_target)) {
        return_slot = fresh(0);
        return_slot.is_float = returns_float;
        emit(result, "  %s = alloca %s, align 4\n", return_slot.operand, value_type(return_slot));
    }

    for (past param = node->left; param; param = param->next) {
        param->ivalue = ++temporary;
        char type[TYPE_CAPACITY];
        array_type(param->right ? param->right->next : NULL, type);
        if (param->is_float) strcpy(type, "float");
        emit(result, "  %%%d = alloca %s%s, align %d\n", param->ivalue,
             type, param->right ? "*" : "", param->right ? 8 : 4);
    }
    allocate_locals(node->right, result);
    if (is_main && !returns_void)
        emit(result, "  store i32 0, i32* %s, align 4\n", return_slot.operand);
    int index = 0;
    for (past param = node->left; param; param = param->next) {
        declare(param, result);
        char type[TYPE_CAPACITY];
        array_type(param->right ? param->right->next : NULL, type);
        if (param->is_float) strcpy(type, "float");
        const char *pointer = param->right ? "*" : "";
        emit(result, "  store %s%s %%%d, %s%s* %%%d, align %d\n", type, pointer,
             index++, type, pointer, param->ivalue, param->right ? 8 : 4);
    }
    statement(node->right, result);
    int return_alias = 0;
    if (return_target) {
        size_t length = strlen(result);
        if (!terminated && current_block && length >= 2 && result[length - 2] == ':') {
            return_alias = current_block;
        } else {
            if (!terminated) jump(return_target, result);
            start_block(return_target, result);
        }
        if (returns_void) {
            emit(result, "  ret void\n");
        } else {
            Value value = fresh(0);
            value.is_float = returns_float;
            emit(result, "  %s = load %s, %s* %s, align 4\n",
                 value.operand, value_type(value), value_type(value), return_slot.operand);
            emit(result, "  ret %s %s\n", value_type(value), value.operand);
        }
    } else if (!terminated) {
        emit(result, returns_void ? "  ret void\n" : returns_float ? "  ret float 0.000000e+00\n" : "  ret i32 0\n");
    }
    emit(result, "}\n");
    if (label != 0) number_blocks(result, return_alias);
    pop_bindings(NULL);
}


static int constant_float(past node, float *value)
{
    if (!node) { *value = 0; return 1; }
    if (node->nodeType == FLOATING_LITERAL) { *value = node->fvalue; return 1; }
    int integer;
    if (constant_int(node, &integer)) { *value = (float)integer; return 1; }
    if (node->nodeType == PAREN_EXPR) return constant_float(node->left, value);
    float left, right;
    if (node->nodeType == UNARY_OPERATOR && constant_float(node->left, &left)) {
        if (node->ivalue == Y_ADD) { *value = left; return 1; }
        if (node->ivalue == Y_SUB) { *value = -left; return 1; }
    }
    if (node->nodeType != BINARY_OPERATOR ||
        !constant_float(node->left, &left) || !constant_float(node->right, &right)) return 0;
    switch (node->ivalue) {
    case Y_ADD: *value = left + right; return 1;
    case Y_SUB: *value = left - right; return 1;
    case Y_MUL: *value = left * right; return 1;
    case Y_DIV: *value = left / right; return 1;
    default: return 0;
    }
}

void genProgram(past node, char *result)
{
    if (!node) fail("expected an int function definition");

    char *body = malloc(IR_CAPACITY);
    char *function_bodies = calloc(IR_CAPACITY, 1);
    constant_data = calloc(IR_CAPACITY, 1);
    if (!body || !function_bodies || !constant_data) fail("out of memory");
    needs_memcpy = needs_memset = 0;
    resolve_constants(node);
    pop_bindings(NULL);
    functions = node;
    result[0] = '\0';
    for (past global = node; global; global = global->next) {

        if (global->nodeType != VAR_DECL) continue;
        if (global->is_float) {
            if (global->right) fail("float arrays are not supported");
            float initial;
            if (!constant_float(global->left, &initial)) fail("expected a constant float initializer");
            Value value = float_value(initial);
            emit(result, "@%s = %s float %s, align 4\n", global->svalue,
                 global->is_const ? "constant" : "global", value.operand);
            continue;
        }
        if (global->right) {
            char type[TYPE_CAPACITY];
            array_type(global->right, type);
            emit(result, "@%s = %s %s ", global->svalue,
                 global->is_const ? "constant" : "global", type);
            if (!global->left || (global->left->nodeType == INIT_LIST_EXPR &&
                                  !global->left->left)) {
                emit(result, "zeroinitializer");
            } else {
                if (global->left->nodeType != INIT_LIST_EXPR) fail("expected array initializer list");
                past item = global->left->left;
                size_t start = strlen(result);
                initialize_array(global->right, &item, (Value){0}, 1, result);
                if (is_zero_initializer(global->left->left)) {
                    result[start] = '\0';
                    emit(result, "zeroinitializer");
                }
                if (item) fail("too many array initializers");
            }
            emit(result, ", align %d\n", array_alignment(global->right));
            continue;
        }
        int initial = 0;
        if (global->left && !constant_int(global->left, &initial))
            fail("global initializer must be a constant integer expression");
        emit(result, "@%s = %s i32 %d, align 4\n",
             global->svalue, global->is_const ? "constant" : "global", initial);
    }
    int emitted_function = 0;
    for (past function = node; function; function = function->next) {
        if (function->nodeType != FUNCTION_DECL) continue;
        gen_function(function, body);
        if (emitted_function) emit(function_bodies, "\n");
        emitted_function = 1;
        emit(function_bodies, "%s", body);
    }
    emit(result, "%s%s", constant_data, function_bodies);
    if (needs_memcpy)
        emit(result, "declare void @llvm.memcpy.p0i8.p0i8.i64(i8*, i8*, i64, i1)\n");
    if (needs_memset)
        emit(result, "declare void @llvm.memset.p0i8.i64(i8*, i8, i64, i1)\n");
    while (array_constants) {
        ArrayConstant *constant = array_constants;
        array_constants = constant->next;
        free(constant->name);
        free(constant);
    }
    functions = NULL;
    current_function = NULL;
    free(constant_data);
    constant_data = NULL;
    free(function_bodies);
    free(body);
}










