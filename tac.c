#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "tac.h"

typedef struct {
    int eliminated_temps;
    int constant_folded;
    int dead_code_removed;
} OptimizationStats;

OptimizationStats opt_stats = {0};

TACList tacList;
TACList optimizedList;
TempAllocator tempAlloc;

void initTAC() {
    tacList.head = NULL;
    tacList.tail = NULL;
    tacList.tempCount = 0;
    tacList.labelCount = 0;
    optimizedList.head = NULL;
    optimizedList.tail = NULL;

    /* Initialize temporary allocator */
    for (int i = 0; i < MAX_TEMPS; i++) {
        tempAlloc.allocated[i] = 0;
    }
    tempAlloc.maxUsed = 0;
    tempAlloc.freeCount = 0;
    printf("TAC: Temporary allocator initialized\n");
}

char* newTemp() {
    char* temp = malloc(10);
    sprintf(temp, "t%d", tacList.tempCount++);
    return temp;
}

char* allocTemp() {
    int tempNum;

    if (tempAlloc.freeCount > 0) {
        tempNum = tempAlloc.freeList[--tempAlloc.freeCount];
        printf("    [TAC ALLOC] Reusing temporary t%d\n", tempNum);
    } else {
        tempNum = tempAlloc.maxUsed++;
        if (tempNum >= MAX_TEMPS) {
            fprintf(stderr, "ERROR: Exceeded maximum temporaries\n");
            exit(1);
        }
        printf("    [TAC ALLOC] Allocating new temporary t%d\n", tempNum);
    }

    tempAlloc.allocated[tempNum] = 1;
    char* temp = malloc(10);
    sprintf(temp, "t%d", tempNum);
    return temp;
}

void freeTemp(char* temp) {
    if (!temp || temp[0] != 't') return;

    int tempNum = atoi(temp + 1);
    if (!tempAlloc.allocated[tempNum]) return;

    tempAlloc.allocated[tempNum] = 0;
    if (tempAlloc.freeCount < MAX_TEMPS) {
        tempAlloc.freeList[tempAlloc.freeCount++] = tempNum;
        printf("    [TAC FREE] Released temporary %s\n", temp);
    }
}

char* newLabel() {
    char* label = malloc(10);
    sprintf(label, "L%d", tacList.labelCount++);
    return label;
}

void printTempAllocatorState() {
    printf("\n┌──────────────────────────────────────────────────────────┐\n");
    printf("│ TEMPORARY ALLOCATOR STATISTICS                           │\n");
    printf("├──────────────────────────────────────────────────────────┤\n");
    printf("│ Total temporaries used:      %3d                         │\n", tempAlloc.maxUsed);
    printf("│ Currently allocated:         %3d                         │\n",
           tempAlloc.maxUsed - tempAlloc.freeCount);
    printf("│ Available for reuse:         %3d                         │\n", tempAlloc.freeCount);
    printf("└──────────────────────────────────────────────────────────┘\n\n");
}

TACInstr* createTAC(TACOp op, char* arg1, char* arg2, char* result, char* type) {
    TACInstr* instr = malloc(sizeof(TACInstr));
    instr->op = op;
    instr->arg1 = arg1 ? strdup(arg1) : NULL;
    instr->arg2 = arg2 ? strdup(arg2) : NULL;
    instr->result = result ? strdup(result) : NULL;
    instr->type = type ? strdup(type) : NULL;
    instr->next = NULL;
    return instr;
}

void appendTAC(TACInstr* instr) {
    if (!tacList.head) {
        tacList.head = tacList.tail = instr;
    } else {
        tacList.tail->next = instr;
        tacList.tail = instr;
    }
}

void appendOptimizedTAC(TACInstr* instr) {
    if (!optimizedList.head) {
        optimizedList.head = optimizedList.tail = instr;
    } else {
        optimizedList.tail->next = instr;
        optimizedList.tail = instr;
    }
}

/* Forward declaration */
static void generateTACStmt(ASTNode* node);

