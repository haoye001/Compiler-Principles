%{
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "token.h"
char *endptr;
int int_value;
float float_value;
void print_token(int token, const char *info)
{
    printf("<%d, %s>\n", token, info);
}
%}
%option noyywrap
DIGIT    [0-9]
HEXDIGIT [0-9a-fA-F]
SIGN     [+-]
LETTER   [A-Za-z_]
ID       {LETTER}({LETTER}|{DIGIT})*
%%
[ \t\r\n]+                    {}
"//".*                         {}
"/*"([^*]|\*+[^*/])*\*+"/"     {}
0[xX]{HEXDIGIT}+               {
                                    int_value = (int)strtol(yytext, &endptr, 16);
                                    printf("<%d, %d>\n", num_INT, int_value);
                                }
{SIGN}?{DIGIT}+"."{DIGIT}+     {
                                    float_value = (float)atof(yytext);
                                    printf("<%d, %f>\n", num_FLOAT, float_value);
                                }
{SIGN}?{DIGIT}+                {
                                    int_value = (int)strtol(yytext, &endptr, 10);
                                    printf("<%d, %d>\n", num_INT, int_value);
                                }
"int"                          { print_token(Y_INT, "KEYWORD"); }
"void"                         { print_token(Y_VOID, "KEYWORD"); }
"const"                        { print_token(Y_CONST, "KEYWORD"); }
"if"                           { print_token(Y_IF, "KEYWORD"); }
"else"                         { print_token(Y_ELSE, "KEYWORD"); }
"while"                        { print_token(Y_WHILE, "KEYWORD"); }
"break"                        { print_token(Y_BREAK, "KEYWORD"); }
"continue"                     { print_token(Y_CONTINUE, "KEYWORD"); }
"return"                       { print_token(Y_RETURN, "KEYWORD"); }
"float"                        { print_token(Y_FLOAT, "KEYWORD"); }
{ID}                           { print_token(Y_ID, yytext); }
"<="                           { print_token(Y_LESSEQ, "OPERATOR"); }
">="                           { print_token(Y_GREATEQ, "OPERATOR"); }
"!="                           { print_token(Y_NOTEQ, "OPERATOR"); }
"=="                           { print_token(Y_EQ, "OPERATOR"); }
"&&"                           { print_token(Y_AND, "OPERATOR"); }
"||"                           { print_token(Y_OR, "OPERATOR"); }
"+"                            { print_token(Y_ADD, "OPERATOR"); }
"-"                            { print_token(Y_SUB, "OPERATOR"); }
"*"                            { print_token(Y_MUL, "OPERATOR"); }
"/"                            { print_token(Y_DIV, "OPERATOR"); }
"%"                            { print_token(Y_MODULO, "OPERATOR"); }
"<"                            { print_token(Y_LESS, "OPERATOR"); }
">"                            { print_token(Y_GREAT, "OPERATOR"); }
"!"                            { print_token(Y_NOT, "OPERATOR"); }
"="                            { print_token(Y_ASSIGN, "OPERATOR"); }
"("                            { print_token(Y_LPAR, "SYMBOL"); }
")"                            { print_token(Y_RPAR, "SYMBOL"); }
"{"                            { print_token(Y_LBRACKET, "SYMBOL"); }
"}"                            { print_token(Y_RBRACKET, "SYMBOL"); }
"["                            { print_token(Y_LSQUARE, "SYMBOL"); }
"]"                            { print_token(Y_RSQUARE, "SYMBOL"); }
","                            { print_token(Y_COMMA, "SYMBOL"); }
";"                            { print_token(Y_SEMICOLON, "SYMBOL"); }
.                              { fprintf(stderr, "Unknown character: %s\n", yytext); }
%%
int main(void)
{
    yylex();
    return 0;
}