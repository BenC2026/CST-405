#ifndef CODEGEN_H
#define CODEGEN_H

#include "ast.h"
#include "tac.h"

/* REGISTER ALLOCATION
 * Manages MIPS temporary registers ($t0-$t9)
 * Implements register allocation with spilling
 */

#define NUM_TEMP_REGS 10  /* $t0-$t9 */
#define MAX_VAR_NAME 64

typedef struct {
    char varName[MAX_VAR_NAME];  /* Variable/temp currently in register */
    int isDirty;                  /* 1 if value differs from memory */
    int inUse;                    /* 1 if register is allocated */
    int lastUsed;                 /* Timestamp for LRU replacement */
} RegisterDescriptor;

typedef struct {
    RegisterDescriptor regs[NUM_TEMP_REGS];
    int timestamp;                /* Current timestamp for LRU */
    int spillCount;               /* Number of spills performed */
} RegisterAllocator;

/* Register allocation functions */
void initRegAlloc();                         /* Initialize register allocator */
int allocReg(const char* varName);           /* Allocate register for variable */
void freeReg(int regNum);                    /* Free a register */
int findVarReg(const char* varName);         /* Find register holding variable */
void spillReg(int regNum);                   /* Spill register to memory */
int selectVictimReg();                       /* Choose register to spill (LRU) */
void printRegAllocStats();                   /* Print allocation statistics */

void generateMIPS(ASTNode* root, const char* filename);
void generateMIPSFromTAC(const char* filename);  /* Generate MIPS from optimized TAC */

#endif
