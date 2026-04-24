/* SEMANTIC ANALYSIS IMPLEMENTATION - WITH FUNCTION SUPPORT
 * Performs semantic checks on the Abstract Syntax Tree
 * Now supports functions, scopes, parameters, control flow
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "semantic.h"
#include "symtab.h"

#define MAX_FUNCTIONS 100
#define MAX_PARAMS 20
#define MAX_SCOPE_DEPTH 10

/* Function symbol table entry */
typedef struct {
    char* name;
    int paramCount;
    char* params[MAX_PARAMS];
    int isDefined;
} FunctionSymbol;

/* Scope for variables */
typedef struct {
    char* names[MAX_VARS];
    int count;
} Scope;

/* Global semantic information */
static SemanticInfo semInfo;
static FunctionSymbol functions[MAX_FUNCTIONS];
static int functionCount = 0;
static Scope scopes[MAX_SCOPE_DEPTH];
static int scopeDepth = 0;
static char* currentFunction = NULL;
static int inFunction = 0;
static int breakDepth = 0;  /* Track nested loops for break/continue validity */

/* Initialize semantic analyzer */
void initSemantic() {
    semInfo.errorCount = 0;
    semInfo.warningCount = 0;
    functionCount = 0;
    scopeDepth = 0;
    currentFunction = NULL;
    inFunction = 0;
    breakDepth = 0;

    /* Add built-in functions */
    functions[functionCount].name = strdup("print");
    functions[functionCount].paramCount = 1;
    functions[functionCount].isDefined = 1;  /* Built-in */
    functionCount++;

    functions[functionCount].name = strdup("output_string");
    functions[functionCount].paramCount = 1;
    functions[functionCount].isDefined = 1;  /* Built-in */
    functionCount++;

    functions[functionCount].name = strdup("string_equal");
    functions[functionCount].paramCount = 2;
    functions[functionCount].isDefined = 1;  /* Built-in */
    functionCount++;

    functions[functionCount].name = strdup("string_concat");
    functions[functionCount].paramCount = 2;
    functions[functionCount].isDefined = 1;  /* Built-in */
    functionCount++;

    printf("SEMANTIC ANALYZER: Initialized with function support\n\n");
}

/* Scope management */
static void enterScope() {
    if (scopeDepth >= MAX_SCOPE_DEPTH) {
        fprintf(stderr, "SEMANTIC ERROR: Maximum scope depth exceeded\n");
        semInfo.errorCount++;
        return;
    }
    scopes[scopeDepth].count = 0;
    scopeDepth++;
}

static void exitScope() {
    if (scopeDepth > 0) {
        /* Free variable names in this scope */
        for (int i = 0; i < scopes[scopeDepth - 1].count; i++) {
            free(scopes[scopeDepth - 1].names[i]);
        }
        scopeDepth--;
    }
}

/* Print current semantic scopes for debugging */
static void printSemanticScopes() {
    printf("\n┌─────────────────────────────────────────────────────────┐\n");
    printf("│ SEMANTIC SCOPE STACK (Depth: %d)                        \n", scopeDepth);
    printf("├─────────────────────────────────────────────────────────┤\n");

    if (scopeDepth == 0) {
        printf("│ (no active scopes)                                      │\n");
    } else {
        for (int depth = 0; depth < scopeDepth; depth++) {
            if (depth == 0) {
                printf("│ Scope[%d] GLOBAL (%d variables)                        \n", depth, scopes[depth].count);
            } else {
                if (inFunction && depth == scopeDepth - 1) {
                    printf("│ Scope[%d] Function '%s' (%d variables)              \n",
                           depth, currentFunction ? currentFunction : "unknown", scopes[depth].count);
                } else {
                    printf("│ Scope[%d] LOCAL (%d variables)                         \n", depth, scopes[depth].count);
                }
            }

            if (scopes[depth].count > 0) {
                printf("│   Variables: ");
                for (int i = 0; i < scopes[depth].count; i++) {
                    printf("%s", scopes[depth].names[i]);
                    if (i < scopes[depth].count - 1) printf(", ");
                }
                printf("\n");
            } else {
                printf("│   (empty)\n");
            }
        }
    }
    printf("└─────────────────────────────────────────────────────────┘\n\n");
}

