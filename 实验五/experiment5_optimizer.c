#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>

#define MAXN 3000
#define MAXLEN 32
#define MAXVAR 3000

typedef enum {
    IR_ASSIGN,
    IR_BINARY,
    IR_PRINT
} IRKind;

typedef struct {
    IRKind kind;
    char result[MAXLEN];
    char arg1[MAXLEN];
    char op[4];
    char arg2[MAXLEN];
    int deleted;
} IR;

IR code[MAXN];
int n;

/* ---------- 输出 ---------- */
void print_program(void) {
    for (int i = 0; i < n; i++) {
        if (code[i].deleted) continue;

        if (code[i].kind == IR_ASSIGN) {
            printf("%s = %s\n", code[i].result, code[i].arg1);
        } else if (code[i].kind == IR_BINARY) {
            printf("%s = %s %s %s\n",
                   code[i].result,
                   code[i].arg1,
                   code[i].op,
                   code[i].arg2);
        } else if (code[i].kind == IR_PRINT) {
            printf("print %s\n", code[i].arg1);
        }
    }
}

/* ---------- 请补充其它函数 ---------- */

/* Each instruction can mention up to three different variables. */
#define SYMBOL_SLOTS (6 * MAXN + 1)

static char symbols[SYMBOL_SLOTS][MAXLEN];

static int variable_id(const char *name) {
    if (!isalpha((unsigned char)name[0]) && name[0] != '_') return -1;

    unsigned hash = 0;
    for (const unsigned char *p = (const unsigned char *)name; *p; ++p) {
        hash = hash * 31u + *p;
    }
    unsigned slot = hash % SYMBOL_SLOTS;
    while (symbols[slot][0] && strcmp(symbols[slot], name) != 0) {
        slot = (slot + 1) % SYMBOL_SLOTS;
    }
    if (!symbols[slot][0]) strcpy(symbols[slot], name);
    return (int)slot;
}

static int read_program(void) {
    char line[256];
    if (scanf("%d", &n) != 1 || n < 0 || n > MAXN) return 0;
    if (n > 0 && !fgets(line, sizeof line, stdin)) return 0;

    for (int i = 0; i < n; ++i) {
        char first[MAXLEN], second[MAXLEN];
        IR *ins = &code[i];
        if (!fgets(line, sizeof line, stdin)) return 0;
        int count = sscanf(line, "%31s %31s %31s %3s %31s",
                           first, second, ins->arg1, ins->op, ins->arg2);
        if (count == 2 && strcmp(first, "print") == 0) {
            ins->kind = IR_PRINT;
            strcpy(ins->arg1, second);
        } else if ((count == 3 || count == 5) && strcmp(second, "=") == 0) {
            ins->kind = count == 3 ? IR_ASSIGN : IR_BINARY;
            strcpy(ins->result, first);
        } else {
            return 0;
        }
    }
    return 1;
}

static int substitute_constant(char *arg, const char *const *values) {
    int id = variable_id(arg);
    if (id < 0 || !values[id]) return 0;
    strcpy(arg, values[id]);
    return 1;
}

static int propagate_constants(void) {
    const char *values[SYMBOL_SLOTS] = {0};
    int changed = 0;
    for (int i = 0; i < n; ++i) {
        IR *ins = &code[i];
        if (ins->deleted) continue;
        changed |= substitute_constant(ins->arg1, values);
        if (ins->kind == IR_BINARY) {
            changed |= substitute_constant(ins->arg2, values);
        }
        if (ins->kind != IR_PRINT) {
            int dst = variable_id(ins->result);
            values[dst] = ins->kind == IR_ASSIGN && variable_id(ins->arg1) < 0
                          ? ins->arg1 : NULL;
        }
    }
    return changed;
}

typedef struct {
    int valid;
    int source;
    int source_version;
} Copy;

static int substitute_copy(char *arg, const Copy *copies, const int *versions) {
    int id = variable_id(arg);
    if (id < 0 || !copies[id].valid) return 0;
    Copy copy = copies[id];
    if (versions[copy.source] != copy.source_version) return 0;
    strcpy(arg, symbols[copy.source]);
    return 1;
}

