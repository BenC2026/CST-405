#ifndef TAC_H
#define TAC_H

#include "ast.h"

/* THREE-ADDRESS CODE (TAC) - WITH FUNCTION SUPPORT
 * Intermediate representation between AST and machine code
 * Each instruction has at most 3 operands (result = arg1 op arg2)
 * Now supports functions, control flow, and all operators
 */

/* TAC INSTRUCTION TYPES */
typedef enum {
    /* Arithmetic operations */
    TAC_ADD,        /* result = arg1 + arg2 */
    TAC_SUB,        /* result = arg1 - arg2 */
    TAC_MUL,        /* result = arg1 * arg2 */
    TAC_DIV,        /* result = arg1 / arg2 */
    TAC_NEG,        /* result = -arg1 (unary minus) */

    /* Comparison operations */
    TAC_LT,         /* result = arg1 < arg2 */
    TAC_GT,         /* result = arg1 > arg2 */
    TAC_LE,         /* result = arg1 <= arg2 */
    TAC_GE,         /* result = arg1 >= arg2 */
    TAC_EQ,         /* result = arg1 == arg2 */
    TAC_NE,         /* result = arg1 != arg2 */

    /* Assignment and I/O */
    TAC_ASSIGN,     /* result = arg1 */
    TAC_PRINT,      /* print(arg1) */
    TAC_DECL,       /* declare result */

    /* Control flow */
    TAC_LABEL,      /* label: */
    TAC_GOTO,       /* goto label */
    TAC_IF_FALSE,   /* if_false arg1 goto label */
    TAC_IF_TRUE,    /* if_true arg1 goto label */

    /* Function operations */
    TAC_FUNC_BEGIN, /* function name: */
    TAC_FUNC_END,   /* end function name */
    TAC_PARAM,      /* param name */
    TAC_ARG,        /* arg value */
    TAC_CALL,       /* result = call name, argCount */
    TAC_RETURN,     /* return arg1 */

    TAC_ARRAY_DECL, /* declare array result of size arg1 */
    TAC_ARRAY_ASSIGN, /* result[arg1] = arg2 */
    TAC_ARRAY_ACCESS, /* result = arg1[arg2] */

    TAC_NOP           /* No operation (placeholder) */
} TACOp;

/* TAC INSTRUCTION STRUCTURE */
typedef struct TACInstr {
    TACOp op;               /* Operation type */
    char* arg1;             /* First operand */
    char* arg2;             /* Second operand */
    char* result;           /* Result/destination */
    char* type;             /* Type information for declarations and parameters */
    struct TACInstr* next;  /* Linked list pointer */
} TACInstr;

/* TAC LIST MANAGEMENT */
typedef struct {
    TACInstr* head;    /* First instruction */
    TACInstr* tail;    /* Last instruction */
    int tempCount;     /* Counter for temporary variables */
    int labelCount;    /* Counter for labels */
} TACList;

/* TEMPORARY VARIABLE ALLOCATION/DEALLOCATION */
#define MAX_TEMPS 100

typedef struct {
    int allocated[MAX_TEMPS];
    int maxUsed;
    int freeList[MAX_TEMPS];
    int freeCount;
} TempAllocator;

/* TAC GENERATION FUNCTIONS */
void initTAC();                                                    /* Initialize TAC */
char* newTemp();                                                   /* Generate new temp */
char* allocTemp();                                                 /* Allocate temp with reuse */
void freeTemp(char* temp);                                         /* Free temp */
char* newLabel();                                                  /* Generate new label */
void printTempAllocatorState();                                    /* Display stats */

TACInstr* createTAC(TACOp op, char* arg1, char* arg2, char* result, char* type); /* Create TAC */
void appendTAC(TACInstr* instr);                                  /* Add to list */
void generateTAC(ASTNode* node);                                  /* Generate from AST */
char* generateTACExpr(ASTNode* node);                             /* Generate for expr */

/* TAC OPTIMIZATION AND OUTPUT */
void printTAC();                                                   /* Display unoptimized */
void optimizeTAC();                                                /* Apply optimizations */
void printOptimizedTAC();                                          /* Display optimized */
TACList* getOptimizedTAC();                                        /* Get optimized list */
void saveTACToFile(const char* filename);                          /* Save unoptimized */
void saveOptimizedTACToFile(const char* filename);                 /* Save optimized */

#endif