/* Add variable to current scope */
static int addVarToScope(char* name) {
    if (scopeDepth == 0) {
        fprintf(stderr, "SEMANTIC ERROR: No scope to add variable to\n");
        return -1;
    }

    Scope* currentScope = &scopes[scopeDepth - 1];

    /* Check if already declared in current scope */
    for (int i = 0; i < currentScope->count; i++) {
        if (strcmp(currentScope->names[i], name) == 0) {
            return -1;  /* Already declared in this scope */
        }
    }

    /* Add to current scope */
    if (currentScope->count >= MAX_VARS) {
        fprintf(stderr, "SEMANTIC ERROR: Too many variables in scope\n");
        return -1;
    }

    currentScope->names[currentScope->count] = strdup(name);
    currentScope->count++;
    return 0;
}

/* Check if variable is declared in any visible scope */
static int isVarDeclaredInScope(char* name) {
    /* Search from innermost to outermost scope */
    for (int depth = scopeDepth - 1; depth >= 0; depth--) {
        for (int i = 0; i < scopes[depth].count; i++) {
            if (strcmp(scopes[depth].names[i], name) == 0) {
                return 1;
            }
        }
    }
    return 0;
}

/* Function management */
static FunctionSymbol* findFunction(char* name) {
    for (int i = 0; i < functionCount; i++) {
        if (strcmp(functions[i].name, name) == 0) {
            return &functions[i];
        }
    }
    return NULL;
}

static int addFunction(char* name, int paramCount, char** params) {
    if (functionCount >= MAX_FUNCTIONS) {
        fprintf(stderr, "SEMANTIC ERROR: Too many functions\n");
        return -1;
    }

    /* Check if function already exists */
    if (findFunction(name)) {
        fprintf(stderr, "  ✗ SEMANTIC ERROR: Function '%s' already defined\n", name);
        semInfo.errorCount++;
        return -1;
    }

    functions[functionCount].name = strdup(name);
    functions[functionCount].paramCount = paramCount;
    for (int i = 0; i < paramCount; i++) {
        functions[functionCount].params[i] = strdup(params[i]);
    }
    functions[functionCount].isDefined = 1;
    functionCount++;

    printf("  ✓ Function '%s' defined with %d parameter(s)\n", name, paramCount);
    return 0;
}

/* Count parameters in parameter list (recursive to handle nested PARAM_LIST) */
static int countParams(ASTNode* params, char** paramNames) {
    if (!params) return 0;

    if (params->type == NODE_PARAM) {
        paramNames[0] = params->data.param.name;
        return 1;
    } else if (params->type == NODE_PARAM_LIST) {
        int count = countParams(params->data.param_list.param, paramNames);
        count += countParams(params->data.param_list.next, paramNames + count);
        return count;
    }

    return 0;
}

/* Count arguments in argument list */
static int countArgs(ASTNode* args) {
    if (!args) return 0;

    if (args->type == NODE_ARG_LIST) {
        return countArgs(args->data.arg_list.expr) + countArgs(args->data.arg_list.next);
    }
    /* Single argument (any expression node) */
    return 1;
}

/* Map a type string (from the parser) to a VarType enum value */
static VarType typeStringToVarType(const char* typeStr) {
    if (!typeStr) return TYPE_UNKNOWN;
    if (strcmp(typeStr, "int")    == 0) return TYPE_INT;
    if (strcmp(typeStr, "string") == 0) return TYPE_STRING;
    if (strcmp(typeStr, "float")  == 0) return TYPE_FLOAT;
    if (strcmp(typeStr, "char")    == 0) return TYPE_CHAR;
    if (strcmp(typeStr, "boolean") == 0) return TYPE_BOOL;
    if (strcmp(typeStr, "void")    == 0) return TYPE_VOID;
    return TYPE_UNKNOWN;
}

/* Forward declarations */
static void checkExpr(ASTNode* node);
static void checkStmt(ASTNode* node);
static void checkStmtList(ASTNode* node);

