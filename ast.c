/* AST IMPLEMENTATION
 * Functions to create and manipulate Abstract Syntax Tree nodes
 * The AST is built during parsing and used for all subsequent phases
 */

#include <stdlib.h>

#define POOL_SIZE 4096  // Allocate in 4KB chunks
#define MAX_POOLS 100

typedef struct MemPool {
    char* memory;
    size_t used;
    size_t size;
    struct MemPool* next;
} MemPool;

typedef struct {
    MemPool* current;
    MemPool* head;
    int total_allocations;
    int pool_count;
    size_t total_memory;
} ASTMemoryManager;

static ASTMemoryManager ast_mem = {0};

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "ast.h"

/* External line number from scanner */
extern int yylineno;

/* Forward declaration */
void* ast_alloc(size_t size);

/* Create a number literal node */
ASTNode* createNum(int value) {
    ASTNode* node = (ASTNode*)ast_alloc(sizeof(ASTNode));
    node->type = NODE_NUM;
    node->lineno = yylineno;
    node->data.num = value;  /* Store the integer value */
    return node;
}

/* Create a variable reference node */
ASTNode* createVar(char* name) {
    ASTNode* node = (ASTNode*)ast_alloc(sizeof(ASTNode));
    node->type = NODE_VAR;
    node->lineno = yylineno;
    node->data.decl.name = strdup(name);  /* Copy the variable name */
    return node;
}

/* Create a binary operation node (for addition) */
ASTNode* createBinOp(char op, ASTNode* left, ASTNode* right) {
    ASTNode* node = (ASTNode*)ast_alloc(sizeof(ASTNode));
    node->type = NODE_BINOP;
    node->lineno = yylineno;
    node->data.binop.op = op;        /* Store operator (+) */
    node->data.binop.left = left;    /* Left subtree */
    node->data.binop.right = right;  /* Right subtree */
    return node;
}

/* Create a variable declaration node */
ASTNode* createDecl(char* name, char* type) {
    ASTNode* node = (ASTNode*)ast_alloc(sizeof(ASTNode));
    node->type = NODE_DECL;
    node->lineno = yylineno;
    node->data.decl.name = strdup(name);  /* Store variable name */
    node->data.decl.type = strdup(type);  /* Store variable type */
    return node;
}

/*
ASTNode* createDeclWithAssgn(char* name, int value) {
    ASTNode* node = malloc(sizeof(ASTNode));
    node->type = NODE_DECL;
    node->data.name = strdup(name); 
    node->data.value = value;
    return node;
}

*/

/* Create an assignment statement node */
ASTNode* createAssign(char* var, ASTNode* value) {
    ASTNode* node = (ASTNode*)ast_alloc(sizeof(ASTNode));
    node->type = NODE_ASSIGN;
    node->lineno = yylineno;
    node->data.assign.var = strdup(var);  /* Variable name */
    node->data.assign.value = value;      /* Expression tree */
    return node;
}

/* Create a print statement node */
ASTNode* createPrint(ASTNode* expr) {
    ASTNode* node = (ASTNode*)ast_alloc(sizeof(ASTNode));
    node->type = NODE_PRINT;
    node->lineno = yylineno;
    node->data.expr = expr;  /* Expression to print */
    return node;
}

/* Create a statement list node (links statements together) */
ASTNode* createStmtList(ASTNode* stmt1, ASTNode* stmt2) {
    ASTNode* node = (ASTNode*)ast_alloc(sizeof(ASTNode));
    node->type = NODE_STMT_LIST;
    node->lineno = stmt1 ? stmt1->lineno : (stmt2 ? stmt2->lineno : yylineno);
    node->data.stmtlist.stmt = stmt1;  /* First statement */
    node->data.stmtlist.next = stmt2;  /* Rest of list */
    return node;
}

/* Create a function definition node */
ASTNode* createFuncDef(char* name, ASTNode* params, ASTNode* body) {
    ASTNode* node = (ASTNode*)ast_alloc(sizeof(ASTNode));
    node->type = NODE_FUNC_DEF;
    node->lineno = yylineno;
    node->data.func_def.name = strdup(name);
    node->data.func_def.params = params;
    node->data.func_def.body = body;
    return node;
}

/* Create a parameter node */
ASTNode* createParam(char* name) {
    ASTNode* node = (ASTNode*)ast_alloc(sizeof(ASTNode));
    node->type = NODE_PARAM;
    node->lineno = yylineno;
    node->data.param.name = strdup(name);
    return node;
}

/* Create a parameter list node */
ASTNode* createParamList(ASTNode* param, ASTNode* next) {
    ASTNode* node = (ASTNode*)ast_alloc(sizeof(ASTNode));
    node->type = NODE_PARAM_LIST;
    node->lineno = param ? param->lineno : yylineno;
    node->data.param_list.param = param;
    node->data.param_list.next = next;
    return node;
}

/* Create a function call node */
ASTNode* createFuncCall(char* name, ASTNode* args) {
    ASTNode* node = (ASTNode*)ast_alloc(sizeof(ASTNode));
    node->type = NODE_FUNC_CALL;
    node->lineno = yylineno;
    node->data.func_call.name = strdup(name);
    node->data.func_call.args = args;
    return node;
}