/* Recursively emit TAC_ARG for all arguments in nested ARG_LIST */
static int generateTACArgs(ASTNode* arg) {
    if (!arg) return 0;
    if (arg->type == NODE_ARG_LIST) {
        int count = generateTACArgs(arg->data.arg_list.expr);
        count += generateTACArgs(arg->data.arg_list.next);
        return count;
    }
    /* Single argument expression */
    char* argTemp = generateTACExpr(arg);
    appendTAC(createTAC(TAC_ARG, argTemp, NULL, NULL, NULL));
    return 1;
}

/* Generate TAC for expression - returns the temp/var holding result */
char* generateTACExpr(ASTNode* node) {
    if (!node) return NULL;

    switch(node->type) {
        case NODE_NUM: {
            char* temp = malloc(20);
            sprintf(temp, "%d", node->data.num);
            return temp;
        }

        case NODE_VAR:
            return strdup(node->data.decl.name);

        case NODE_BINOP: {
            char* left = generateTACExpr(node->data.binop.left);
            char* right = NULL;

            /* Unary minus has no right operand */
            if (node->data.binop.op != 'u') {
                right = generateTACExpr(node->data.binop.right);
            }

            char* temp = allocTemp();
            char* resultType = "int";

            /* Select operation type */
            TACOp op;
            switch(node->data.binop.op) {
                case '+': op = TAC_ADD; break;
                case '-': op = TAC_SUB; break;
                case '*': op = TAC_MUL; break;
                case '/': op = TAC_DIV; break;
                case '<': op = TAC_LT; break;
                case '>': op = TAC_GT; break;
                case 'l': op = TAC_LE; break;  /* <= */
                case 'g': op = TAC_GE; break;  /* >= */
                case 'e': op = TAC_EQ; break;  /* == */
                case 'n': op = TAC_NE; break;  /* != */
                case 'u': op = TAC_NEG; break; /* unary - */
                default:  op = TAC_ADD; break;
            }

            appendTAC(createTAC(op, left, right, temp, resultType));

            freeTemp(left);
            if (right) freeTemp(right);

            return temp;
        }

        case NODE_FUNC_CALL: {
            /* Generate TAC for arguments (recursive to handle nested ARG_LIST) */
            int argCount = generateTACArgs(node->data.func_call.args);

            /* Generate call instruction */
            char* result = allocTemp();
            char argCountStr[10];
            sprintf(argCountStr, "%d", argCount);
            appendTAC(createTAC(TAC_CALL, node->data.func_call.name, argCountStr, result, "int"));

            return result;
        }

        case NODE_ARRAY_ACCESS: {
            char* index = generateTACExpr(node->data.array_access.index);
            char* temp = allocTemp();

            appendTAC(createTAC(TAC_ARRAY_ACCESS, node->data.array_access.name, index,
                               temp, "int"));

            freeTemp(index);
            return temp;
        }

        default:
            return NULL;
    }
}

/* Generate TAC for statement list */
static void generateTACStmtList(ASTNode* node) {
    if (!node) return;

    if (node->type == NODE_STMT_LIST) {
        generateTACStmt(node->data.stmtlist.stmt);
        generateTACStmtList(node->data.stmtlist.next);
    } else {
        generateTACStmt(node);
    }
}