static int propagate_copies(void) {
    Copy copies[SYMBOL_SLOTS] = {0};
    int versions[SYMBOL_SLOTS] = {0};
    int changed = 0;
    for (int i = 0; i < n; ++i) {
        IR *ins = &code[i];
        if (ins->deleted) continue;
        changed |= substitute_copy(ins->arg1, copies, versions);
        if (ins->kind == IR_BINARY) {
            changed |= substitute_copy(ins->arg2, copies, versions);
        }
        if (ins->kind == IR_PRINT) continue;

        int dst = variable_id(ins->result);
        int src = variable_id(ins->arg1);
        if (ins->kind == IR_ASSIGN && dst == src) continue;
        ++versions[dst];
        copies[dst] = (Copy){ins->kind == IR_ASSIGN && src >= 0, src,
                             src >= 0 ? versions[src] : 0};
    }
    return changed;
}

static int fold_constants(void) {
    int changed = 0;
    for (int i = 0; i < n; ++i) {
        IR *ins = &code[i];
        if (ins->deleted || ins->kind != IR_BINARY ||
            variable_id(ins->arg1) >= 0 || variable_id(ins->arg2) >= 0) continue;

        long long left = strtoll(ins->arg1, NULL, 10);
        long long right = strtoll(ins->arg2, NULL, 10);
        long long value;
        switch (ins->op[0]) {
            case '+': value = left + right; break;
            case '-': value = left - right; break;
            case '*': value = left * right; break;
            case '/':
                if (right == 0) continue;
                value = left / right;
                break;
            default: continue;
        }
        snprintf(ins->arg1, sizeof ins->arg1, "%lld", value);
        ins->kind = IR_ASSIGN;
        changed = 1;
    }
    return changed;
}

static int is_constant(const char *arg, long value) {
    return variable_id(arg) < 0 && strtol(arg, NULL, 10) == value;
}

static int simplify_algebra(void) {
    int changed = 0;
    for (int i = 0; i < n; ++i) {
        IR *ins = &code[i];
        if (ins->deleted || ins->kind != IR_BINARY) continue;
        const char *left = ins->arg1;
        const char *right = ins->arg2;
        char op = ins->op[0];

        if (op == '*' && (is_constant(left, 0) || is_constant(right, 0))) {
            strcpy(ins->arg1, "0");
        } else if (((op == '+' || op == '-') && is_constant(right, 0)) ||
                   ((op == '*' || op == '/') && is_constant(right, 1))) {
            /* Keep the left operand. */
        } else if ((op == '+' && is_constant(left, 0)) ||
                   (op == '*' && is_constant(left, 1))) {
            strcpy(ins->arg1, right);
        } else {
            continue;
        }
        ins->kind = IR_ASSIGN;
        changed = 1;
    }
    return changed;
}

static void mark_live(unsigned char *live, const char *arg) {
    int id = variable_id(arg);
    if (id >= 0) live[id] = 1;
}

static int eliminate_dead_code(void) {
    unsigned char live[SYMBOL_SLOTS] = {0};
    int changed = 0;
    for (int i = n - 1; i >= 0; --i) {
        IR *ins = &code[i];
        if (ins->deleted) continue;
        if (ins->kind != IR_PRINT) {
            int dst = variable_id(ins->result);
            if (!live[dst]) {
                ins->deleted = 1;
                changed = 1;
                continue;
            }
            live[dst] = 0;
        }
        mark_live(live, ins->arg1);
        if (ins->kind == IR_BINARY) mark_live(live, ins->arg2);
    }
    return changed;
}

int main(void) {
    if (!read_program()) return EXIT_FAILURE;

    int changed;
    do {
        changed = propagate_constants();
        changed |= propagate_copies();
        changed |= fold_constants();
        changed |= simplify_algebra();
        changed |= eliminate_dead_code();
    } while (changed);

    print_program();
    return EXIT_SUCCESS;
}
