%{
/* LEXICAL ANALYZER (SCANNER) - WITH FUNCTION SUPPORT
 * This is the first phase of compilation - breaking input into tokens
 * Flex generates a scanner from these rules that reads character by character
 * Now supports functions, control flow, and more operators
 */
#include <stdio.h>
#include <stdlib.h>
#include "parser.tab.h"  /* Token definitions from Bison */
#define YY_BUF_SIZE 32768

struct keyword {
    char* word;
    int token;
};

#define NUM_KEYWORDS 7
static struct keyword keywords[] = {
    {"int", INT},
    {"float", FLOAT},
    {"print", PRINT},
    {"return", RETURN},
    {"if", IF},
    {"else", ELSE},
    {"while", WHILE}
};

int keyword_hash(const char* str) {
    int len = strlen(str);
    return (len + str[0]) % 13;  // Simple but effective for our set
}

int check_keyword(const char* str) {
    int h = keyword_hash(str);
    // Check if it matches our keyword at that hash
    for(int i = 0; i < NUM_KEYWORDS; i++) {
        if(strcmp(keywords[i].word, str) == 0) {
            return keywords[i].token;
        }
    }
    return ID;  // Not a keyword, must be identifier
}

%}

/* Flex options to suppress warnings for unused functions */
%option nounput
%option noinput
%option yylineno

%%

"int"           { return INT; }
"float"         { return FLOAT; }
"print"         { return PRINT; }
"return"        { return RETURN; }
"if"            { return IF; }
"else"          { return ELSE; }
"while"         { return WHILE; }

[a-zA-Z_][a-zA-Z0-9_]* {
    int token = check_keyword(yytext);
    if (token == ID) {
        yylval.str = strdup(yytext);
    }
    return token;
}

[0-9]+          {
    yylval.num = atoi(yytext);
    return NUM;
}

"<="            { return LE; }
">="            { return GE; }
"=="            { return EQ; }
"!="            { return NE; }

"+"             { return '+'; }
"-"             { return '-'; }
"*"             { return '*'; }
"/"             { return '/'; }

"<"             { return '<'; }
">"             { return '>'; }

"="             { return '='; }

";"             { return ';'; }
"("             { return '('; }
")"             { return ')'; }
"["             { return '['; }
"]"             { return ']'; }
"{"             { return '{'; }
"}"             { return '}'; }
","             { return ','; }

"//".*          { /* Skip single-line comments */ }
"/*"([^*]|\*+[^*/])*\*+"/"  { /* Skip multi-line comments */ }

[ \t\n]         { }

.               { printf("Lexical Error: Unknown character '%c'\n", *yytext); }

%%

int yywrap() {
    return 1;
}