/* Create an argument list node */
ASTNode* createArgList(ASTNode* expr, ASTNode* next) {
    ASTNode* node = (ASTNode*)ast_alloc(sizeof(ASTNode));
    node->type = NODE_ARG_LIST;
    node->lineno = expr ? expr->lineno : yylineno;
    node->data.arg_list.expr = expr;
    node->data.arg_list.next = next;
    return node;
}

/* Create a return statement node */
ASTNode* createReturn(ASTNode* expr) {
    ASTNode* node = (ASTNode*)ast_alloc(sizeof(ASTNode));
    node->type = NODE_RETURN;
    node->lineno = yylineno;
    node->data.ret.expr = expr;
    return node;
}

/* Create an if statement node */
ASTNode* createIf(ASTNode* condition, ASTNode* then_stmt, ASTNode* else_stmt) {
    ASTNode* node = (ASTNode*)ast_alloc(sizeof(ASTNode));
    node->type = NODE_IF;
    node->lineno = yylineno;
    node->data.if_stmt.condition = condition;
    node->data.if_stmt.then_stmt = then_stmt;
    node->data.if_stmt.else_stmt = else_stmt;
    return node;
}

/* Create a while loop node */
ASTNode* createWhile(ASTNode* condition, ASTNode* body) {
    ASTNode* node = (ASTNode*)ast_alloc(sizeof(ASTNode));
    node->type = NODE_WHILE;
    node->lineno = yylineno;
    node->data.while_stmt.condition = condition;
    node->data.while_stmt.body = body;
    return node;
}

/* Create a for loop node */
ASTNode* createFor(ASTNode* init, ASTNode* condition, ASTNode* update, ASTNode* body) {
    ASTNode* node = (ASTNode*)ast_alloc(sizeof(ASTNode));
    node->type = NODE_FOR;
    node->lineno = yylineno;
    node->data.for_stmt.init = init;
    node->data.for_stmt.condition = condition;
    node->data.for_stmt.update = update;
    node->data.for_stmt.body = body;
    return node;
}

/* Create a block statement node */
ASTNode* createBlock(ASTNode* stmt_list) {
    ASTNode* node = (ASTNode*)ast_alloc(sizeof(ASTNode));
    node->type = NODE_BLOCK;
    node->lineno = yylineno;
    node->data.block.stmt_list = stmt_list;
    return node;
}

ASTNode* createArrayDecl(char* name, char* type, int size) {
    ASTNode* node = (ASTNode*)ast_alloc(sizeof(ASTNode));
    node->type = NODE_ARRAY_DECL;
    node->lineno = yylineno;
    node->data.array_decl.name = strdup(name);
    node->data.array_decl.type = strdup(type);
    node->data.array_decl.size = size;
    return node;
}

ASTNode* createArrayAssign(char* name, ASTNode* index, ASTNode* value) {
    ASTNode* node = (ASTNode*)ast_alloc(sizeof(ASTNode));
    node->type = NODE_ARRAY_ASSIGN;
    node->lineno = yylineno;
    node->data.array_assign.name = strdup(name);
    node->data.array_assign.index = index;
    node->data.array_assign.value = value;
    return node;
}

ASTNode* createArrayAccess(char* name, ASTNode* index) {
    ASTNode* node = (ASTNode*)ast_alloc(sizeof(ASTNode));
    node->type = NODE_ARRAY_ACCESS;
    node->lineno = yylineno;
    node->data.array_access.name = strdup(name);
    node->data.array_access.index = index;
    return node;
}

