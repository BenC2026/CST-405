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
int addVar(char* name) {
    /* Check for duplicate declaration */
    if (isVarDeclared(name)) {
        printf("SYMBOL TABLE: Failed to add '%s' - already declared\n", name);
        return -1;  /* Error: variable already exists */
    }

    /* Add new symbol entry */
    symtab.vars[symtab.count].name = strdup(name);
    symtab.vars[symtab.count].offset = symtab.nextOffset;

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

/* Check if a variable has been declared */
int isVarDeclared(char* name) {
    return getVarOffset(name) != -1;  /* True if found, false otherwise */
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
            printf("  [%d] %s -> offset %d\n", i, symtab.vars[i].name, symtab.vars[i].offset);
        }
    }
    printf("==========================\n\n");
}