/* Generate TAC for a statement */
static void generateTACStmt(ASTNode* node) {
    if (!node) return;

    switch(node->type) {
        case NODE_DECL:
            appendTAC(createTAC(TAC_DECL, NULL, NULL, node->data.decl.name, node->data.decl.type));
            break;

        case NODE_ASSIGN: {
            char* expr = generateTACExpr(node->data.assign.value);
            appendTAC(createTAC(TAC_ASSIGN, expr, NULL, node->data.assign.var, "int"));
            freeTemp(expr);
            break;
        }

        case NODE_PRINT: {
            char* expr = generateTACExpr(node->data.expr);
            appendTAC(createTAC(TAC_PRINT, expr, NULL, NULL, NULL));
            freeTemp(expr);
            break;
        }

        case NODE_RETURN: {
            if (node->data.ret.expr) {
                char* expr = generateTACExpr(node->data.ret.expr);
                appendTAC(createTAC(TAC_RETURN, expr, NULL, NULL, NULL));
                freeTemp(expr);
            } else {
                appendTAC(createTAC(TAC_RETURN, NULL, NULL, NULL, NULL));
            }
            break;
        }

        case NODE_IF: {
            /* Generate labels */
            char* labelElse = newLabel();
            char* labelEnd = newLabel();

            /* Evaluate condition */
            char* cond = generateTACExpr(node->data.if_stmt.condition);

            /* If condition is false, jump to else (or end) */
            if (node->data.if_stmt.else_stmt) {
                appendTAC(createTAC(TAC_IF_FALSE, cond, labelElse, NULL, NULL));
            } else {
                appendTAC(createTAC(TAC_IF_FALSE, cond, labelEnd, NULL, NULL));
            }
            freeTemp(cond);

            /* Generate then branch */
            generateTACStmt(node->data.if_stmt.then_stmt);

            if (node->data.if_stmt.else_stmt) {
                /* Jump over else branch */
                appendTAC(createTAC(TAC_GOTO, labelEnd, NULL, NULL, NULL));

                /* Else label */
                appendTAC(createTAC(TAC_LABEL, NULL, NULL, labelElse, NULL));

                /* Generate else branch */
                generateTACStmt(node->data.if_stmt.else_stmt);
            }

            /* End label */
            appendTAC(createTAC(TAC_LABEL, NULL, NULL, labelEnd, NULL));
            break;
        }

        case NODE_WHILE: {
            /* Generate labels */
            char* labelStart = newLabel();
            char* labelEnd = newLabel();

            /* Start label */
            appendTAC(createTAC(TAC_LABEL, NULL, NULL, labelStart, NULL));

            /* Evaluate condition */
            char* cond = generateTACExpr(node->data.while_stmt.condition);

            /* If condition is false, jump to end */
            appendTAC(createTAC(TAC_IF_FALSE, cond, labelEnd, NULL, NULL));
            freeTemp(cond);

            /* Generate body */
            generateTACStmt(node->data.while_stmt.body);

            /* Jump back to start */
            appendTAC(createTAC(TAC_GOTO, labelStart, NULL, NULL, NULL));

            /* End label */
            appendTAC(createTAC(TAC_LABEL, NULL, NULL, labelEnd, NULL));
            break;
        }

        case NODE_FOR:
            /* Generate labels */
            char* labelStart = newLabel();
            char* labelEnd = newLabel();

            /* Initialization */
            if (node->data.for_stmt.init) {
                generateTACStmt(node->data.for_stmt.init);
            }

            /* Start label */
            appendTAC(createTAC(TAC_LABEL, NULL, NULL, labelStart, NULL));

            /* Condition */
            if (node->data.for_stmt.condition) {
                char* cond = generateTACExpr(node->data.for_stmt.condition);
                appendTAC(createTAC(TAC_IF_FALSE, cond, labelEnd, NULL, NULL));
                freeTemp(cond);
            }

            /* Body */
            generateTACStmt(node->data.for_stmt.body);

            /* Update */
            if (node->data.for_stmt.update) {
                generateTACStmt(node->data.for_stmt.update);
            }

            /* Jump back to start */
            appendTAC(createTAC(TAC_GOTO, labelStart, NULL, NULL, NULL));

            /* End label */
            appendTAC(createTAC(TAC_LABEL, NULL, NULL, labelEnd, NULL));
            break;

        case NODE_BLOCK:
            generateTACStmtList(node->data.block.stmt_list);
            break;

        case NODE_FUNC_CALL:
            /* Function call as statement (result discarded) */
            generateTACExpr(node);
            break;

        case NODE_STMT_LIST:
            generateTACStmtList(node);
            break;

        case NODE_ARRAY_DECL: {
            char sizeStr[20];
            sprintf(sizeStr, "%d", node->data.array_decl.size);
            appendTAC(createTAC(TAC_ARRAY_DECL, sizeStr,
                               NULL, node->data.array_decl.name,
                               node->data.array_decl.type));
            break;
        }

        case NODE_ARRAY_ASSIGN: {
            char* index = generateTACExpr(node->data.array_assign.index);
            char* value = generateTACExpr(node->data.array_assign.value);
            appendTAC(createTAC(TAC_ARRAY_ASSIGN, index, value,
                               node->data.array_assign.name, NULL));
            freeTemp(index);
            freeTemp(value);
            break;
        }

        default:
            break;
    }
}

