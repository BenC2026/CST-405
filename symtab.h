#ifndef SYMTAB_H
#define SYMTAB_H
#define HASH_SIZE 211  // Prime number for better distribution

/* SYMBOL TABLE
 * Tracks all declared variables during compilation
 * Maps variable names to their memory locations (stack offsets)
 * Used for semantic checking and code generation
 */

#define MAX_VARS 1000  /* Maximum number of variables supported */

/* VARIABLE TYPES */
typedef enum {
    TYPE_INT,
    TYPE_FLOAT,
    TYPE_CHAR,
    TYPE_BOOL,
    TYPE_STRING,
    TYPE_VOID,
    TYPE_UNKNOWN
} VarType;

/* SYMBOL ENTRY - Information about each variable */
typedef struct Symbol {
    char* name;     /* Variable identifier */
    char* type;     /* Variable type (e.g., "int") */
    int offset;     /* Stack offset in bytes (for MIPS stack frame) */
    int isArray;   /* 1 if variable is an array, 0 otherwise */
    int arraySize; /* Size of the array if isArray is 1 */
    struct Symbol* next; /* For handling hash collisions (chaining) */
} Symbol;

/* SYMBOL TABLE STRUCTURE */
typedef struct {
    Symbol vars[MAX_VARS];  /* Array of all variables */
    Symbol* hashTable[HASH_SIZE]; /* Hash table for quick lookups */
    int count;              /* Number of variables declared */
    int nextOffset;         /* Next available stack offset */
    int lookups;
    int collisions;
} SymbolTable;



/* Global symbol table instance (defined in symtab.c) */
extern SymbolTable symtab;

/* SYMBOL TABLE OPERATIONS */
void initSymTab();               /* Initialize empty symbol table */
int addVar(char* name, char* type);          /* Add new variable, returns offset or -1 if duplicate */
int getVarOffset(char* name);    /* Get stack offset for variable, -1 if not found */
char* getVarType(char* name);    /* Get type string for variable, NULL if not found */
int isVarDeclared(char* name);   /* Check if variable exists (1=yes, 0=no) */
int addArrayVar(char* name, char* type, int size); /* Add new array variable */
int isArrayVar(char* name);      /* Check if variable is an array (1=yes, 0=no) */
int getArraySize(char* name);    /* Get size of array */
void printSymTab();              /* Print current symbol table contents for tracing */
unsigned int hash(const char* str);          /* Hash function for variable names */

#endif
