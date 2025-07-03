%{
#include "parser.tab.h"
#include <string.h>
#include <stdlib.h>

#ifdef _WIN32
#define YY_NO_UNISTD_H
#endif

extern int yylineno;

char* copy_string(const char* text) {
    char* str = (char*)malloc(strlen(text) + 1);
    strcpy(str, text);
    return str;
}
%}

%option noyywrap
%option yylineno
%option nounistd

%%

"contract"      { return CONTRACT; }
"function"      { return FUNCTION; }
"public"        { return PUBLIC; }
"returns"       { return RETURNS; }
"if"            { return IF; }
"else"          { return ELSE; }
"return"        { return RETURN; }
"true"          { return TRUE; }
"false"         { return FALSE; }

"uint"          { return UINT; }
"int"           { return INT; }
"bool"          { return BOOL; }
"address"       { return ADDRESS; }

"+"             { return PLUS; }
"-"             { return MINUS; }
"*"             { return MULTIPLY; }
"/"             { return DIVIDE; }
"%"             { return MODULUS; }
"="             { return ASSIGN; }
"=="            { return EQUAL; }
"!="            { return NOTEQUAL; }
">"             { return GREATERTHAN; }
"<"             { return LESSERTHAN; }
">="            { return GREATEREQUAL; }
"<="            { return LESSEREQUAL; }
"&&"            { return AND; }
"||"            { return OR; }
"!"             { return NOT; }

"("             { return LPAREN; }
")"             { return RPAREN; }
"{"             { return LBRACE; }
"}"             { return RBRACE; }
";"             { return SEMICOLON; }
","             { return COMMA; }

"0x"[0-9a-fA-F]+ { yylval.strval = copy_string(yytext); return ADDRESS_LITERAL; }
[0-9]+          { yylval.intval = atoi(yytext); return INT_LITERAL; }
[a-zA-Z][a-zA-Z0-9_]*  { yylval.strval = copy_string(yytext); return IDENTIFIER; }

[ \t\r\n]+      { }
"//".*          { }

.               {
                    fprintf(stderr, "WRONG: %c at line %d\n", yytext[0], yylineno);
                    return ERROR;
                }

%%