/* Recursively emit TAC_PARAM for all parameters */
static void generateTACParams(ASTNode* param) {
    if (!param) return;
    if (param->type == NODE_PARAM) {
        appendTAC(createTAC(TAC_PARAM, NULL, NULL, param->data.param.name, "int"));
    } else if (param->type == NODE_PARAM_LIST) {
        generateTACParams(param->data.param_list.param);
        generateTACParams(param->data.param_list.next);
    }
}

/* Generate TAC for function definition */
static void generateTACFuncDef(ASTNode* node) {
    if (!node || node->type != NODE_FUNC_DEF) return;

    /* Function begin marker */
    appendTAC(createTAC(TAC_FUNC_BEGIN, NULL, NULL, node->data.func_def.name, "int"));

    /* Generate parameter declarations (recursive helper for nested PARAM_LIST) */
    generateTACParams(node->data.func_def.params);

    /* Generate function body */
    generateTACStmt(node->data.func_def.body);

    /* Function end marker */
    appendTAC(createTAC(TAC_FUNC_END, NULL, NULL, node->data.func_def.name, "int"));
}

/* Recursive helper to generate TAC for declarations/functions list */
static void generateTACList(ASTNode* node) {
    if (!node) return;

    if (node->type == NODE_STMT_LIST) {
        /* The stmt field might itself be a STMT_LIST (left-recursive grammar) */
        generateTACList(node->data.stmtlist.stmt);
        /* The next field contains the last item */
        generateTACList(node->data.stmtlist.next);
    } else if (node->type == NODE_FUNC_DEF) {
        generateTACFuncDef(node);
    } else {
        generateTACStmt(node);
    }
}

/* Main TAC generation entry point */
void generateTAC(ASTNode* node) {
    if (!node) return;
    generateTACList(node);
}

/* Get operation name for printing */
static const char* getOpName(TACOp op) {
    switch(op) {
        case TAC_ADD: return "+";
        case TAC_SUB: return "-";
        case TAC_MUL: return "*";
        case TAC_DIV: return "/";
        case TAC_NEG: return "NEG";
        case TAC_LT: return "<";
        case TAC_GT: return ">";
        case TAC_LE: return "<=";
        case TAC_GE: return ">=";
        case TAC_EQ: return "==";
        case TAC_NE: return "!=";
        default: return "?";
    }
}

