/* SEMANTIC ANALYZER IMPLEMENTATION
 * Checks the AST for semantic errors before code generation
 * This ensures the program makes sense semantically, even if syntactically correct
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "semantic.h"
#include "symtab.h"

/* Track number of semantic errors found */
int semanticErrors = 0;

/* Initialize semantic analyzer */
void initSemantic() {
    semanticErrors = 0;
    printf("SEMANTIC ANALYZER: Initialized\n\n");
}

/* Report a semantic error */
void reportSemanticError(const char* msg) {
    printf("✗ SEMANTIC ERROR: %s\n", msg);
    semanticErrors++;
}

/* Analyze an expression for semantic correctness */
void analyzeExpr(ASTNode* node) {
    if (!node) return;

    switch(node->type) {
        case NODE_NUM:
            /* Numbers are always valid */
            break;

        case NODE_VAR: {
            /* Check if variable has been declared */
            if (!isVarDeclared(node->data.decl.name)) {
                char errorMsg[256];
                sprintf(errorMsg, "Variable '%s' used before declaration", node->data.decl.name);
                reportSemanticError(errorMsg);
            } else {
                printf("  ✓ Variable '%s' is declared\n", node->data.decl.name);
            }
            break;
        }

        case NODE_BINOP:
            /* Recursively analyze both operands */
            analyzeExpr(node->data.binop.left);
            analyzeExpr(node->data.binop.right);
            break;

        default:
            break;
    }
}

/* Analyze a statement for semantic correctness */
void analyzeStmt(ASTNode* node) {
    if (!node) return;

    switch(node->type) {
        case NODE_DECL: {
            /* Check if variable is already declared */
            if (isVarDeclared(node->data.decl.name)) {
                char errorMsg[256];
                sprintf(errorMsg, "Variable '%s' already declared", node->data.decl.name);
                reportSemanticError(errorMsg);
            } else {
                /* Add variable to symbol table */
                int offset = addVar(node->data.decl.name, node->data.decl.type);
                if (offset != -1) {
                    printf("  ✓ Variable '%s' declared successfully\n", node->data.decl.name);
                }
            }
            break;
        }

        case NODE_ASSIGN: {
            /* Check if variable being assigned to has been declared */
            if (!isVarDeclared(node->data.assign.var)) {
                char errorMsg[256];
                sprintf(errorMsg, "Cannot assign to undeclared variable '%s'", node->data.assign.var);
                reportSemanticError(errorMsg);
            } else {
                printf("  ✓ Assignment to declared variable '%s'\n", node->data.assign.var);
            }
            /* Check the expression being assigned */
            analyzeExpr(node->data.assign.value);
            break;
        }

        case NODE_PRINT:
            /* Check the expression being printed */
            printf("  Checking print statement expression:\n");
            analyzeExpr(node->data.expr);
            break;

        case NODE_STMT_LIST:
            /* Recursively analyze all statements */
            analyzeStmt(node->data.stmtlist.stmt);
            analyzeStmt(node->data.stmtlist.next);
            break;

        default:
            break;
    }
}

/* Analyze the entire program */
int analyzeProgram(ASTNode* root) {
    printf("Starting semantic analysis...\n");
    printf("───────────────────────────────\n");

    /* Initialize symbol table for semantic checking */
    initSymTab();

    /* Analyze all statements */
    analyzeStmt(root);

    printf("───────────────────────────────\n");

    /* Report results */
    if (semanticErrors == 0) {
        printf("✓ Semantic analysis passed - no errors found!\n");
        return 1;  /* Success */
    } else {
        printf("✗ Semantic analysis failed with %d error(s)\n", semanticErrors);
        return 0;  /* Failure */
    }
}
