%option noyywrap yylineno nodefault never-interactive nounistd noinput nounput

%{
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "lrparser.tab.h"
%}

DIGIT       [0-9]
LETTER      [a-zA-Z_]
ID          {LETTER}({LETTER}|{DIGIT})*
HEX_DIGIT   [0-9a-fA-F]
EXPONENT    [eE][+-]?{DIGIT}+
DEC_INT     [1-9]{DIGIT}*
OCT_INT     0[0-7]*
HEX_INT     0[xX]{HEX_DIGIT}+
FLOAT_NUM   (({DIGIT}+"."{DIGIT}*|"."{DIGIT}+){EXPONENT}?|{DIGIT}+{EXPONENT})
HEX_FLOAT   0[xX]({HEX_DIGIT}+("."{HEX_DIGIT}*)?|"."{HEX_DIGIT}+)[pP][+-]?{DIGIT}+

%%

[ \t\r\n\v\f]+              ;

"//"[^\n]*                  ;
\/\*([^*]|\*+[^*/])*\*+\/   ;

"const"                     { return Y_CONST; }
"int"                       { return Y_INT; }
"float"                     { return Y_FLOAT; }
"void"                      { return Y_VOID; }

"if"                        { return Y_IF; }
"else"                      { return Y_ELSE; }
"while"                     { return Y_WHILE; }
"break"                     { return Y_BREAK; }
"continue"                  { return Y_CONTINUE; }
"return"                    { return Y_RETURN; }

"&&"                        { return Y_AND; }
"||"                        { return Y_OR; }
"=="                        { return Y_EQ; }
"!="                        { return Y_NOTEQ; }
"<="                        { return Y_LESSEQ; }
">="                        { return Y_GREATEQ; }

"+"                         { return Y_ADD; }
"-"                         { return Y_SUB; }
"*"                         { return Y_MUL; }
"/"                         { return Y_DIV; }
"%"                         { return Y_MODULO; }
"<"                         { return Y_LESS; }
">"                         { return Y_GREAT; }
"!"                         { return Y_NOT; }
"="                         { return Y_ASSIGN; }


"("                         { return Y_LPAR; }
")"                         { return Y_RPAR; }


"{"                         { return Y_LBRACKET; }
"}"                         { return Y_RBRACKET; }


"["                         { return Y_LSQUARE; }
"]"                         { return Y_RSQUARE; }

","                         { return Y_COMMA; }
";"                         { return Y_SEMICOLON; }

{FLOAT_NUM}|{HEX_FLOAT}     {
                                yylval.fvalue = strtof(yytext, NULL);
                                return num_FLOAT;
                            }

{HEX_INT}|{OCT_INT}|{DEC_INT} {
                                yylval.ivalue =
                                    (int)strtol(yytext, NULL, 0);
                                return num_INT;
                            }
{ID}                        {
                                yylval.svalue =
                                    (char *)malloc((size_t)yyleng + 1);

                                if (yylval.svalue == NULL) {
                                    fprintf(stderr,
                                            "Memory allocation failed\n");
                                    exit(EXIT_FAILURE);
                                }

                                memcpy(yylval.svalue, yytext,
                                       (size_t)yyleng);
                                yylval.svalue[yyleng] = '\0';

                                return Y_ID;
                            }


.                           {
                                fprintf(stderr,
                                        "Unknown character '%s' at line %d\n",
                                        yytext, yylineno);
                                return yytext[0];
                            }

%%
