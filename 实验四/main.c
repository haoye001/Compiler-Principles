#include <stdio.h>
#include <stdlib.h>
#include "codegen.h"

extern int yyparse(void);
extern past ast_root;
extern FILE *yyin;

int main(int argc, char **argv)
{
    static char result[IR_CAPACITY];
    FILE *input = stdin;

    if (argc > 2) {
        fprintf(stderr, "Usage: %s [source.c]\n", argv[0]);
        return EXIT_FAILURE;
    }
    if (argc == 2) {
        input = fopen(argv[1], "r");
        if (input == NULL) {
            perror(argv[1]);
            return EXIT_FAILURE;
        }
    }

    /* Match the sample's test.c convention when no stdin was supplied. */
    if (argc == 1) {
        int first = fgetc(stdin);
        if (first == EOF) {
            input = fopen("test.c", "r");
            if (input == NULL) {
                perror("test.c");
                return EXIT_FAILURE;
            }
        } else {
            ungetc(first, stdin);
        }
    }

    yyin = input;
    int status = yyparse();
    if (input != stdin) fclose(input);
    if (status != 0) return EXIT_FAILURE;

    genProgram(ast_root, result);
    return fputs(result, stdout) == EOF ? EXIT_FAILURE : EXIT_SUCCESS;
}