/* Check expression for semantic correctness */
static void checkExpr(ASTNode* node) {
    if (!node) return;

    switch(node->type) {
        case NODE_NUM:
            /* Literal numbers are always valid */
            break;

        case NODE_VAR:
            if (!isVarDeclaredInScope(node->data.decl.name)) {
                fprintf(stderr, "  ✗ SEMANTIC ERROR (line %d): Variable '%s' used before declaration\n",
                        node->lineno, node->data.decl.name);
                semInfo.errorCount++;
            }
            break;

        case NODE_ARRAY_ACCESS:
            if (!isVarDeclaredInScope(node->data.array_access.name)) {
                fprintf(stderr, "  ✗ SEMANTIC ERROR (line %d): Array '%s' used before declaration\n",
                        node->lineno, node->data.array_access.name);
                semInfo.errorCount++;
            }
            checkExpr(node->data.array_access.index);
            break;

        case NODE_BINOP:
            checkExpr(node->data.binop.left);
            if (node->data.binop.right) {  /* Unary minus has no right operand */
                checkExpr(node->data.binop.right);
            }
            break;

        case NODE_FUNC_CALL: {
            FunctionSymbol* func = findFunction(node->data.func_call.name);
            if (!func) {
                fprintf(stderr, "  ✗ SEMANTIC ERROR (line %d): Function '%s' not declared\n",
                        node->lineno, node->data.func_call.name);
                semInfo.errorCount++;
            } else {
                int argCount = countArgs(node->data.func_call.args);
                if (argCount != func->paramCount) {
                    fprintf(stderr, "  ✗ SEMANTIC ERROR (line %d): Function '%s' expects %d arguments, got %d\n",
                            node->lineno, node->data.func_call.name, func->paramCount, argCount);
                    semInfo.errorCount++;
                } else {
                    printf("  ✓ Function call '%s' has correct argument count\n",
                           node->data.func_call.name);
                }

                /* Check argument expressions */
                ASTNode* arg = node->data.func_call.args;
                while (arg) {
                    if (arg->type == NODE_ARG_LIST) {
                        checkExpr(arg->data.arg_list.expr);
                        arg = arg->data.arg_list.next;
                    } else {
                        checkExpr(arg);
                        break;
                    }
                }
            }
            break;
        }

        default:
            break;
    }
}

