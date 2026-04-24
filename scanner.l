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

#define NUM_KEYWORDS 21
static struct keyword keywords[] = {
    {"int", INT},
    {"char", CHAR_KW},
    {"boolean", BOOLEAN_KW},
    {"void", VOID_KW},
    {"true", TRUE_LIT},
    {"false", FALSE_LIT},
    {"string", STRING_KW},
    {"float", FLOAT},
    {"print", PRINT},
    {"return", RETURN},
    {"if", IF},
    {"else", ELSE},
    {"while", WHILE},
    {"for", FOR},
    {"switch", SWITCH},
    {"case", CASE},
    {"break", BREAK},
    {"default", DEFAULT},
    {"and", AND_KW},
    {"or", OR_KW},
    {"not", NOT_KW}
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
"char"          { return CHAR_KW; }
"boolean"       { return BOOLEAN_KW; }
"void"          { return VOID_KW; }
"true"          { yylval.num = 1; return TRUE_LIT; }
"false"         { yylval.num = 0; return FALSE_LIT; }
"string"        { return STRING_KW; }
"float"         { return FLOAT; }
"print"         { return PRINT; }
"return"        { return RETURN; }
"if"            { return IF; }
"else"          { return ELSE; }
"while"         { return WHILE; }
"for"           { return FOR; }
"switch"        { return SWITCH; }
"case"          { return CASE; }
"break"         { return BREAK; }
"default"       { return DEFAULT; }
"and"           { return AND_KW; }
"or"            { return OR_KW; }
"not"           { return NOT_KW; }

":"             { return ':'; }

[a-zA-Z_][a-zA-Z0-9_]* {
    int token = check_keyword(yytext);
    if (token == ID) {
        yylval.str = strdup(yytext);
    }
    return token;
}

[0-9]+\.[0-9]+  {
    yylval.fval = atof(yytext);
    return FLOAT_LIT;
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

\"([^\\\"]|\\.)*\"   {
    /* strip surrounding quotes */
    yylval.sval = strndup(yytext+1, yyleng-2);
    return STRING_LITERAL;
}

\'([^\'\\]|\\.)\'  {
    /* character literal: 'a', '\n', '\\', etc. */
    char c = yytext[1];
    if (c == '\\') {
        switch (yytext[2]) {
            case 'n':  c = '\n'; break;
            case 't':  c = '\t'; break;
            case '\\': c = '\\'; break;
            case '\'': c = '\''; break;
            case '0':  c = '\0'; break;
            default:   c = yytext[2]; break;
        }
    }
    yylval.num = (int)(unsigned char)c;
    return CHAR_LIT;
}

[ \t\n]         { }

.               { printf("Lexical Error: Unknown character '%c'\n", *yytext); }

%%

int yywrap() {
    return 1;
}
