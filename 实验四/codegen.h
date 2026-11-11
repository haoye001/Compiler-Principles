#ifndef CODEGEN_H
#define CODEGEN_H

#include "ast.h"

enum { IR_CAPACITY = 1024 * 1024 };

/* result is a NUL-terminated buffer of at least IR_CAPACITY bytes.
   genProgram initializes the buffer and the current function context. */
/* Arithmetic returns the result temporary number, or 0 for an immediate.
   Logic and statement generators return 0 on success, as in the sample. */
int genArithmeticExpr(past node, char *result);
int genLogicExpr(past node, char *result);
int genAssignStmt(past node, char *result);
int genIfStmt(past node, char *result);
int genWhileStmt(past node, char *result);
void genProgram(past node, char *result);

#endif