void printTAC() {
    printf("Unoptimized TAC Instructions:\n");
    printf("─────────────────────────────\n");
    TACInstr* curr = tacList.head;
    int lineNum = 1;
    while (curr) {
        printf("%3d: ", lineNum++);
        switch(curr->op) {
            case TAC_FUNC_BEGIN:
                printf("FUNC_BEGIN %s\n", curr->result);
                break;
            case TAC_FUNC_END:
                printf("FUNC_END %s\n", curr->result);
                break;
            case TAC_PARAM:
                printf("PARAM %s\n", curr->result);
                break;
            case TAC_DECL:
                printf("DECL %s\n", curr->result);
                break;
            case TAC_ADD:
            case TAC_SUB:
            case TAC_MUL:
            case TAC_DIV:
            case TAC_LT:
            case TAC_GT:
            case TAC_LE:
            case TAC_GE:
            case TAC_EQ:
            case TAC_NE:
                printf("%s = %s %s %s\n", curr->result, curr->arg1,
                       getOpName(curr->op), curr->arg2);
                break;
            case TAC_NEG:
                printf("%s = -%s\n", curr->result, curr->arg1);
                break;
            case TAC_ASSIGN:
                printf("%s = %s\n", curr->result, curr->arg1);
                break;
            case TAC_PRINT:
                printf("PRINT %s\n", curr->arg1);
                break;
            case TAC_ARG:
                printf("ARG %s\n", curr->arg1);
                break;
            case TAC_CALL:
                printf("%s = CALL %s, %s\n", curr->result, curr->arg1, curr->arg2);
                break;
            case TAC_RETURN:
                if (curr->arg1) {
                    printf("RETURN %s\n", curr->arg1);
                } else {
                    printf("RETURN\n");
                }
                break;
            case TAC_LABEL:
                printf("%s:\n", curr->result);
                break;
            case TAC_GOTO:
                printf("GOTO %s\n", curr->arg1);
                break;
            case TAC_IF_FALSE:
                printf("IF_FALSE %s GOTO %s\n", curr->arg1, curr->arg2);
                break;
            case TAC_IF_TRUE:
                printf("IF_TRUE %s GOTO %s\n", curr->arg1, curr->arg2);
                break;
            case TAC_ARRAY_DECL:
                printf("ARRAY_DECL %s[%s]\n", curr->result, curr->arg1);
                break;
            case TAC_ARRAY_ASSIGN:
                printf("ARRAY_ASSIGN %s[%s] = %s\n", curr->result, curr->arg1, curr->arg2);
                break;
            case TAC_ARRAY_ACCESS:
                printf("%s = %s[%s]\n", curr->result, curr->arg1, curr->arg2);
                break;
            default:
                printf("UNKNOWN\n");
                break;
        }
        curr = curr->next;
    }
}

/* Helper: Check if string is a constant number */
static int isConstant(const char* str);
static int isConstantNumber(const char* str) {
    if (!str || str[0] == '\0') return 0;
    int i = 0;
    if (str[i] == '-' || str[i] == '+') i++;
    if (str[i] == '\0') return 0;
    while (str[i] != '\0') {
        if (str[i] < '0' || str[i] > '9') return 0;
        i++;
    }
    return 1;
}

static int isConstant(const char* str) {
    return isConstantNumber(str);
}

/* Helper: Perform constant folding on binary operation */
static char* foldConstants(TACOp op, const char* arg1, const char* arg2, int* folded) {
    *folded = 0;

    if (!isConstantNumber(arg1) || !isConstantNumber(arg2)) {
        return NULL;
    }

    int val1 = atoi(arg1);
    int val2 = atoi(arg2);
    int result = 0;

    switch(op) {
        case TAC_ADD: result = val1 + val2; *folded = 1; break;
        case TAC_SUB: result = val1 - val2; *folded = 1; break;
        case TAC_MUL: result = val1 * val2; *folded = 1; break;
        case TAC_DIV:
            if (val2 != 0) {
                result = val1 / val2;
                *folded = 1;
            }
            break;
        case TAC_LT: result = (val1 < val2) ? 1 : 0; *folded = 1; break;
        case TAC_GT: result = (val1 > val2) ? 1 : 0; *folded = 1; break;
        case TAC_LE: result = (val1 <= val2) ? 1 : 0; *folded = 1; break;
        case TAC_GE: result = (val1 >= val2) ? 1 : 0; *folded = 1; break;
        case TAC_EQ: result = (val1 == val2) ? 1 : 0; *folded = 1; break;
        case TAC_NE: result = (val1 != val2) ? 1 : 0; *folded = 1; break;
        default: break;
    }

    if (*folded) {
        char* resultStr = malloc(20);
        sprintf(resultStr, "%d", result);
        return resultStr;
    }
    return NULL;
}

