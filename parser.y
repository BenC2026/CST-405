%{
/* SYNTAX ANALYZER (PARSER) - WITH FUNCTION SUPPORT
 * This is the second phase of compilation - checking grammar rules
 * Bison generates a parser that builds an Abstract Syntax Tree (AST)
 * Now supports functions, control flow, and more operators
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "ast.h"

/* External declarations for lexer interface */
extern int yylex();
extern int yyparse();
extern FILE* yyin;
extern int yylineno;  /* Line number from scanner */

void yyerror(const char* s);
ASTNode* root = NULL;
%}

/* SEMANTIC VALUES UNION */
%union {
    int num;
    double fval;
    char* str;
    char* sval;
    struct ASTNode* node;
}

/* TOKEN DECLARATIONS */
%token <num> NUM
%token <fval> FLOAT_LIT
%token <str> ID
%token <sval> STRING_LITERAL
%token INT STRING_KW FLOAT CHAR_KW BOOLEAN_KW VOID_KW PRINT RETURN IF ELSE WHILE FOR
%token SWITCH CASE BREAK DEFAULT
%token LE GE EQ NE
%token AND_KW OR_KW NOT_KW
%token <num> CHAR_LIT TRUE_LIT FALSE_LIT

/* NON-TERMINAL TYPES */
%type <node> program decl_or_func_list decl_or_func
%type <node> func_def params param_list param
%type <node> stmt_list stmt decl assign expr print_stmt return_stmt
%type <node> switch_stmt break_stmt case_list case_clause opt_stmt_list
%type <node> if_stmt while_stmt for_stmt for_init for_cond for_update block
%type <node> func_call arg_list args

/* OPERATOR PRECEDENCE AND ASSOCIATIVITY */
%left OR_KW
%left AND_KW
%right NOT_KW
%left EQ NE
%left '<' '>' LE GE
%left '+' '-'
%left '*' '/'
%right UMINUS

%%

/* PROGRAM RULE - Entry point */
program:
    decl_or_func_list {
        root = $1;
    }
    ;

/* Declaration or function list */
decl_or_func_list:
    decl_or_func {
        $$ = $1;
    }
    | decl_or_func_list decl_or_func {
        $$ = createStmtList($1, $2);
    }
    ;

/* Can be a function definition, variable declaration, or global assignment */
decl_or_func:
    func_def { $$ = $1; }
    | decl { $$ = $1; }
    | assign { $$ = $1; }
    ;

/* FUNCTION DEFINITION - int name(params) { body } */
func_def:
    INT ID '(' params ')' block {
        $$ = createFuncDef($2, $4, $6);
        free($2);
    }
    | FLOAT ID '(' params ')' block {
        $$ = createFuncDef($2, $4, $6);
        free($2);
    }
    | INT ID '(' ')' block {
        $$ = createFuncDef($2, NULL, $5);
        free($2);
    }
    | FLOAT ID '(' ')' block {
        $$ = createFuncDef($2, NULL, $5);
        free($2);
    }
    | STRING_KW ID '(' params ')' block {
        $$ = createFuncDef($2, $4, $6);
        free($2);
    }
    | STRING_KW ID '(' ')' block {
        $$ = createFuncDef($2, NULL, $5);
        free($2);
    }
    | INT ID '[' ']' '(' params ')' block {
        $$ = createFuncDef($2, $6, $8);
        free($2);
    }
    | INT ID '[' ']' '(' ')' block {
        $$ = createFuncDef($2, NULL, $7);
        free($2);
    }
    | VOID_KW ID '(' params ')' block {
        $$ = createFuncDef($2, $4, $6);
        free($2);
    }
    | VOID_KW ID '(' ')' block {
        $$ = createFuncDef($2, NULL, $5);
        free($2);
    }
    | BOOLEAN_KW ID '(' params ')' block {
        $$ = createFuncDef($2, $4, $6);
        free($2);
    }
    | BOOLEAN_KW ID '(' ')' block {
        $$ = createFuncDef($2, NULL, $5);
        free($2);
    }
    | CHAR_KW ID '(' params ')' block {
        $$ = createFuncDef($2, $4, $6);
        free($2);
    }
    | CHAR_KW ID '(' ')' block {
        $$ = createFuncDef($2, NULL, $5);
        free($2);
    }
    ;

/* PARAMETERS */
params:
    param_list { $$ = $1; }
    ;

