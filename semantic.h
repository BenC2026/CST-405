#ifndef SEMANTIC_H
#define SEMANTIC_H

#include "ast.h"

/* SEMANTIC ANALYSIS - WITH FUNCTION SUPPORT
 * This phase checks the semantic correctness of the program
 * Ensures variables are declared before use, no redeclarations, etc.
 * Now supports functions, scopes, parameters, and control flow
 */

/* SEMANTIC ERROR TRACKING */
typedef struct {
    int errorCount;           /* Number of semantic errors found */
    int warningCount;         /* Number of warnings issued */
} SemanticInfo;

/* SEMANTIC ANALYSIS FUNCTIONS */
void initSemantic();                     /* Initialize semantic analyzer */
int performSemanticAnalysis(ASTNode* root);  /* Run semantic checks on AST, returns 0 if successful */
void printSemanticSummary();             /* Print summary of semantic analysis results */

#endif