/* TAC Optimization with tracking */
void optimizeTAC() {
    TACInstr* current = tacList.head;

    while (current && current->next) {
        // Pattern 1: Constant folding
        // t0 = 5; t1 = 10; t2 = t0 + t1; => t2 = 15;
        if (current->op == TAC_ASSIGN &&
            current->next->op == TAC_ASSIGN &&
            isConstant(current->arg1) &&
            isConstant(current->next->arg1)) {

            TACInstr* third = current->next->next;
            if (third && third->op == TAC_ADD &&
                strcmp(third->arg1, current->result) == 0 &&
                strcmp(third->arg2, current->next->result) == 0) {

                int val1 = atoi(current->arg1);
                int val2 = atoi(current->next->arg1);
                char buffer[20];
                sprintf(buffer, "%d", val1 + val2);

                // Replace with single assignment
                third->op = TAC_ASSIGN;
                third->arg1 = strdup(buffer);
                third->arg2 = NULL;

                // Remove redundant instructions
                current->op = TAC_NOP;  // Mark for removal
                current->next->op = TAC_NOP;

                opt_stats.constant_folded++;
            }
        }

        // Pattern 2: Copy propagation
        // t0 = x; y = t0; => y = x;
        if (current->op == TAC_ASSIGN &&
            current->next->op == TAC_ASSIGN &&
            strcmp(current->next->arg1, current->result) == 0) {

            current->next->arg1 = strdup(current->arg1);
            current->op = TAC_NOP;
            opt_stats.eliminated_temps++;
        }

        current = current->next;
    }

    // Remove NOP instructions
    removeNOPs();

    // Copy optimized tacList into optimizedList
    TACInstr* src = tacList.head;
    while (src) {
        TACInstr* copy = (TACInstr*)malloc(sizeof(TACInstr));
        *copy = *src;
        copy->result = src->result ? strdup(src->result) : NULL;
        copy->arg1 = src->arg1 ? strdup(src->arg1) : NULL;
        copy->arg2 = src->arg2 ? strdup(src->arg2) : NULL;
        copy->type = src->type ? strdup(src->type) : NULL;
        copy->next = NULL;
        appendOptimizedTAC(copy);
        src = src->next;
    }
}

void printOptimizedTAC() {
    printf("Optimized TAC Instructions:\n");
    printf("─────────────────────────────\n");
    TACInstr* curr = optimizedList.head;
    int lineNum = 1;
    while (curr) {
        printf("%3d: ", lineNum++);
        switch(curr->op) {
            case TAC_FUNC_BEGIN:
                printf("FUNC_BEGIN %s\n", curr->result);
                break;
            case TAC_FUNC_END:
                printf("FUNC_END %s\n", curr->result);
                break;
            case TAC_PARAM:
                printf("PARAM %s\n", curr->result);
                break;
            case TAC_DECL:
                printf("DECL %s\n", curr->result);
                break;
            case TAC_ADD:
            case TAC_SUB:
            case TAC_MUL:
            case TAC_DIV:
            case TAC_LT:
            case TAC_GT:
            case TAC_LE:
            case TAC_GE:
            case TAC_EQ:
            case TAC_NE:
                printf("%s = %s %s %s\n", curr->result, curr->arg1,
                       getOpName(curr->op), curr->arg2);
                break;
            case TAC_NEG:
                printf("%s = -%s\n", curr->result, curr->arg1);
                break;
            case TAC_ASSIGN:
                printf("%s = %s\n", curr->result, curr->arg1);
                break;
            case TAC_PRINT:
                printf("PRINT %s\n", curr->arg1);
                break;
            case TAC_ARG:
                printf("ARG %s\n", curr->arg1);
                break;
            case TAC_CALL:
                printf("%s = CALL %s, %s\n", curr->result, curr->arg1, curr->arg2);
                break;
            case TAC_RETURN:
                if (curr->arg1) {
                    printf("RETURN %s\n", curr->arg1);
                } else {
                    printf("RETURN\n");
                }
                break;
            case TAC_LABEL:
                printf("%s:\n", curr->result);
                break;
            case TAC_GOTO:
                printf("GOTO %s\n", curr->arg1);
                break;
            case TAC_IF_FALSE:
                printf("IF_FALSE %s GOTO %s\n", curr->arg1, curr->arg2);
                break;
            case TAC_IF_TRUE:
                printf("IF_TRUE %s GOTO %s\n", curr->arg1, curr->arg2);
                break;
            case TAC_ARRAY_DECL:
                printf("ARRAY_DECL %s[%s]\n", curr->result, curr->arg1);
                break;
            case TAC_ARRAY_ASSIGN:
                printf("ARRAY_ASSIGN %s[%s] = %s\n", curr->result, curr->arg1, curr->arg2);
                break;
            case TAC_ARRAY_ACCESS:
                printf("%s = %s[%s]\n", curr->result, curr->arg1, curr->arg2);
                break;
            default:
                printf("UNKNOWN\n");
                break;
        }
        curr = curr->next;
    }
}

