#ifndef AST_H
#define AST_H

/* ABSTRACT SYNTAX TREE (AST)
 * The AST is an intermediate representation of the program structure
 * It represents the hierarchical syntax of the source code
 * Each node represents a construct in the language
 */

/* NODE TYPES - Different kinds of AST nodes in our language */
typedef enum {
    NODE_NUM,         /* Numeric literal (e.g., 42) */
    NODE_VAR,         /* Variable reference (e.g., x) */
    NODE_BINOP,       /* Binary operation (e.g., x + y) */
    NODE_DECL,        /* Variable declaration (e.g., int x) */
    NODE_ASSIGN,      /* Assignment statement (e.g., x = 10) */
    NODE_PRINT,       /* Print statement (e.g., print(x)) */
    NODE_STMT_LIST,   /* List of statements (program structure) */
    NODE_FUNC_DEF,    /* Function definition */
    NODE_PARAM,       /* Parameter (name only, type is always int) */
    NODE_PARAM_LIST,  /* Parameter list */
    NODE_FUNC_CALL,   /* Function call */
    NODE_ARG_LIST,    /* Argument list */
    NODE_RETURN,      /* Return statement */
    NODE_IF,          /* If statement */
    NODE_WHILE,       /* While loop */
    NODE_FOR,         /* For loop (not implemented yet) */
    NODE_BLOCK,       /* Block statement { ... } */
    NODE_ARRAY_DECL,  /* Array declaration (e.g., int arr[10]) */
    NODE_ARRAY_ASSIGN,/* Array assignment (e.g., arr[0] = 5) */
    NODE_ARRAY_ACCESS,/* Array access (e.g., arr[0]) */
    NODE_SWITCH,      /* Switch statement (not implemented yet) */
    NODE_CASE,        /* Case in switch statement (not implemented yet) */
    NODE_BREAK,       /* Break statement (not implemented yet) */
    NODE_STRING_LIT,  /* String literal (e.g., "Hello") */
    NODE_FLOAT_LIT    /* Floating-point literal (e.g., 3.14) */
} NodeType;

/* AST NODE STRUCTURE
 * Uses a union to efficiently store different node data
 * Only the relevant fields for each node type are used
 */
typedef struct ASTNode {
    NodeType type;  /* Identifies what kind of node this is */
    int lineno;     /* Line number in source code for error reporting */
    char* sval;     /* String literal value (NODE_STRING_LIT) */
    double fval;    /* Float literal value (NODE_FLOAT_LIT) */

    /* Union allows same memory to store different data types */
    union {
        /* Literal number value (NODE_NUM) */
        int num;
        
        /* Variable or declaration name (NODE_VAR, NODE_DECL) */
        struct {
            char* name;     /* Variable identifier */
            char* type;     /* Variable type */
        } decl;
        /* Variable type for declarations (NODE_DECL) */
        /* int value; */ /* For potential future use in declarations with assignment */

        
        /* Binary operation structure (NODE_BINOP) */
        struct {
            char op;                    /* Operator character ('+') */
            struct ASTNode* left;       /* Left operand */
            struct ASTNode* right;      /* Right operand */
        } binop;
        
        /* Assignment structure (NODE_ASSIGN) */
        struct {
            char* var;                  /* Variable being assigned to */
            struct ASTNode* value;      /* Expression being assigned */
        } assign;
        
        /* Print expression (NODE_PRINT) */
        struct ASTNode* expr;
        
        /* Statement list structure (NODE_STMT_LIST) */
        struct {
            struct ASTNode* stmt;       /* Current statement */
            struct ASTNode* next;       /* Rest of the list */
        } stmtlist;

        /* Function definition (NODE_FUNC_DEF) */
        struct {
            char* name;                 /* Function name */
            struct ASTNode* params;     /* Parameter list */
            struct ASTNode* body;       /* Function body (block or stmt_list) */
        } func_def;

        /* Parameter (NODE_PARAM) - just a name */
        struct {
            char* name;
            char* type; /* For potential future use if we add types to parameters */
        } param;

        /* Parameter list (NODE_PARAM_LIST) */
        struct {
            struct ASTNode* param;      /* Current parameter */
            struct ASTNode* next;       /* Rest of parameters */
        } param_list;

        /* Function call (NODE_FUNC_CALL) */
        struct {
            char* name;                 /* Function name */
            struct ASTNode* args;       /* Argument list */
        } func_call;

        /* Argument list (NODE_ARG_LIST) */
        struct {
            struct ASTNode* expr;       /* Current argument expression */
            struct ASTNode* next;       /* Rest of arguments */
        } arg_list;

        /* Return statement (NODE_RETURN) */
        struct {
            struct ASTNode* expr;       /* Expression to return (NULL for void) */
        } ret;

        /* If statement (NODE_IF) */
        struct {
            struct ASTNode* condition;  /* Condition expression */
            struct ASTNode* then_stmt;  /* Then branch */
            struct ASTNode* else_stmt;  /* Else branch (NULL if no else) */
        } if_stmt;

        /* While loop (NODE_WHILE) */
        struct {
            struct ASTNode* condition;  /* Loop condition */
            struct ASTNode* body;       /* Loop body */
        } while_stmt;

        /* For loop (NODE_FOR) */
        struct {
            struct ASTNode* init;       /* Initialization statement */
            struct ASTNode* condition;  /* Loop condition */
            struct ASTNode* update;     /* Update statement */
            struct ASTNode* body;       /* Loop body */
        } for_stmt;

        /* Block statement (NODE_BLOCK) */
        struct {
            struct ASTNode* stmt_list;  /* Statements in block */
        } block;

        struct {
            char* name;     /* Array name */
            char* type;     /* Array type */
            int size;       /* Size of the array */
        } array_decl;     /* Array declaration (NODE_ARRAY_DECL) */

        struct {
            char* name;                 /* Array name */
            struct ASTNode* index;      /* Index expression */
            struct ASTNode* value;      /* Value to assign */
        } array_assign;  /* Array assignment (NODE_ARRAY_ASSIGN) */

        struct {
            char* name;                 /* Array name */
            struct ASTNode* index;      /* Index expression */
        } array_access;  /* Array access (NODE_ARRAY_ACCESS) */

        /* Switch statement (NODE_SWITCH) */
        struct {
            struct ASTNode* expr;    /* Controlling expression (evaluated once) */
            struct ASTNode* cases;   /* Linked list of NODE_CASE nodes */
        } switch_stmt;

        /* Case / default clause (NODE_CASE) 
        * The 'next' pointer links this clause to the following one,
        * forming a singly-linked list through all cases in the switch. */
        struct {
            int  value;              /* Case constant (unused when isDefault == 1) */
            int  isDefault;          /* 1 → default clause, 0 → case clause */
            struct ASTNode* body;    /* Statement list (NULL = empty/fall-through) */
            struct ASTNode* next;    /* Next case/default clause */
        } case_clause;
    } data;
} ASTNode;

