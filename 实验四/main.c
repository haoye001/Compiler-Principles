#include <stdio.h>
#include <stdlib.h>
#include "codegen.h"

extern int yyparse(void);
extern past ast_root;

int main(void)
{
    static char result[IR_CAPACITY];

    if (yyparse() != 0) return EXIT_FAILURE;
    genProgram(ast_root, result);
    return fputs(result, stdout) == EOF ? EXIT_FAILURE : EXIT_SUCCESS;
}