TACList* getOptimizedTAC() {
    return &optimizedList;
}

void saveTACToFile(const char* filename) {
    FILE* file = fopen(filename, "w");
    if (!file) {
        fprintf(stderr, "Error: Cannot open file '%s' for writing\n", filename);
        return;
    }

    fprintf(file, "Unoptimized Three-Address Code (TAC)\n");
    fprintf(file, "=====================================\n");
    fprintf(file, "Intermediate representation with functions\n\n");

    TACInstr* curr = tacList.head;
    int lineNum = 1;
    while (curr) {
        fprintf(file, "%3d: ", lineNum++);
        switch(curr->op) {
            case TAC_FUNC_BEGIN:
                fprintf(file, "FUNC_BEGIN %s\n", curr->result);
                break;
            case TAC_FUNC_END:
                fprintf(file, "FUNC_END %s\n", curr->result);
                break;
            case TAC_PARAM:
                fprintf(file, "PARAM %s\n", curr->result);
                break;
            case TAC_DECL:
                fprintf(file, "DECL %s\n", curr->result);
                break;
            case TAC_ADD:
            case TAC_SUB:
            case TAC_MUL:
            case TAC_DIV:
            case TAC_LT:
            case TAC_GT:
            case TAC_LE:
            case TAC_GE:
            case TAC_EQ:
            case TAC_NE:
                fprintf(file, "%s = %s %s %s\n", curr->result, curr->arg1,
                       getOpName(curr->op), curr->arg2);
                break;
            case TAC_NEG:
                fprintf(file, "%s = -%s\n", curr->result, curr->arg1);
                break;
            case TAC_ASSIGN:
                fprintf(file, "%s = %s\n", curr->result, curr->arg1);
                break;
            case TAC_PRINT:
                fprintf(file, "PRINT %s\n", curr->arg1);
                break;
            case TAC_ARG:
                fprintf(file, "ARG %s\n", curr->arg1);
                break;
            case TAC_CALL:
                fprintf(file, "%s = CALL %s, %s\n", curr->result, curr->arg1, curr->arg2);
                break;
            case TAC_RETURN:
                if (curr->arg1) {
                    fprintf(file, "RETURN %s\n", curr->arg1);
                } else {
                    fprintf(file, "RETURN\n");
                }
                break;
            case TAC_LABEL:
                fprintf(file, "%s:\n", curr->result);
                break;
            case TAC_GOTO:
                fprintf(file, "GOTO %s\n", curr->arg1);
                break;
            case TAC_IF_FALSE:
                fprintf(file, "IF_FALSE %s GOTO %s\n", curr->arg1, curr->arg2);
                break;
            case TAC_IF_TRUE:
                fprintf(file, "IF_TRUE %s GOTO %s\n", curr->arg1, curr->arg2);
                break;
            case TAC_ARRAY_DECL:
                fprintf(file, "ARRAY_DECL %s[%s]\n", curr->result, curr->arg1);
                break;
            case TAC_ARRAY_ASSIGN:
                fprintf(file, "ARRAY_ASSIGN %s[%s] = %s\n", curr->result, curr->arg1, curr->arg2);
                break;
            case TAC_ARRAY_ACCESS:
                fprintf(file, "%s = %s[%s]\n", curr->result, curr->arg1, curr->arg2);
                break;
            default:
                fprintf(file, "UNKNOWN\n");
                break;
        }
        curr = curr->next;
    }

    fclose(file);
    printf("✓ Unoptimized TAC saved to: %s\n", filename);
}