/* AST CONSTRUCTION FUNCTIONS
 * These functions are called by the parser to build the tree
 */
/* Basic nodes */
ASTNode* createNum(int value);                                   /* Create number node */
ASTNode* createVar(char* name);                                  /* Create variable node */
ASTNode* createBinOp(char op, ASTNode* left, ASTNode* right);   /* Create binary op node */
ASTNode* createDecl(char* name, char* type);                    /* Create declaration node */
ASTNode* createAssign(char* var, ASTNode* value);               /* Create assignment node */
ASTNode* createPrint(ASTNode* expr);                            /* Create print node */
ASTNode* createStmtList(ASTNode* stmt1, ASTNode* stmt2);        /* Create statement list */

/* Function-related nodes */
ASTNode* createFuncDef(char* name, ASTNode* params, ASTNode* body);  /* Create function definition */
ASTNode* createParam(char* name);                               /* Create parameter */
ASTNode* createParamList(ASTNode* param, ASTNode* next);        /* Create parameter list */
ASTNode* createFuncCall(char* name, ASTNode* args);             /* Create function call */
ASTNode* createArgList(ASTNode* expr, ASTNode* next);           /* Create argument list */
ASTNode* createReturn(ASTNode* expr);                           /* Create return statement */

/* Control flow nodes */
ASTNode* createIf(ASTNode* condition, ASTNode* then_stmt, ASTNode* else_stmt);  /* Create if statement */
ASTNode* createWhile(ASTNode* condition, ASTNode* body);        /* Create while loop */
ASTNode* createFor(ASTNode* init, ASTNode* condition, ASTNode* update, ASTNode* body); /* Create for loop (not implemented yet) */
ASTNode* createBlock(ASTNode* stmt_list);                       /* Create block statement */

/* Array-related nodes */
ASTNode* createArrayDecl(char* name, char* type, int size);                /* Create array declaration */
ASTNode* createArrayAssign(char* name, ASTNode* index, ASTNode* value); /* Create array assignment */
ASTNode* createArrayAccess(char* name, ASTNode* index);          /* Create array access */

/* Switch / case / break constructors */
ASTNode* createSwitch(ASTNode* expr, ASTNode* cases);
ASTNode* createCase(int value, int isDefault, ASTNode* body);
ASTNode* createBreak();

/* String literal constructor */
ASTNode* makeStringLit(const char* s);

/* Float literal constructor */
ASTNode* makeFloatLit(double val);

/* Memory pool initialization */
void init_ast_memory(void);

/* AST DISPLAY FUNCTION */
void printAST(ASTNode* node, int level);                        /* Pretty-print the AST */

#endif
