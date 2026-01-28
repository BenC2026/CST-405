%{
/* LEXICAL ANALYZER (SCANNER)
 * This is the first phase of compilation - breaking input into tokens
 * Flex generates a scanner from these rules that reads character by character
 */

 /* Majority of code by Isac Artzi, with modifications by Ben Croyle */
 /* Lines 26 and 27 detect common coding terms such as int or print and return their respective tokens. */
 /* Lines 29 through 32 detect whether the keyword is a variable consisting of letters followed by either more letters or numbers and returns the ID token. */
 /* Lines 34 through 37 detect whether the keyword is a number and returns the NUM token. */
 /* Lines 39 through 47 detect operators and miscellaneous symbols and returns their respective tokens. */
 /* Line 49 returns nothing if spaces, tabs, or newlines are detected. */
 /* Line 51 returns an error message and the location of the erroneous character if it comes across one. */

#include <stdio.h>
#include <stdlib.h>
#include "parser.tab.h"  /* Token definitions from Bison */
%}

/* Flex options to suppress warnings for unused functions */
%option nounput
%option noinput

%%

"int"           { return INT; }
"print"         { return PRINT; }
"float"         { return FLOAT; }

[a-zA-Z_][a-zA-Z0-9_]* { 
    yylval.str = strdup(yytext);
    return ID; 
}

[0-9]+          { 
    yylval.num = atoi(yytext);
    return NUM; 
}

"+"             { return '+'; }
"-"             { return '-'; }
"*"             { return '*'; }
"/"             { return '/'; }
"%"             { return '%'; }
"="             { return '='; }
";"             { return ';'; }
"("             { return '('; }
")"             { return ')'; }
"/*"           { return "/*"; }
"*/"           { return "*/"; }

[ \t\n]         { }

.               { printf("Lexical Error: Unknown character '%c'\n", *yytext); }

%%

/* Required by Flex - signals end of input */
int yywrap() {
    return 1;  /* 1 means no more input files */
}