param_list:
    param {
        $$ = $1;
    }
    | param_list ',' param {
        $$ = createParamList($1, $3);
    }
    ;

param:
    INT ID {
        $$ = createParam($2);
        free($2);
    }
    | FLOAT ID {
        $$ = createParam($2);
        free($2);
    }
    | INT ID '[' ']' {
        $$ = createParam($2);
        free($2);
    }
    | STRING_KW ID {
        $$ = createParam($2);
        free($2);
    }
    | STRING_KW ID '[' ']' {
        $$ = createParam($2);
        free($2);
    }
    | CHAR_KW ID {
        $$ = createParam($2);
        free($2);
    }
    | BOOLEAN_KW ID {
        $$ = createParam($2);
        free($2);
    }
    ;

/* BLOCK STATEMENT */
block:
    '{' stmt_list '}' {
        $$ = createBlock($2);
    }
    | '{' '}' {
        $$ = createBlock(NULL);
    }
    ;

/* STATEMENT LIST */
stmt_list:
    stmt {
        $$ = $1;
    }
    | stmt_list stmt {
        $$ = createStmtList($1, $2);
    }
    ;

/* STATEMENT TYPES */
stmt:
    decl
    | assign
    | print_stmt
    | return_stmt
    | if_stmt
    | while_stmt
    | for_stmt
    | switch_stmt
    | break_stmt
    | block
    | func_call ';' { $$ = $1; }
    ;

/* DECLARATION - int x; */
decl:
    INT ID ';' {
        $$ = createDecl($2, "int");
        addVar($2, "int");
        free($2);
    }
    | FLOAT ID ';' {
        $$ = createDecl($2, "float");
        addVar($2, "float");
        free($2);
    }
    | FLOAT ID '[' NUM ']' ';' {
        $$ = createArrayDecl($2, "float", $4);
        addArrayVar($2, "float", $4);
        free($2);
    }
    | INT ID '[' NUM ']' ';' {
        $$ = createArrayDecl($2, "int", $4);
        addArrayVar($2, "int", $4);
        free($2);
    }
    | STRING_KW ID ';' {
        $$ = createDecl($2, "string");
        addVar($2, "string");
        free($2);
    }
    | STRING_KW ID '[' NUM ']' ';' {
        $$ = createArrayDecl($2, "string", $4);
        addArrayVar($2, "string", $4);
        free($2);
    }
    | CHAR_KW ID ';' {
        $$ = createDecl($2, "char");
        addVar($2, "char");
        free($2);
    }
    | BOOLEAN_KW ID ';' {
        $$ = createDecl($2, "boolean");
        addVar($2, "boolean");
        free($2);
    }
    ;

/* ASSIGNMENT - x = expr; */
assign:
    ID '=' expr ';' {
        $$ = createAssign($1, $3);
        free($1);
    }
    | ID '[' expr ']' '=' expr ';' {
        $$ = createArrayAssign($1, $3, $6);
        free($1);
    }
    ;

/* RETURN STATEMENT */
return_stmt:
    RETURN expr ';' {
        $$ = createReturn($2);
    }
    | RETURN ';' {
        $$ = createReturn(NULL);
    }
    ;

/* IF STATEMENT */
if_stmt:
    IF '(' expr ')' stmt {
        $$ = createIf($3, $5, NULL);
    }
    | IF '(' expr ')' stmt ELSE stmt {
        $$ = createIf($3, $5, $7);
    }
    ;

/* WHILE LOOP */
while_stmt:
    WHILE '(' expr ')' stmt {
        $$ = createWhile($3, $5);
    }
    ;

/* FOR LOOP */
for_stmt:
    FOR '(' for_init ';' for_cond ';' for_update ')' stmt {
        $$ = createFor($3, $5, $7, $9);
    }
    ;

for_init:
    /* empty */ {$$ = NULL;}
    | ID '=' expr {
        $$ = createAssign($1, $3);
        free($1);
    }
    | ID '[' expr ']' '=' expr {
        $$ = createArrayAssign($1, $3, $6);
        free($1);
    }
    ;

for_cond:
    /* empty */ {$$ = NULL;}
    | expr {$$ = $1;}
    ;

for_update:
    /* empty */ {$$ = NULL;}
    | ID '=' expr {
        $$ = createAssign($1, $3);
        free($1);
    }
    | ID '[' expr ']' '=' expr {
        $$ = createArrayAssign($1, $3, $6);
        free($1);
    }
    ;

