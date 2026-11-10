#ifndef CODEGEN_H
#define CODEGEN_H

#include "ast.h"

enum { IR_CAPACITY = 1024 * 1024 };

/* result is a NUL-terminated buffer of at least IR_CAPACITY bytes.
   genProgram initializes the buffer and the current function context. */
void genArithmeticExpr(past node, char *result);
void genLogicExpr(past node, char *result);
void genAssignStmt(past node, char *result);
void genIfStmt(past node, char *result);
void genWhileStmt(past node, char *result);
void genProgram(past node, char *result);

#endif