/* Check statement */
static void checkStmt(ASTNode* node) {
    if (!node) return;

    switch(node->type) {
        case NODE_DECL: {
            VarType vt = typeStringToVarType(node->data.decl.type);
            if (vt == TYPE_UNKNOWN) {
                fprintf(stderr, "  ✗ SEMANTIC ERROR (line %d): Unknown type '%s' for variable '%s'\n",
                        node->lineno, node->data.decl.type ? node->data.decl.type : "(null)",
                        node->data.decl.name);
                semInfo.errorCount++;
            }
            if (addVarToScope(node->data.decl.name) == -1) {
                fprintf(stderr, "  ✗ SEMANTIC ERROR (line %d): Variable '%s' already declared in this scope\n",
                        node->lineno, node->data.decl.name);
                semInfo.errorCount++;
            } else {
                printf("  ✓ Variable '%s' declared as %s (line %d)\n",
                       node->data.decl.name, node->data.decl.type, node->lineno);
            }
            break;
        }

        case NODE_ARRAY_DECL:
            if (addVarToScope(node->data.array_decl.name) == -1) {
                fprintf(stderr, "  ✗ SEMANTIC ERROR (line %d): Array '%s' already declared in this scope\n",
                        node->lineno, node->data.array_decl.name);
                semInfo.errorCount++;
            } else {
                printf("  ✓ Array '%s[%d]' declared (line %d)\n", node->data.array_decl.name, node->data.array_decl.size, node->lineno);
            }
            break;

        case NODE_ARRAY_ASSIGN:
            if (!isVarDeclaredInScope(node->data.array_assign.name)) {
                fprintf(stderr, "  ✗ SEMANTIC ERROR (line %d): Assignment to undeclared array '%s'\n",
                        node->lineno, node->data.array_assign.name);
                semInfo.errorCount++;
            }
            checkExpr(node->data.array_assign.index);
            checkExpr(node->data.array_assign.value);
            break;

        case NODE_ASSIGN:
            if (!isVarDeclaredInScope(node->data.assign.var)) {
                fprintf(stderr, "  ✗ SEMANTIC ERROR (line %d): Assignment to undeclared variable '%s'\n",
                        node->lineno, node->data.assign.var);
                semInfo.errorCount++;
            } else {
                printf("  ✓ Assignment to '%s' is valid (line %d)\n", node->data.assign.var, node->lineno);
            }
            checkExpr(node->data.assign.value);
            break;

        case NODE_PRINT:
            checkExpr(node->data.expr);
            printf("  ✓ Print statement is valid\n");
            break;

        case NODE_RETURN:
            if (!inFunction) {
                fprintf(stderr, "  ✗ SEMANTIC ERROR: Return statement outside function\n");
                semInfo.errorCount++;
            } else {
                printf("  ✓ Return statement in function '%s'\n", currentFunction);
                checkExpr(node->data.ret.expr);
            }
            break;

        case NODE_IF:
            printf("  ✓ Checking if statement\n");
            checkExpr(node->data.if_stmt.condition);
            checkStmt(node->data.if_stmt.then_stmt);
            if (node->data.if_stmt.else_stmt) {
                checkStmt(node->data.if_stmt.else_stmt);
            }
            break;

        case NODE_WHILE:
            printf("  ✓ Checking while loop at line %d\n", node->lineno);

            /* Check condition exists and is valid */
            if (!node->data.while_stmt.condition) {
                fprintf(stderr, "Error: While loop missing condition\n");
            } else {
                checkExpr(node->data.while_stmt.condition);

                /* Warn about constant conditions */
                if (node->data.while_stmt.condition->type == NODE_NUM) {
                    int value = node->data.while_stmt.condition->data.num;
                    if (value == 0) {
                        fprintf(stderr, "Warning: Dead loop (always false)\n");
                    } else {
                        fprintf(stderr, "Warning: Infinite loop (always true)\n");
                    }
                }
            }

            /* Check body */
            breakDepth++;
            checkStmt(node->data.while_stmt.body);
            breakDepth--;
            break;

        case NODE_FOR:
            printf("  ✓ Checking for loop at line %d\n", node->lineno);

            /* Check initialization */
            if (node->data.for_stmt.init) {
                checkStmt(node->data.for_stmt.init);
            }

            /* Check condition */
            if (node->data.for_stmt.condition) {
                checkExpr(node->data.for_stmt.condition);

                /* Warn about constant conditions */
                if (node->data.for_stmt.condition->type == NODE_NUM) {
                    int value = node->data.for_stmt.condition->data.num;
                    if (value == 0) {
                        fprintf(stderr, "Warning: Dead loop (always false)\n");
                    } else {
                        fprintf(stderr, "Warning: Infinite loop (always true)\n");
                    }
                }
            }

            /* Check update */
            if (node->data.for_stmt.update) {
                checkStmt(node->data.for_stmt.update);
            }

            /* Check body */
            breakDepth++;
            checkStmt(node->data.for_stmt.body);
            breakDepth--;
            break;

        case NODE_BLOCK:
            enterScope();
            checkStmtList(node->data.block.stmt_list);
            exitScope();
            break;

        case NODE_BREAK:
            if (breakDepth == 0) {
                fprintf(stderr, "\n╔════════════════════════════════════════╗\n");
                fprintf(stderr, "║ SEMANTIC ERROR - break outside context ║\n");
                fprintf(stderr, "╚════════════════════════════════════════╝\n");
                fprintf(stderr, "  📍 Location: Line %d\n", node->lineno);
                fprintf(stderr, "  ❌ 'break' must be inside a switch, for, or while\n\n");
                semInfo.errorCount++;
            } else {
                printf("  ✓ break at line %d\n", node->lineno);
            }
            break;

        case NODE_SWITCH: {
            printf("  ✓ Checking switch statement at line %d\n", node->lineno);

            /* 1. Check the controlling expression */
            checkExpr(node->data.switch_stmt.expr);

            /* 2. Walk case clauses */
            int seenValues[512];
            int seenCount = 0;
            int hasDefault = 0;
            ASTNode* c = node->data.switch_stmt.cases;

            breakDepth++;   /* break is now valid inside case bodies */
            while (c) {
                if (c->data.case_clause.isDefault) {
                    if (hasDefault) {
                        fprintf(stderr, "  ✗ SEMANTIC ERROR (line %d): duplicate default clause in switch\n",
                                c->lineno);
                        semInfo.errorCount++;
                    }
                    hasDefault = 1;
                } else {
                    int val = c->data.case_clause.value;
                    int duplicate = 0;
                    for (int i = 0; i < seenCount; i++) {
                        if (seenValues[i] == val) { duplicate = 1; break; }
                    }
                    if (duplicate) {
                        fprintf(stderr, "  ✗ SEMANTIC ERROR (line %d): duplicate case value %d in switch\n",
                                c->lineno, val);
                        semInfo.errorCount++;
                    } else {
                        seenValues[seenCount++] = val;
                    }
                }
                checkStmt(c->data.case_clause.body);
                c = c->data.case_clause.next;
            }
            breakDepth--;

            /* 3. Warn if no default clause */
            if (!hasDefault) {
                fprintf(stderr, "  ⚠  Warning: switch at line %d has no default clause\n\n",
                        node->lineno);
                semInfo.warningCount++;
            }
            break;
        }

        case NODE_FUNC_CALL:
            /* Function call as statement */
            checkExpr(node);
            break;

        case NODE_STMT_LIST:
            checkStmtList(node);
            break;

        default:
            break;
    }
}