/* SWITCH STATEMENT */
switch_stmt:
    SWITCH '(' expr ')' '{' case_list '}' {
        $$ = createSwitch($3, $6);
    }
    ;

/* CASE LIST - zero or more case/default clauses */
case_list:
    /* empty */ {
        $$ = NULL;
    }
    | case_list case_clause {
        /* Append the new clause to the END of the linked list */
        if ($1 == NULL) {
            $$ = $2;
        } else {
            ASTNode* tail = $1;
            while (tail->data.case_clause.next)
                tail = tail->data.case_clause.next;
            tail->data.case_clause.next = $2;
            $$ = $1;
        }
    }
    ;

/* CASE CLAUSE */
case_clause:
    CASE NUM ':' opt_stmt_list {
        $$ = createCase($2, 0, $4);   /* isDefault = 0 */
    }
    | DEFAULT ':' opt_stmt_list {
        $$ = createCase(0, 1, $3);   /* isDefault = 1 */
    }
    ;

/* OPTIONAL STATEMENT LIST (may be empty) */
opt_stmt_list:
    /* empty */ { $$ = NULL; }
    | stmt_list { $$ = $1; }
    ;

/* BREAK STATEMENT */
break_stmt:
    BREAK ';' {
        $$ = createBreak();
    }
    ;

/* EXPRESSION RULES */
expr:
    NUM {
        $$ = createNum($1);
    }
    | CHAR_LIT {
        $$ = makeCharLit($1);
    }
    | TRUE_LIT {
        $$ = makeBoolLit(1);
    }
    | FALSE_LIT {
        $$ = makeBoolLit(0);
    }
    | STRING_LITERAL {
        $$ = makeStringLit($1);
    }
    | FLOAT_LIT {
        $$ = makeFloatLit($1);
    }
    | ID {
        $$ = createVar($1);
        free($1);
    }
    | ID '[' expr ']' {
        $$ = createArrayAccess($1, $3);
        free($1);
    }
    | func_call {
        $$ = $1;
    }
    | expr '+' expr {
        $$ = createBinOp('+', $1, $3);
    }
    | expr '-' expr {
        $$ = createBinOp('-', $1, $3);
    }
    | expr '*' expr {
        $$ = createBinOp('*', $1, $3);
    }
    | expr '/' expr {
        $$ = createBinOp('/', $1, $3);
    }
    | expr '<' expr {
        $$ = createBinOp('<', $1, $3);
    }
    | expr '>' expr {
        $$ = createBinOp('>', $1, $3);
    }
    | expr LE expr {
        $$ = createBinOp('l', $1, $3);  /* 'l' for <= */
    }
    | expr GE expr {
        $$ = createBinOp('g', $1, $3);  /* 'g' for >= */
    }
    | expr EQ expr {
        $$ = createBinOp('e', $1, $3);  /* 'e' for == */
    }
    | expr NE expr {
        $$ = createBinOp('n', $1, $3);  /* 'n' for != */
    }
    | expr AND_KW expr {
        $$ = createBinOp('&', $1, $3);  /* '&' for and */
    }
    | expr OR_KW expr {
        $$ = createBinOp('|', $1, $3);  /* '|' for or */
    }
    | NOT_KW expr %prec NOT_KW {
        $$ = createBinOp('!', $2, NULL); /* '!' for not (unary) */
    }
    | '-' expr %prec UMINUS {
        $$ = createBinOp('u', $2, NULL);  /* 'u' for unary minus */
    }
    | '(' expr ')' {
        $$ = $2;
    }
    ;

/* FUNCTION CALL */
func_call:
    ID '(' args ')' {
        $$ = createFuncCall($1, $3);
        free($1);
    }
    | ID '(' ')' {
        $$ = createFuncCall($1, NULL);
        free($1);
    }
    ;

/* ARGUMENTS */
args:
    arg_list { $$ = $1; }
    ;

arg_list:
    expr {
        $$ = $1;  /* Single argument becomes the arg node */
    }
    | arg_list ',' expr {
        $$ = createArgList($1, $3);
    }
    ;

/* PRINT STATEMENT */
print_stmt:
    PRINT '(' expr ')' ';' {
        $$ = createPrint($3);
    }
    ;

%%

/* ERROR HANDLING */
void yyerror(const char* s) {
    fprintf(stderr, "Syntax Error at line %d: %s\n", yylineno, s);
}
