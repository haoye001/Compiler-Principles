#include <stdio.h>
#include <stdlib.h>
#include "ast.h"
extern int yyparse(void);
extern past ast_root;
int main(void){
    if(yyparse()==0){
        showAst(ast_root, 0, false);
        return EXIT_SUCCESS;
    }
    return EXIT_FAILURE;
}