/* Check statement list */
static void checkStmtList(ASTNode* node) {
    if (!node) return;

    if (node->type == NODE_STMT_LIST) {
        checkStmt(node->data.stmtlist.stmt);
        checkStmtList(node->data.stmtlist.next);
    } else {
        checkStmt(node);
    }
}

/* Check function definition */
static void checkFuncDef(ASTNode* node) {
    if (!node || node->type != NODE_FUNC_DEF) return;

    /* Function should already be registered from first pass */
    /* Enter function scope */
    currentFunction = node->data.func_def.name;
    inFunction = 1;
    enterScope();
    printf("  Entered function '%s' scope\n", currentFunction);
    printSemanticScopes();

    /* Add parameters as variables in function scope */
    char* paramNames[MAX_PARAMS];
    int paramCount = countParams(node->data.func_def.params, paramNames);
    for (int i = 0; i < paramCount; i++) {
        addVarToScope(paramNames[i]);
        printf("  ✓ Parameter '%s' added to function scope\n", paramNames[i]);
    }

    if (paramCount > 0) {
        printSemanticScopes();
    }

    /* Check function body */
    checkStmt(node->data.func_def.body);

    /* Exit function scope */
    printf("  Exiting function '%s' scope\n", currentFunction);
    exitScope();
    printSemanticScopes();
    inFunction = 0;
    currentFunction = NULL;
}

/* Register function (first pass) */
static void registerFunction(ASTNode* node) {
    if (!node || node->type != NODE_FUNC_DEF) return;

    char* paramNames[MAX_PARAMS];
    int paramCount = countParams(node->data.func_def.params, paramNames);
    addFunction(node->data.func_def.name, paramCount, paramNames);
}

/* Helper to recursively register all functions in AST */
static void registerFunctions(ASTNode* node) {
    if (!node) return;
    if (node->type == NODE_STMT_LIST) {
        registerFunctions(node->data.stmtlist.stmt);
        registerFunctions(node->data.stmtlist.next);
    } else if (node->type == NODE_FUNC_DEF) {
        registerFunction(node);
    }
}

/* Helper to recursively check all functions/statements in AST */
static void checkFunctions(ASTNode* node);  /* Forward declaration */

static void checkFunctions(ASTNode* node) {
    if (!node) return;
    if (node->type == NODE_STMT_LIST) {
        checkFunctions(node->data.stmtlist.stmt);
        checkFunctions(node->data.stmtlist.next);
    } else if (node->type == NODE_FUNC_DEF) {
        printf("─── Checking function: %s ───\n", node->data.func_def.name);
        checkFuncDef(node);
        printf("\n");
    } else if (node->type == NODE_DECL) {
        extern SemanticInfo semInfo;  /* Access global */
        if (addVarToScope(node->data.decl.name) == -1) {
            fprintf(stderr, "  ✗ SEMANTIC ERROR: Global variable '%s' already declared\n",
                    node->data.decl.name);
            semInfo.errorCount++;
        } else {
            printf("  ✓ Global variable '%s' declared\n", node->data.decl.name);
        }
    } else {
        checkStmt(node);
    }
}

/* Perform complete semantic analysis */
int performSemanticAnalysis(ASTNode* root) {
    if (!root) {
        fprintf(stderr, "SEMANTIC ERROR: No AST to analyze\n");
        return -1;
    }

    printf("Running semantic analysis with function support...\n\n");

    /* Enter global scope */
    enterScope();
    printf("Entered global scope\n");
    printSemanticScopes();

    /* FIRST PASS: Register all function signatures */
    printf("Pass 1: Registering all functions\n");
    printf("───────────────────────────────\n");
    registerFunctions(root);
    printf("\n");

    /* SECOND PASS: Check function bodies and other statements */
    printf("Pass 2: Checking function bodies\n");
    printf("─────────────────────────────────\n");
    checkFunctions(root);

    /* Exit global scope */
    exitScope();

    return semInfo.errorCount > 0 ? -1 : 0;
}

/* Print semantic analysis summary */
void printSemanticSummary() {
    printf("═══════════════════════════════════════════\n");
    printf("SEMANTIC ANALYSIS SUMMARY\n");
    printf("═══════════════════════════════════════════\n");
    printf("Functions defined:  %d\n", functionCount);
    printf("Errors found:       %d\n", semInfo.errorCount);
    printf("Warnings found:     %d\n", semInfo.warningCount);
    printf("\n");

    if (semInfo.errorCount == 0) {
        printf("✓ Semantic analysis passed - program is semantically correct!\n");
    } else {
        printf("✗ Semantic analysis failed - fix errors before proceeding\n");
    }
    printf("═══════════════════════════════════════════\n\n");
}
