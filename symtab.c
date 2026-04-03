/* SYMBOL TABLE IMPLEMENTATION
 * Manages variable declarations and lookups
 * Essential for semantic analysis (checking if variables are declared)
 * Provides memory layout information for code generation
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "symtab.h"

/* Global symbol table instance */
SymbolTable symtab;

/* Initialize an empty symbol table */
void initSymTab() {
    symtab.count = 0;       /* No variables yet */
    symtab.nextOffset = 0;  /* Start at stack offset 0 */
    printf("SYMBOL TABLE: Initialized\n");
    printSymTab();
}

/* Add a new variable to the symbol table */
int addVar(char* name, char* type) {
    /* Check for duplicate declaration */
    if (isVarDeclared(name)) {
        printf("SYMBOL TABLE: Failed to add '%s' - already declared\n", name);
        return -1;  /* Error: variable already exists */
    }

    /* Add new symbol entry */
    symtab.vars[symtab.count].name = strdup(name);
    symtab.vars[symtab.count].type = strdup(type);
    symtab.vars[symtab.count].offset = symtab.nextOffset;
    symtab.vars[symtab.count].isArray = 0;
    symtab.vars[symtab.count].arraySize = 0;

    /* Advance offset by 4 bytes (size of int in MIPS) */
    symtab.nextOffset += 4;
    symtab.count++;

    printf("SYMBOL TABLE: Added variable '%s' at offset %d\n", name, symtab.vars[symtab.count - 1].offset);
    printSymTab();

    /* Return the offset for this variable */
    return symtab.vars[symtab.count - 1].offset;
}

/* Look up a variable's stack offset */
int getVarOffset(char* name) {
    /* Linear search through symbol table */
    for (int i = 0; i < symtab.count; i++) {
        if (strcmp(symtab.vars[i].name, name) == 0) {
            printf("SYMBOL TABLE: Found variable '%s' at offset %d\n", name, symtab.vars[i].offset);
            return symtab.vars[i].offset;  /* Found it */
        }
    }
    printf("SYMBOL TABLE: Variable '%s' not found\n", name);
    return -1;  /* Variable not found - semantic error */
}

/* Look up a variable's type string ("int", "float", etc.), NULL if not found */
char* getVarType(char* name) {
    for (int i = 0; i < symtab.count; i++) {
        if (strcmp(symtab.vars[i].name, name) == 0)
            return symtab.vars[i].type;
    }
    return NULL;
}

/* Check if a variable has been declared */
int isVarDeclared(char* name) {
    return getVarOffset(name) != -1;  /* True if found, false otherwise */
}

int addArrayVar(char* name, char* type, int size) {
    /* Check for duplicate declaration */
    if (isVarDeclared(name)) {
        printf("SYMBOL TABLE: Failed to add array '%s' - already declared\n", name);
        return -1;  /* Error: variable already exists */
    }

    /* Add new symbol entry */
    symtab.vars[symtab.count].name = strdup(name);
    symtab.vars[symtab.count].type = strdup(type);
    symtab.vars[symtab.count].offset = symtab.nextOffset;
    symtab.vars[symtab.count].isArray = 1;
    symtab.vars[symtab.count].arraySize = size;

    /* Advance offset by size * 4 bytes (size of int in MIPS) */
    symtab.nextOffset += size * 4;
    symtab.count++;

    printf("SYMBOL TABLE: Added array '%s' of size %d at offset %d\n", name, size, symtab.vars[symtab.count - 1].offset);
    printSymTab();

    /* Return the offset for this array */
    return symtab.vars[symtab.count - 1].offset;
}

int isArrayVar(char* name) {
    for (int i = 0; i < symtab.count; i++) {
        if (strcmp(symtab.vars[i].name, name) == 0) {
            return symtab.vars[i].isArray;  /* Return 1 if array, 0 if not */
        }
    }
    return 0;  /* Not found */
}

int getArraySize(char* name) {
    for (int i = 0; i < symtab.count; i++) {
        if (strcmp(symtab.vars[i].name, name) == 0) {
            return symtab.vars[i].arraySize;
        }
    }
    return 0;  /* Not found */
}

/* Print current symbol table contents for debugging/tracing */
void printSymTab() {
    printf("\n=== SYMBOL TABLE STATE ===\n");
    printf("Count: %d, Next Offset: %d\n", symtab.count, symtab.nextOffset);
    if (symtab.count == 0) {
        printf("(empty)\n");
    } else {
        printf("Variables:\n");
        for (int i = 0; i < symtab.count; i++) {
            if (symtab.vars[i].isArray) {
                printf("  [%d] %s[%d] (%s) -> offset %d\n", i, symtab.vars[i].name, symtab.vars[i].arraySize, symtab.vars[i].type, symtab.vars[i].offset);
            } else {
                printf("  [%d] %s (%s) -> offset %d\n", i, symtab.vars[i].name, symtab.vars[i].type, symtab.vars[i].offset);
            }
        }
    }
    printf("==========================\n\n");
}

unsigned int hash(const char* str) {
    unsigned int hash = 5381;
    int c;
    while ((c = *str++))
        hash = ((hash << 5) + hash) + c; // hash * 33 + c
    return hash % HASH_SIZE;
}