void saveOptimizedTACToFile(const char* filename) {
    FILE* file = fopen(filename, "w");
    if (!file) {
        fprintf(stderr, "Error: Cannot open file '%s' for writing\n", filename);
        return;
    }

    fprintf(file, "Optimized Three-Address Code (TAC)\n");
    fprintf(file, "===================================\n");
    fprintf(file, "With function support and control flow\n\n");

    TACInstr* curr = optimizedList.head;
    int lineNum = 1;
    while (curr) {
        fprintf(file, "%3d: ", lineNum++);
        switch(curr->op) {
            case TAC_FUNC_BEGIN:
                fprintf(file, "FUNC_BEGIN %s\n", curr->result);
                break;
            case TAC_FUNC_END:
                fprintf(file, "FUNC_END %s\n", curr->result);
                break;
            case TAC_PARAM:
                fprintf(file, "PARAM %s\n", curr->result);
                break;
            case TAC_DECL:
                fprintf(file, "DECL %s\n", curr->result);
                break;
            case TAC_ADD:
            case TAC_SUB:
            case TAC_MUL:
            case TAC_DIV:
            case TAC_LT:
            case TAC_GT:
            case TAC_LE:
            case TAC_GE:
            case TAC_EQ:
            case TAC_NE:
                fprintf(file, "%s = %s %s %s\n", curr->result, curr->arg1,
                       getOpName(curr->op), curr->arg2);
                break;
            case TAC_NEG:
                fprintf(file, "%s = -%s\n", curr->result, curr->arg1);
                break;
            case TAC_ASSIGN:
                fprintf(file, "%s = %s\n", curr->result, curr->arg1);
                break;
            case TAC_PRINT:
                fprintf(file, "PRINT %s\n", curr->arg1);
                break;
            case TAC_ARG:
                fprintf(file, "ARG %s\n", curr->arg1);
                break;
            case TAC_CALL:
                fprintf(file, "%s = CALL %s, %s\n", curr->result, curr->arg1, curr->arg2);
                break;
            case TAC_RETURN:
                if (curr->arg1) {
                    fprintf(file, "RETURN %s\n", curr->arg1);
                } else {
                    fprintf(file, "RETURN\n");
                }
                break;
            case TAC_LABEL:
                fprintf(file, "%s:\n", curr->result);
                break;
            case TAC_GOTO:
                fprintf(file, "GOTO %s\n", curr->arg1);
                break;
            case TAC_IF_FALSE:
                fprintf(file, "IF_FALSE %s GOTO %s\n", curr->arg1, curr->arg2);
                break;
            case TAC_IF_TRUE:
                fprintf(file, "IF_TRUE %s GOTO %s\n", curr->arg1, curr->arg2);
                break;
            case TAC_ARRAY_DECL:
                fprintf(file, "ARRAY_DECL %s[%s]\n", curr->result, curr->arg1);
                break;
            case TAC_ARRAY_ASSIGN:
                fprintf(file, "ARRAY_ASSIGN %s[%s] = %s\n", curr->result, curr->arg1, curr->arg2);
                break;
            case TAC_ARRAY_ACCESS:
                fprintf(file, "%s = %s[%s]\n", curr->result, curr->arg1, curr->arg2);
                break;
            default:
                fprintf(file, "UNKNOWN\n");
                break;
        }
        curr = curr->next;
    }

    fclose(file);
    printf("✓ Optimized TAC saved to: %s\n", filename);
}

void removeNOPs() {
    TACInstr* current = tacList.head;
    TACInstr* prev = NULL;

    while (current) {
        if (current->op == TAC_NOP) {
            TACInstr* toDelete = current;
            if (prev) {
                prev->next = current->next;
                current = current->next;
            } else {
                tacList.head = current->next;
                current = current->next;
            }
            opt_stats.dead_code_removed++;
            // Note: In production, should free toDelete
        } else {
            prev = current;
            current = current->next;
        }
    }
}