/* Display the AST structure (for debugging and education) */
void printAST(ASTNode* node, int level) {
    if (!node) return;

    /* Indent based on tree depth */
    for (int i = 0; i < level; i++) printf("  ");

    /* Print node based on its type */
    switch(node->type) {
        case NODE_NUM:
            printf("NUM: %d\n", node->data.num);
            break;
        case NODE_VAR:
            printf("VAR: %s\n", node->data.decl.name);
            break;
        case NODE_BINOP:
            printf("BINOP: %c\n", node->data.binop.op);
            printAST(node->data.binop.left, level + 1);
            printAST(node->data.binop.right, level + 1);
            break;
        case NODE_DECL:
            printf("DECL: %s (%s)\n", node->data.decl.name, node->data.decl.type);
            break;
        case NODE_ASSIGN:
            printf("ASSIGN: %s\n", node->data.assign.var);
            printAST(node->data.assign.value, level + 1);
            break;
        case NODE_PRINT:
            printf("PRINT\n");
            printAST(node->data.expr, level + 1);
            break;
        case NODE_STMT_LIST:
            /* Print statements in sequence at same level */
            printAST(node->data.stmtlist.stmt, level);
            printAST(node->data.stmtlist.next, level);
            break;
        case NODE_FUNC_DEF:
            printf("FUNC_DEF: %s\n", node->data.func_def.name);
            if (node->data.func_def.params) {
                for (int i = 0; i < level + 1; i++) printf("  ");
                printf("PARAMS:\n");
                printAST(node->data.func_def.params, level + 2);
            }
            for (int i = 0; i < level + 1; i++) printf("  ");
            printf("BODY:\n");
            printAST(node->data.func_def.body, level + 2);
            break;
        case NODE_PARAM:
            printf("PARAM: %s\n", node->data.param.name);
            break;
        case NODE_PARAM_LIST:
            printAST(node->data.param_list.param, level);
            printAST(node->data.param_list.next, level);
            break;
        case NODE_FUNC_CALL:
            printf("FUNC_CALL: %s\n", node->data.func_call.name);
            if (node->data.func_call.args) {
                for (int i = 0; i < level + 1; i++) printf("  ");
                printf("ARGS:\n");
                printAST(node->data.func_call.args, level + 2);
            }
            break;
        case NODE_ARG_LIST:
            printAST(node->data.arg_list.expr, level);
            printAST(node->data.arg_list.next, level);
            break;
        case NODE_RETURN:
            printf("RETURN\n");
            if (node->data.ret.expr) {
                printAST(node->data.ret.expr, level + 1);
            }
            break;
        case NODE_IF:
            printf("IF\n");
            for (int i = 0; i < level + 1; i++) printf("  ");
            printf("CONDITION:\n");
            printAST(node->data.if_stmt.condition, level + 2);
            for (int i = 0; i < level + 1; i++) printf("  ");
            printf("THEN:\n");
            printAST(node->data.if_stmt.then_stmt, level + 2);
            if (node->data.if_stmt.else_stmt) {
                for (int i = 0; i < level + 1; i++) printf("  ");
                printf("ELSE:\n");
                printAST(node->data.if_stmt.else_stmt, level + 2);
            }
            break;
        case NODE_WHILE:
            printf("WHILE\n");
            for (int i = 0; i < level + 1; i++) printf("  ");
            printf("CONDITION:\n");
            printAST(node->data.while_stmt.condition, level + 2);
            for (int i = 0; i < level + 1; i++) printf("  ");
            printf("BODY:\n");
            printAST(node->data.while_stmt.body, level + 2);
            break;
        case NODE_FOR:
            printf("FOR\n");
            for (int i = 0; i < level + 1; i++) printf("  ");
            printf("INIT:\n");
            printAST(node->data.for_stmt.init, level + 2);
            for (int i = 0; i < level + 1; i++) printf("  ");
            printf("CONDITION:\n");
            printAST(node->data.for_stmt.condition, level + 2);
            for (int i = 0; i < level + 1; i++) printf("  ");
            printf("UPDATE:\n");
            printAST(node->data.for_stmt.update, level + 2);
            for (int i = 0; i < level + 1; i++) printf("  ");
            printf("BODY:\n");
            printAST(node->data.for_stmt.body, level + 2);
            break;
        case NODE_BLOCK:
            printf("BLOCK\n");
            printAST(node->data.block.stmt_list, level + 1);
            break;
        case NODE_ARRAY_DECL:
            printf("%*sARRAY_DECL: %s[%d]\n", level*2, "", node->data.array_decl.name, node->data.array_decl.size);
            break;
        case NODE_ARRAY_ASSIGN:
            printf("%*sARRAY_ASSIGN: %s[] =\n", level*2, "", node->data.array_assign.name);
            printf("%*sIndex:\n", level*2, "");
            printAST(node->data.array_assign.index, level+1);
            printf("%*sValue:\n", level*2, "");
            printAST(node->data.array_assign.value, level+1);
            break;
        case NODE_ARRAY_ACCESS:
            printf("%*sARRAY_ACCESS: %s[]\n", level*2, "", node->data.array_access.name);
            printf("%*sIndex:\n", level*2, "");
            printAST(node->data.array_access.index, level+1);
            break;
    }
}

void init_ast_memory() {
    ast_mem.head = malloc(sizeof(MemPool));
    ast_mem.head->memory = malloc(POOL_SIZE);
    ast_mem.head->used = 0;
    ast_mem.head->size = POOL_SIZE;
    ast_mem.head->next = NULL;
    ast_mem.current = ast_mem.head;
    ast_mem.pool_count = 1;
    ast_mem.total_memory = POOL_SIZE;
}

// Allocate from pool
void* ast_alloc(size_t size) {
    // Align to 8 bytes for better performance
    size = (size + 7) & ~7;

    if (ast_mem.current->used + size > ast_mem.current->size) {
        // Need new pool
        MemPool* new_pool = malloc(sizeof(MemPool));
        new_pool->memory = malloc(POOL_SIZE);
        new_pool->used = 0;
        new_pool->size = POOL_SIZE;
        new_pool->next = NULL;

        ast_mem.current->next = new_pool;
        ast_mem.current = new_pool;
        ast_mem.pool_count++;
        ast_mem.total_memory += POOL_SIZE;
    }

    void* ptr = ast_mem.current->memory + ast_mem.current->used;
    ast_mem.current->used += size;
    ast_mem.total_allocations++;

    return ptr;
}
