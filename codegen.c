#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "codegen.h"
#include "symtab.h"

typedef struct {
    char* reg_name;
    char* var_name;  // Variable currently in this register
    int is_free;
    int last_used;   // For LRU eviction
} Register;

typedef struct {
    Register temp_regs[8];  // $t0-$t7
    Register saved_regs[8]; // $s0-$s7
    int clock;              // For LRU tracking
} RegisterAlligator;

RegisterAlligator reg_alloc;

FILE* output;
int tempReg = 0;
int condReg = 0;  // Register used for condition evaluations
RegisterAllocator regAlloc;
static int argIndex = 0;    // Track argument position for multi-arg calls
static int paramIndex = 0;  // Track parameter position for multi-param functions
#define ARG_AREA_OFFSET 300  // Arguments stored at offsets 300+ in caller's frame

/* FLOAT LITERAL TABLE
 * Collected in a pre-pass so .data float entries can be emitted before .text.
 * Identical values share one label so float equality comparisons work.
 */
#define MAX_FLOAT_LITS 256
typedef struct {
    char*  tempName;  /* TAC result temp          */
    char*  label;     /* .data label, e.g. "_flit0" */
    double value;     /* numeric value            */
    int    isDup;     /* 1 = reuses an earlier label */
} FloatLitEntry;
static FloatLitEntry floatLitTable[MAX_FLOAT_LITS];
static int numFloatLits = 0;

static void collectFloatLiterals(TACInstr* head) {
    int uniqueCount = 0;
    numFloatLits = 0;
    for (TACInstr* c = head; c; c = c->next) {
        if (c->op == TAC_FLOAT_ASSIGN && numFloatLits < MAX_FLOAT_LITS) {
            double val = atof(c->arg1);
            const char* existingLabel = NULL;
            for (int i = 0; i < numFloatLits; i++) {
                if (floatLitTable[i].value == val) {
                    existingLabel = floatLitTable[i].label;
                    break;
                }
            }
            if (existingLabel) {
                floatLitTable[numFloatLits].tempName = c->result;
                floatLitTable[numFloatLits].label    = strdup(existingLabel);
                floatLitTable[numFloatLits].value    = val;
                floatLitTable[numFloatLits].isDup    = 1;
            } else {
                char label[32];
                sprintf(label, "_flit%d", uniqueCount++);
                floatLitTable[numFloatLits].tempName = c->result;
                floatLitTable[numFloatLits].label    = strdup(label);
                floatLitTable[numFloatLits].value    = val;
                floatLitTable[numFloatLits].isDup    = 0;
            }
            numFloatLits++;
        }
    }
}

static const char* lookupFloatLabel(const char* tempName) {
    for (int i = 0; i < numFloatLits; i++)
        if (floatLitTable[i].tempName && strcmp(floatLitTable[i].tempName, tempName) == 0)
            return floatLitTable[i].label;
    return "_flit_unknown";
}

/* Look up float label by the literal value string — avoids temp-reuse aliasing */
static const char* lookupFloatLabelByValue(const char* valStr) {
    double val = atof(valStr);
    for (int i = 0; i < numFloatLits; i++)
        if (!floatLitTable[i].isDup && floatLitTable[i].value == val)
            return floatLitTable[i].label;
    return "_flit_unknown";
}

/* FPU REGISTER ALLOCATOR — $f4 .. $f11 (8 single-precision registers) */
#define NUM_FPU_REGS 8   /* indices 0-7, mapped to $f(i+4) */
typedef struct {
    char varName[MAX_VAR_NAME];
    int  inUse;
    int  lastUsed;
} FPURegDesc;
static FPURegDesc fpuRegs[NUM_FPU_REGS];
static int fpuTimestamp = 0;

static void initFPURegs() {
    for (int i = 0; i < NUM_FPU_REGS; i++) {
        fpuRegs[i].varName[0] = '\0';
        fpuRegs[i].inUse      = 0;
        fpuRegs[i].lastUsed   = 0;
    }
}

/* Returns physical $fN number, or -1 if not found */
static int findFPUReg(const char* varName) {
    for (int i = 0; i < NUM_FPU_REGS; i++) {
        if (fpuRegs[i].inUse && strcmp(fpuRegs[i].varName, varName) == 0) {
            fpuRegs[i].lastUsed = fpuTimestamp++;
            return i + 4;
        }
    }
    return -1;
}

/* Allocate an FPU register for varName; spills LRU if all busy */
static int allocFPUReg(const char* varName) {
    int existing = findFPUReg(varName);
    if (existing != -1) return existing;

    /* Find a free slot */
    for (int i = 0; i < NUM_FPU_REGS; i++) {
        if (!fpuRegs[i].inUse) {
            fpuRegs[i].inUse    = 1;
            fpuRegs[i].lastUsed = fpuTimestamp++;
            strncpy(fpuRegs[i].varName, varName, MAX_VAR_NAME - 1);
            return i + 4;
        }
    }

    /* Spill LRU */
    int victim = 0;
    for (int i = 1; i < NUM_FPU_REGS; i++)
        if (fpuRegs[i].lastUsed < fpuRegs[victim].lastUsed) victim = i;

    int victimReg = victim + 4;
    int offset = getVarOffset(fpuRegs[victim].varName);
    if (offset != -1)
        fprintf(output, "    s.s $f%d, %d($sp)    # spill float %s\n",
                victimReg, offset, fpuRegs[victim].varName);

    fpuRegs[victim].inUse    = 1;
    fpuRegs[victim].lastUsed = fpuTimestamp++;
    strncpy(fpuRegs[victim].varName, varName, MAX_VAR_NAME - 1);
    return victimReg;
}

static void freeFPUReg(int regNum) {
    int idx = regNum - 4;
    if (idx >= 0 && idx < NUM_FPU_REGS) {
        fpuRegs[idx].inUse      = 0;
        fpuRegs[idx].varName[0] = '\0';
    }
}

/* Returns 1 if name is a declared float variable */
static int isFloatVar(const char* name) {
    char* t = getVarType((char*)name);
    return t != NULL && strcmp(t, "float") == 0;
}

/* Returns 1 if name currently lives in an FPU register */
static int isFloatTemp(const char* name) {
    return findFPUReg(name) != -1;
}

/* STRING LITERAL TABLE
 * Collected in a pre-pass so .data entries can be emitted before .text.
 * Each TAC_STRING_ASSIGN result temp maps to a _strN label.
 */
#define MAX_STR_LITERALS 256
typedef struct {
    char* tempName;  /* TAC result, e.g. "t3"        */
    char* label;     /* assembly label, e.g. "_str0"  */
    char* text;      /* raw literal text              */
    int   isDup;     /* 1 = shares label with earlier entry, no .data line emitted */
} StrEntry;
static StrEntry strTable[MAX_STR_LITERALS];
static int numStrLiterals = 0;

/* Pre-pass: walk TAC list and record every TAC_STRING_ASSIGN.
 * Identical string texts reuse the same _strN label so that
 * address-equality comparisons (==) work correctly. */
static void collectStringLiterals(TACInstr* head) {
    int uniqueCount = 0;
    numStrLiterals = 0;
    for (TACInstr* c = head; c; c = c->next) {
        if (c->op == TAC_STRING_ASSIGN && numStrLiterals < MAX_STR_LITERALS) {
            /* Look for an earlier entry with the same text */
            const char* existingLabel = NULL;
            for (int i = 0; i < numStrLiterals; i++) {
                if (strcmp(strTable[i].text, c->arg1) == 0) {
                    existingLabel = strTable[i].label;
                    break;
                }
            }
            if (existingLabel) {
                /* Duplicate: point this temp at the same label */
                strTable[numStrLiterals].tempName = c->result;
                strTable[numStrLiterals].label    = strdup(existingLabel);
                strTable[numStrLiterals].text     = c->arg1;
                strTable[numStrLiterals].isDup    = 1;
            } else {
                /* New unique literal */
                char label[32];
                sprintf(label, "_str%d", uniqueCount++);
                strTable[numStrLiterals].tempName = c->result;
                strTable[numStrLiterals].label    = strdup(label);
                strTable[numStrLiterals].text     = c->arg1;
                strTable[numStrLiterals].isDup    = 0;
            }
            numStrLiterals++;
        }
    }
}

/* Look up the .data label assigned to a string-literal TAC temp */
static const char* lookupStrLabel(const char* tempName) {
    for (int i = 0; i < numStrLiterals; i++) {
        if (strTable[i].tempName && strcmp(strTable[i].tempName, tempName) == 0)
            return strTable[i].label;
    }
    return "_str_unknown";
}

int getNextTemp() {
    int reg = tempReg++;
    if (tempReg > 7) tempReg = 0;  // Reuse $t0-$t7
    return reg;
}

/* REGISTER ALLOCATION ALGORITHM
 * Implements a graph-coloring inspired approach with LRU spilling
 * Key features:
 * 1. Tracks which variables/temporaries are in which registers
 * 2. Detects dirty registers (modified values)
 * 3. Uses LRU (Least Recently Used) for spill victim selection
 * 4. Automatically spills to memory when out of registers
 */

/* Initialize register allocator */
void initRegAlloc() {
    for (int i = 0; i < NUM_TEMP_REGS; i++) {
        regAlloc.regs[i].varName[0] = '\0';
        regAlloc.regs[i].isDirty = 0;
        regAlloc.regs[i].inUse = 0;
        regAlloc.regs[i].lastUsed = 0;
    }
    regAlloc.timestamp = 0;
    regAlloc.spillCount = 0;
    printf("MIPS: Register allocator initialized (10 temp registers: $t0-$t9)\n\n");
}

/* Find which register holds a variable (return -1 if not in register) */
int findVarReg(const char* varName) {
    for (int i = 0; i < NUM_TEMP_REGS; i++) {
        if (regAlloc.regs[i].inUse &&
            strcmp(regAlloc.regs[i].varName, varName) == 0) {
            regAlloc.regs[i].lastUsed = regAlloc.timestamp++;
            return i;
        }
    }
    return -1;
}

/* Select victim register for spilling using LRU */
int selectVictimReg() {
    int victim = 0;
    int oldestTime = regAlloc.regs[0].lastUsed;

    for (int i = 1; i < NUM_TEMP_REGS; i++) {
        if (regAlloc.regs[i].lastUsed < oldestTime) {
            oldestTime = regAlloc.regs[i].lastUsed;
            victim = i;
        }
    }

    return victim;
}

/* Spill a register to memory */
void spillReg(int regNum) {
    if (!regAlloc.regs[regNum].inUse) {
        return;  /* Nothing to spill */
    }

    /* Only spill if dirty (value was modified) */
    if (regAlloc.regs[regNum].isDirty) {
        int offset = getVarOffset(regAlloc.regs[regNum].varName);
        if (offset != -1) {
            fprintf(output, "    # Spilling $t%d (%s) to memory\n", regNum,
                    regAlloc.regs[regNum].varName);
            fprintf(output, "    sw $t%d, %d($sp)\n", regNum, offset);
            regAlloc.spillCount++;
        }
    }

    /* Mark register as free */
    regAlloc.regs[regNum].inUse = 0;
    regAlloc.regs[regNum].isDirty = 0;
    regAlloc.regs[regNum].varName[0] = '\0';
}

/* Allocate a register for a variable */
int allocReg(const char* varName) {
    /* Check if variable is already in a register */
    int existingReg = findVarReg(varName);
    if (existingReg != -1) {
        printf("    [REG ALLOC] Variable '%s' already in $t%d\n", varName, existingReg);
        return existingReg;
    }

    /* Find a free register */
    for (int i = 0; i < NUM_TEMP_REGS; i++) {
        if (!regAlloc.regs[i].inUse) {
            regAlloc.regs[i].inUse = 1;
            strncpy(regAlloc.regs[i].varName, varName, MAX_VAR_NAME - 1);
            regAlloc.regs[i].isDirty = 0;
            regAlloc.regs[i].lastUsed = regAlloc.timestamp++;
            printf("    [REG ALLOC] Allocated $t%d for '%s'\n", i, varName);
            return i;
        }
    }

    /* No free registers - must spill using LRU */
    int victim = selectVictimReg();
    printf("    [REG ALLOC] All registers full, spilling $t%d (%s)\n",
           victim, regAlloc.regs[victim].varName);
    spillReg(victim);

    /* Allocate the freed register */
    regAlloc.regs[victim].inUse = 1;
    strncpy(regAlloc.regs[victim].varName, varName, MAX_VAR_NAME - 1);
    regAlloc.regs[victim].isDirty = 0;
    regAlloc.regs[victim].lastUsed = regAlloc.timestamp++;
    printf("    [REG ALLOC] Allocated $t%d for '%s'\n", victim, varName);
    return victim;
}

/* Free a register */
void freeReg(int regNum) {
    if (regNum >= 0 && regNum < NUM_TEMP_REGS) {
        /* Spill if dirty before freeing */
        if (regAlloc.regs[regNum].isDirty) {
            spillReg(regNum);
        }
        regAlloc.regs[regNum].inUse = 0;
        regAlloc.regs[regNum].varName[0] = '\0';
        printf("    [REG ALLOC] Freed $t%d\n", regNum);
    }
}

/* Print register allocation statistics */
void printRegAllocStats() {
    printf("\n┌──────────────────────────────────────────────────────────┐\n");
    printf("│ REGISTER ALLOCATION STATISTICS                           │\n");
    printf("├──────────────────────────────────────────────────────────┤\n");
    printf("│ Total registers spilled:     %3d                         │\n", regAlloc.spillCount);
    printf("│ Final register usage:                                    │\n");

    int inUseCount = 0;
    for (int i = 0; i < NUM_TEMP_REGS; i++) {
        if (regAlloc.regs[i].inUse) {
            printf("│   $t%-2d: %-20s%s                      │\n",
                   i, regAlloc.regs[i].varName,
                   regAlloc.regs[i].isDirty ? " (dirty)" : "");
            inUseCount++;
        }
    }

    if (inUseCount == 0) {
        printf("│   (all registers free)                                   │\n");
    }

    printf("└──────────────────────────────────────────────────────────┘\n\n");
}

/* Generate expression code with register allocation */
int genExpr(ASTNode* node) {
    if (!node) return -1;

    switch(node->type) {
        case NODE_NUM: {
            /* Allocate register for constant */
            char tempName[32];
            sprintf(tempName, "const_%d", node->data.num);
            int reg = allocReg(tempName);
            fprintf(output, "    li $t%d, %d         # Load constant %d\n",
                    reg, node->data.num, node->data.num);
            return reg;
        }

        case NODE_VAR: {
            int offset = getVarOffset(node->data.decl.name);
            if (offset == -1) {
                fprintf(stderr, "Error: Variable %s not declared\n", node->data.decl.name);
                exit(1);
            }

            /* Check if variable is already in a register */
            int reg = findVarReg(node->data.decl.name);
            if (reg == -1) {
                /* Not in register, allocate one and load from memory */
                reg = allocReg(node->data.decl.name);
                fprintf(output, "    lw $t%d, %d($sp)     # Load variable '%s'\n",
                        reg, offset, node->data.decl.name);
            }
            return reg;
        }

        case NODE_BINOP: {
            /* Generate left operand */
            int leftReg = genExpr(node->data.binop.left);

            /* Generate right operand */
            int rightReg = genExpr(node->data.binop.right);

            /* Perform operation, store result in leftReg */
            fprintf(output, "    add $t%d, $t%d, $t%d   # Binary operation\n",
                    leftReg, leftReg, rightReg);

            /* Mark result register as dirty */
            regAlloc.regs[leftReg].isDirty = 1;

            /* Free the right register as it's no longer needed */
            freeReg(rightReg);

            return leftReg;
        }

        case NODE_ARRAY_ACCESS: {
            /* TODO: Generate code for array access */
            if (!isArrayVar(node->data.array_access.name)) {
                fprintf(stderr, "Error: %s is not an array\n", node->data.array_access.name);
                exit(1);
            }

            /* Generate code for index expression */
            genExpr(node->data.array_access.index);
            int indexReg = tempReg - 1;

            /* Calculate array element address and load value */
            int baseOffset = getVarOffset(node->data.array_access.name);
            int resultReg = getNextTemp();

            fprintf(output, "    # Array access: %s[index]\n", node->data.array_access.name);
            fprintf(output, "    sll $t%d, $t%d, 2    # index * 4\n", indexReg, indexReg);
            fprintf(output, "    addi $t%d, $sp, %d   # base address\n", resultReg, baseOffset);
            fprintf(output, "    add $t%d, $t%d, $t%d # element address\n", resultReg, resultReg, indexReg);
            fprintf(output, "    lw $t%d, 0($t%d)     # load value\n", resultReg, resultReg);
            break;
        }

        default:
            return -1;
    }
}

void genStmt(ASTNode* node) {
    if (!node) return;

    switch(node->type) {
        case NODE_DECL: {
            int offset = addVar(node->data.decl.name, node->data.decl.type);
            if (offset == -1) {
                fprintf(stderr, "Error: Variable %s already declared\n", node->data.decl.name);
                exit(1);
            }
            fprintf(output, "    # Declared '%s' at offset %d\n", node->data.decl.name, offset);
            break;
        }

        case NODE_ASSIGN: {
            int offset = getVarOffset(node->data.assign.var);
            if (offset == -1) {
                fprintf(stderr, "Error: Variable %s not declared\n", node->data.assign.var);
                exit(1);
            }

            /* Generate expression and get result register */
            int reg = genExpr(node->data.assign.value);

            /* Store to variable's memory location */
            fprintf(output, "    sw $t%d, %d($sp)     # Assign to '%s'\n",
                    reg, offset, node->data.assign.var);

            /* Update register descriptor - this register now holds the variable */
            strncpy(regAlloc.regs[reg].varName, node->data.assign.var, MAX_VAR_NAME - 1);
            regAlloc.regs[reg].isDirty = 0;  /* No longer dirty after store */

            break;
        }

        case NODE_PRINT: {
            /* Generate expression and get result register */
            int reg = genExpr(node->data.expr);

            /* Print the value */
            fprintf(output, "    # Print integer\n");
            fprintf(output, "    move $a0, $t%d\n", reg);
            fprintf(output, "    li $v0, 1\n");
            fprintf(output, "    syscall\n");

            /* Print newline */
            fprintf(output, "    # Print newline\n");
            fprintf(output, "    li $v0, 11\n");
            fprintf(output, "    li $a0, 10\n");
            fprintf(output, "    syscall\n");

            /* Free register after print */
            freeReg(reg);
            break;
        }

        case NODE_STMT_LIST:
            genStmt(node->data.stmtlist.stmt);
            genStmt(node->data.stmtlist.next);
            break;

        case NODE_ARRAY_DECL: {
            /* TODO: Generate code for array declaration */
            int offset = addArrayVar(node->data.array_decl.name, node->data.array_decl.type, node->data.array_decl.size);
            if (offset == -1) {
                fprintf(stderr, "Error: Array %s already declared\n", node->data.array_decl.name);
                exit(1);
            }
            fprintf(output, "    # Declared array %s[%d] at offset %d\n", node->data.array_decl.name, node->data.array_decl.size, offset);
            break;
        }

        case NODE_ARRAY_ASSIGN: {
            /* TODO: Generate code for array assignment */
            if (!isArrayVar(node->data.array_assign.name)) {
                fprintf(stderr, "Error: %s is not an array\n", node->data.array_assign.name);
                exit(1);
            }

            /* Generate code for index expression */
            genExpr(node->data.array_assign.index);
            int indexReg = tempReg - 1;

            /* Generate code for value expression */
            genExpr(node->data.array_assign.value);
            int valueReg = tempReg - 1;

            /* Calculate array element address */
            int baseOffset = getVarOffset(node->data.array_assign.name);
            fprintf(output, "    # Array assignment: %s[index] = value\n", node->data.array_assign.name);
            fprintf(output, "    sll $t%d, $t%d, 2    # index * 4\n", indexReg, indexReg);
            fprintf(output, "    addi $t%d, $sp, %d   # base address\n", indexReg, baseOffset);
            fprintf(output, "    add $t%d, $t%d, $t%d # element address\n", indexReg, indexReg, indexReg);
            fprintf(output, "    sw $t%d, 0($t%d)     # store value\n", valueReg, indexReg);
            tempReg = 0;
            break;
        }

        default:
            break;
    }
}

void generateMIPS(ASTNode* root, const char* filename) {
    output = fopen(filename, "w");
    if (!output) {
        fprintf(stderr, "Cannot open output file %s\n", filename);
        exit(1);
    }

    // Initialize symbol table and register allocator
    initSymTab();
    initRegAlloc();

    // MIPS program header
    fprintf(output, ".data\n");
    fprintf(output, "_heap_ptr:\t.word _heap_mem\n");
    fprintf(output, "_heap_mem:\t.space 4096\n");
    fprintf(output, "\n.text\n");
    fprintf(output, ".globl main\n");
    fprintf(output, "main:\n");

    // Allocate stack space (max 100 variables * 4 bytes)
    fprintf(output, "    # Allocate stack space\n");
    fprintf(output, "    addi $sp, $sp, -400\n\n");

    // Generate code for statements
    genStmt(root);

    // Spill any remaining dirty registers
    fprintf(output, "\n    # Spill any remaining registers\n");
    for (int i = 0; i < NUM_TEMP_REGS; i++) {
        if (regAlloc.regs[i].inUse && regAlloc.regs[i].isDirty) {
            spillReg(i);
        }
    }

    // Program exit
    fprintf(output, "\n    # Exit program\n");
    fprintf(output, "    addi $sp, $sp, 400\n");
    fprintf(output, "    li $v0, 10\n");
    fprintf(output, "    syscall\n");

    fclose(output);

    // Print register allocation statistics
    printRegAllocStats();
}

/* Helper function to check if a string is a TAC temporary (t0, t1, etc.) */
static int isTACTemp(const char* name) {
    return (name && name[0] == 't' && name[1] >= '0' && name[1] <= '9');
}

/* Helper function to check if a string is a constant */
static int isConstant(const char* str) {
    if (!str || str[0] == '\0') return 0;
    int i = 0;
    if (str[i] == '-' || str[i] == '+') i++;  /* Handle sign */
    while (str[i] != '\0') {
        if (str[i] < '0' || str[i] > '9') return 0;
        i++;
    }
    return 1;
}

/* GENERATE MIPS CODE FROM OPTIMIZED TAC
 * This function consumes the optimized TAC and produces MIPS assembly
 * Key advantages:
 * 1. Uses optimized TAC (constant folding, copy propagation already done)
 * 2. Simpler code generation logic (TAC is already linearized)
 * 3. Better code quality due to optimizations
 */
void generateMIPSFromTAC(const char* filename) {
    output = fopen(filename, "w");
    if (!output) {
        fprintf(stderr, "Cannot open output file %s\n", filename);
        exit(1);
    }

    // Initialize symbol table and register allocator
    initSymTab();
    initRegAlloc();

    // Get optimized TAC list
    TACList* tac = getOptimizedTAC();
    if (!tac || !tac->head) {
        fprintf(stderr, "Error: No optimized TAC available\n");
        fclose(output);
        return;
    }

    // Pre-pass: collect all string and float literals so .data can be emitted first
    collectStringLiterals(tac->head);
    collectFloatLiterals(tac->head);
    initFPURegs();

    // MIPS program header — .data section
    fprintf(output, ".data\n");
    for (int i = 0; i < numStrLiterals; i++) {
        if (!strTable[i].isDup)
            fprintf(output, "%s:\t.asciiz \"%s\"\n",
                    strTable[i].label, strTable[i].text);
    }
    for (int i = 0; i < numFloatLits; i++) {
        if (!floatLitTable[i].isDup) {
            char fbuf[32];
            snprintf(fbuf, sizeof(fbuf), "%g", floatLitTable[i].value);
            /* SPIM requires a decimal point in .float values (e.g. 2.0 not 2) */
            if (!strchr(fbuf, '.') && !strchr(fbuf, 'e') && !strchr(fbuf, 'E'))
                strncat(fbuf, ".0", sizeof(fbuf) - strlen(fbuf) - 1);
            fprintf(output, "%s:\t.float %s\n", floatLitTable[i].label, fbuf);
        }
    }
    fprintf(output, "_heap_ptr:\t.word _heap_mem\n");
    fprintf(output, "_heap_mem:\t.space 4096\n");

    fprintf(output, "\n.text\n");
    fprintf(output, ".globl main\n");
    fprintf(output, "    j main\n\n");

    /* ── __print_float subroutine ──────────────────────────────────────────
     * Input:  $f12 (single-precision float to print)
     * Prints the value with trailing zeros trimmed (e.g. 5.4 not 5.40000010)
     * Clobbers: $t0-$t4, $f12-$f15, $v0, $a0
     * Preserves: $ra (saved/restored on stack)
     */
    fprintf(output, "__print_float:\n");
    fprintf(output, "    addi $sp, $sp, -4\n");
    fprintf(output, "    sw $ra, 0($sp)\n");
    fprintf(output, "    mtc1 $zero, $f15\n");
    fprintf(output, "    c.lt.s $f12, $f15\n");
    fprintf(output, "    bc1f __pfl_positive\n");
    fprintf(output, "    neg.s $f12, $f12\n");
    fprintf(output, "    li $v0, 11\n");
    fprintf(output, "    li $a0, 45\n");
    fprintf(output, "    syscall\n");
    fprintf(output, "__pfl_positive:\n");
    fprintf(output, "    trunc.w.s $f13, $f12\n");
    fprintf(output, "    mfc1 $a0, $f13\n");
    fprintf(output, "    li $v0, 1\n");
    fprintf(output, "    syscall\n");
    fprintf(output, "    cvt.s.w $f13, $f13\n");
    fprintf(output, "    sub.s $f12, $f12, $f13\n");
    fprintf(output, "    li $t0, 1000000\n");
    fprintf(output, "    mtc1 $t0, $f13\n");
    fprintf(output, "    cvt.s.w $f13, $f13\n");
    fprintf(output, "    mul.s $f12, $f12, $f13\n");
    fprintf(output, "    li $t0, 0x3F000000\n");
    fprintf(output, "    mtc1 $t0, $f13\n");
    fprintf(output, "    add.s $f12, $f12, $f13\n");
    fprintf(output, "    trunc.w.s $f12, $f12\n");
    fprintf(output, "    mfc1 $t0, $f12\n");
    fprintf(output, "    beqz $t0, __pfl_done\n");
    fprintf(output, "    addi $sp, $sp, -8\n");
    fprintf(output, "    li $t1, 6\n");
    fprintf(output, "    add $t2, $sp, 5\n");
    fprintf(output, "__pfl_fill:\n");
    fprintf(output, "    li $t3, 10\n");
    fprintf(output, "    div $t0, $t3\n");
    fprintf(output, "    mfhi $t4\n");
    fprintf(output, "    addi $t4, $t4, 48\n");
    fprintf(output, "    sb $t4, 0($t2)\n");
    fprintf(output, "    mflo $t0\n");
    fprintf(output, "    addi $t2, $t2, -1\n");
    fprintf(output, "    addi $t1, $t1, -1\n");
    fprintf(output, "    bnez $t1, __pfl_fill\n");
    fprintf(output, "    add $t2, $sp, 5\n");
    fprintf(output, "    li $t1, 6\n");
    fprintf(output, "__pfl_trim:\n");
    fprintf(output, "    lb $t3, 0($t2)\n");
    fprintf(output, "    bne $t3, 48, __pfl_print\n");
    fprintf(output, "    addi $t2, $t2, -1\n");
    fprintf(output, "    addi $t1, $t1, -1\n");
    fprintf(output, "    bnez $t1, __pfl_trim\n");
    fprintf(output, "    addi $sp, $sp, 8\n");
    fprintf(output, "    j __pfl_done\n");
    fprintf(output, "__pfl_print:\n");
    fprintf(output, "    li $v0, 11\n");
    fprintf(output, "    li $a0, 46\n");
    fprintf(output, "    syscall\n");
    fprintf(output, "    move $t2, $sp\n");
    fprintf(output, "__pfl_loop:\n");
    fprintf(output, "    beqz $t1, __pfl_end\n");
    fprintf(output, "    lb $a0, 0($t2)\n");
    fprintf(output, "    li $v0, 11\n");
    fprintf(output, "    syscall\n");
    fprintf(output, "    addi $t2, $t2, 1\n");
    fprintf(output, "    addi $t1, $t1, -1\n");
    fprintf(output, "    j __pfl_loop\n");
    fprintf(output, "__pfl_end:\n");
    fprintf(output, "    addi $sp, $sp, 8\n");
    fprintf(output, "__pfl_done:\n");
    fprintf(output, "    lw $ra, 0($sp)\n");
    fprintf(output, "    addi $sp, $sp, 4\n");
    fprintf(output, "    jr $ra\n\n");

    // Track current function for return code generation
    static char currentFuncName[64] = "";

    // Process each TAC instruction
    TACInstr* curr = tac->head;
    while (curr) {
        switch(curr->op) {
            case TAC_FUNC_BEGIN: {
                // Function begin - reset symbol table for new function scope
                initSymTab();
                initRegAlloc();
                argIndex = 0;
                paramIndex = 0;
                strncpy(currentFuncName, curr->result, sizeof(currentFuncName) - 1);

                fprintf(output, "\n# Function: %s\n", curr->result);
                fprintf(output, "%s:\n", curr->result);
                fprintf(output, "    addi $sp, $sp, -400\n");
                if (strcmp(curr->result, "main") != 0) {
                    fprintf(output, "    sw $ra, 396($sp)\n");
                }
                break;
            }

            case TAC_FUNC_END: {
                // Function end
                fprintf(output, "    # End of function %s\n", curr->result);
                if (strcmp(curr->result, "main") == 0) {
                    // Main exits the program
                    fprintf(output, "    addi $sp, $sp, 400\n");
                    fprintf(output, "    li $v0, 10\n");
                    fprintf(output, "    syscall\n");
                } else {
                    fprintf(output, "    lw $ra, 396($sp)\n");
                    fprintf(output, "    addi $sp, $sp, 400\n");
                    fprintf(output, "    jr $ra\n");
                }
                break;
            }

            case TAC_PARAM: {
                // Function parameter - add to symbol table
                int offset = addVar(curr->result, curr->type);
                if (offset == -1) {
                    fprintf(stderr, "Error: Parameter %s already declared\n", curr->result);
                    exit(1);
                }
                // Load parameter from caller's argument area on the stack
                // Caller stored args at ARG_AREA_OFFSET + i*4 in its frame
                // After callee's addi $sp, $sp, -400, caller's frame is at $sp+400
                // So args are at $sp + 400 + ARG_AREA_OFFSET + paramIndex*4
                int callerArgOffset = 400 + ARG_AREA_OFFSET + paramIndex * 4;
                int reg = allocReg(curr->result);
                fprintf(output, "    lw $t%d, %d($sp)      # Load parameter '%s' from caller arg %d\n",
                        reg, callerArgOffset, curr->result, paramIndex);
                fprintf(output, "    sw $t%d, %d($sp)      # Store parameter '%s'\n",
                        reg, offset, curr->result);
                paramIndex++;
                break;
            }

            case TAC_DECL: {
                // Declare variable in symbol table
                int offset = addVar(curr->result, curr->type);
                if (offset == -1) {
                    fprintf(stderr, "Error: Variable %s already declared\n", curr->result);
                    exit(1);
                }
                fprintf(output, "    # Declared '%s' of type '%s' at offset %d\n", curr->result, curr->type, offset);
                break;
            }

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
            case TAC_AND:
            case TAC_OR: {
                // Float arithmetic path (TAC_ADD/SUB/MUL/DIV only)
                int isFloatOp = curr->type && strcmp(curr->type, "float") == 0;
                if (isFloatOp && (curr->op == TAC_ADD || curr->op == TAC_SUB ||
                                  curr->op == TAC_MUL || curr->op == TAC_DIV)) {
                    // Load arg1 into FPU register
                    int f1;
                    if (isTACTemp(curr->arg1)) {
                        f1 = findFPUReg(curr->arg1);
                        if (f1 == -1) { fprintf(stderr,"Error: float temp %s not in FPU\n",curr->arg1); exit(1); }
                    } else {
                        int off = getVarOffset(curr->arg1);
                        f1 = allocFPUReg(curr->arg1);
                        fprintf(output, "    l.s $f%d, %d($sp)    # load float %s\n", f1, off, curr->arg1);
                    }
                    // Load arg2 into FPU register
                    int f2;
                    if (isTACTemp(curr->arg2)) {
                        f2 = findFPUReg(curr->arg2);
                        if (f2 == -1) { fprintf(stderr,"Error: float temp %s not in FPU\n",curr->arg2); exit(1); }
                    } else {
                        int off = getVarOffset(curr->arg2);
                        f2 = allocFPUReg(curr->arg2);
                        fprintf(output, "    l.s $f%d, %d($sp)    # load float %s\n", f2, off, curr->arg2);
                    }
                    int fResult = allocFPUReg(curr->result);
                    switch (curr->op) {
                        case TAC_ADD: fprintf(output,"    add.s $f%d,$f%d,$f%d  # %s=%s+%s\n",fResult,f1,f2,curr->result,curr->arg1,curr->arg2); break;
                        case TAC_SUB: fprintf(output,"    sub.s $f%d,$f%d,$f%d  # %s=%s-%s\n",fResult,f1,f2,curr->result,curr->arg1,curr->arg2); break;
                        case TAC_MUL: fprintf(output,"    mul.s $f%d,$f%d,$f%d  # %s=%s*%s\n",fResult,f1,f2,curr->result,curr->arg1,curr->arg2); break;
                        case TAC_DIV: fprintf(output,"    div.s $f%d,$f%d,$f%d  # %s=%s/%s\n",fResult,f1,f2,curr->result,curr->arg1,curr->arg2); break;
                        default: break;
                    }
                    break;
                }

                // Integer arithmetic path
                int reg1, reg2, regResult;

                // Load arg1 into register
                if (isConstant(curr->arg1)) {
                    // arg1 is a constant
                    char tempName[32];
                    sprintf(tempName, "const_%s", curr->arg1);
                    reg1 = allocReg(tempName);
                    fprintf(output, "    li $t%d, %s         # Load constant %s\n",
                            reg1, curr->arg1, curr->arg1);
                } else if (isTACTemp(curr->arg1)) {
                    // TAC temporary - must already be in a register
                    reg1 = findVarReg(curr->arg1);
                    if (reg1 == -1) {
                        fprintf(stderr, "Error: TAC temporary %s not in register\n", curr->arg1);
                        exit(1);
                    }
                } else {
                    // Named variable - always reload from memory for loop correctness
                    int offset = getVarOffset(curr->arg1);
                    if (offset == -1) {
                        fprintf(stderr, "Error: Variable %s not declared\n", curr->arg1);
                        exit(1);
                    }
                    reg1 = allocReg(curr->arg1);
                    fprintf(output, "    lw $t%d, %d($sp)     # Load variable '%s'\n",
                            reg1, offset, curr->arg1);
                }

                // Load arg2 into register
                if (isConstant(curr->arg2)) {
                    // arg2 is a constant
                    char tempName[32];
                    sprintf(tempName, "const_%s", curr->arg2);
                    reg2 = allocReg(tempName);
                    fprintf(output, "    li $t%d, %s         # Load constant %s\n",
                            reg2, curr->arg2, curr->arg2);
                } else if (isTACTemp(curr->arg2)) {
                    // TAC temporary - must already be in a register
                    reg2 = findVarReg(curr->arg2);
                    if (reg2 == -1) {
                        fprintf(stderr, "Error: TAC temporary %s not in register\n", curr->arg2);
                        exit(1);
                    }
                } else {
                    // Named variable - always reload from memory for loop correctness
                    int offset = getVarOffset(curr->arg2);
                    if (offset == -1) {
                        fprintf(stderr, "Error: Variable %s not declared\n", curr->arg2);
                        exit(1);
                    }
                    reg2 = allocReg(curr->arg2);
                    fprintf(output, "    lw $t%d, %d($sp)     # Load variable '%s'\n",
                            reg2, offset, curr->arg2);
                }

                // Allocate register for result
                regResult = allocReg(curr->result);

                // Perform operation based on TAC operation type
                switch(curr->op) {
                    case TAC_ADD:
                        fprintf(output, "    add $t%d, $t%d, $t%d   # %s = %s + %s\n",
                                regResult, reg1, reg2, curr->result, curr->arg1, curr->arg2);
                        break;
                    case TAC_SUB:
                        fprintf(output, "    sub $t%d, $t%d, $t%d   # %s = %s - %s\n",
                                regResult, reg1, reg2, curr->result, curr->arg1, curr->arg2);
                        break;
                    case TAC_MUL:
                        fprintf(output, "    mul $t%d, $t%d, $t%d   # %s = %s * %s\n",
                                regResult, reg1, reg2, curr->result, curr->arg1, curr->arg2);
                        break;
                    case TAC_DIV:
                        fprintf(output, "    div $t%d, $t%d         # %s / %s\n",
                                reg1, reg2, curr->arg1, curr->arg2);
                        fprintf(output, "    mflo $t%d              # %s = quotient\n",
                                regResult, curr->result);
                        break;
                    case TAC_LT:
                        fprintf(output, "    slt $t%d, $t%d, $t%d   # %s = %s < %s\n",
                                regResult, reg1, reg2, curr->result, curr->arg1, curr->arg2);
                        break;
                    case TAC_GT:
                        fprintf(output, "    sgt $t%d, $t%d, $t%d   # %s = %s > %s\n",
                                regResult, reg1, reg2, curr->result, curr->arg1, curr->arg2);
                        break;
                    case TAC_LE:
                        fprintf(output, "    sle $t%d, $t%d, $t%d   # %s = %s <= %s\n",
                                regResult, reg1, reg2, curr->result, curr->arg1, curr->arg2);
                        break;
                    case TAC_GE:
                        fprintf(output, "    sge $t%d, $t%d, $t%d   # %s = %s >= %s\n",
                                regResult, reg1, reg2, curr->result, curr->arg1, curr->arg2);
                        break;
                    case TAC_EQ:
                        fprintf(output, "    seq $t%d, $t%d, $t%d   # %s = %s == %s\n",
                                regResult, reg1, reg2, curr->result, curr->arg1, curr->arg2);
                        break;
                    case TAC_NE:
                        fprintf(output, "    sne $t%d, $t%d, $t%d   # %s = %s != %s\n",
                                regResult, reg1, reg2, curr->result, curr->arg1, curr->arg2);
                        break;
                    case TAC_AND:
                        fprintf(output, "    and $t%d, $t%d, $t%d   # %s = %s and %s\n",
                                regResult, reg1, reg2, curr->result, curr->arg1, curr->arg2);
                        break;
                    case TAC_OR:
                        fprintf(output, "    or $t%d, $t%d, $t%d    # %s = %s or %s\n",
                                regResult, reg1, reg2, curr->result, curr->arg1, curr->arg2);
                        break;
                    default:
                        break;
                }

                // Mark result register as dirty (only if it's a real variable, not a TAC temp)
                if (!isTACTemp(curr->result)) {
                    regAlloc.regs[regResult].isDirty = 1;
                }

                // Free operand registers (unless they're still needed)

                break;
            }

            case TAC_NOT: {
                // result = !arg1 — logical NOT: 0 → 1, nonzero → 0
                int reg1, regResult;
                if (isConstant(curr->arg1)) {
                    char tempName[32];
                    sprintf(tempName, "const_%s", curr->arg1);
                    reg1 = allocReg(tempName);
                    fprintf(output, "    li $t%d, %s\n", reg1, curr->arg1);
                } else if (isTACTemp(curr->arg1)) {
                    reg1 = findVarReg(curr->arg1);
                    if (reg1 == -1) {
                        fprintf(stderr, "Error: TAC temporary %s not in register\n", curr->arg1);
                        exit(1);
                    }
                } else {
                    int offset = getVarOffset(curr->arg1);
                    reg1 = allocReg(curr->arg1);
                    fprintf(output, "    lw $t%d, %d($sp)    # load %s\n", reg1, offset, curr->arg1);
                }
                regResult = allocReg(curr->result);
                fprintf(output, "    seq $t%d, $t%d, $zero  # %s = not %s\n",
                        regResult, reg1, curr->result, curr->arg1);
                if (!isTACTemp(curr->result)) {
                    regAlloc.regs[regResult].isDirty = 1;
                }
                break;
            }

            case TAC_ASSIGN: {
                // result = arg1
                int isTempResult = isTACTemp(curr->result);
                int offset = -1;

                // Get offset for real variables (not TAC temporaries)
                if (!isTempResult) {
                    offset = getVarOffset(curr->result);
                    if (offset == -1) {
                        fprintf(stderr, "Error: Variable %s not declared\n", curr->result);
                        exit(1);
                    }
                }

                // Float assignment path
                if (!isTempResult && isFloatVar(curr->result)) {
                    int fsrc;
                    if (isTACTemp(curr->arg1) || isFloatTemp(curr->arg1)) {
                        fsrc = findFPUReg(curr->arg1);
                        if (fsrc == -1) { fprintf(stderr,"Error: float temp %s not in FPU\n",curr->arg1); exit(1); }
                    } else if (isFloatVar(curr->arg1)) {
                        int srcOff = getVarOffset(curr->arg1);
                        fsrc = allocFPUReg(curr->arg1);
                        fprintf(output, "    l.s $f%d, %d($sp)    # load float %s\n", fsrc, srcOff, curr->arg1);
                    } else {
                        fprintf(stderr, "Error: cannot assign non-float to float variable %s\n", curr->result);
                        exit(1);
                    }
                    fprintf(output, "    s.s $f%d, %d($sp)    # store float %s\n", fsrc, offset, curr->result);
                    break;
                }

                // Integer assignment path
                int regSrc, regDest;
                // Load source value into register
                if (isConstant(curr->arg1)) {
                    // Source is a constant
                    regDest = allocReg(curr->result);
                    fprintf(output, "    li $t%d, %s         # %s = %s (constant)\n",
                            regDest, curr->arg1, curr->result, curr->arg1);
                } else {
                    // Source is a variable or TAC temporary
                    regSrc = findVarReg(curr->arg1);
                    if (regSrc == -1) {
                        // Not in register, need to load
                        if (isTACTemp(curr->arg1)) {
                            fprintf(stderr, "Error: TAC temporary %s not in register\n", curr->arg1);
                            exit(1);
                        }
                        int srcOffset = getVarOffset(curr->arg1);
                        if (srcOffset == -1) {
                            fprintf(stderr, "Error: Variable %s not declared\n", curr->arg1);
                            exit(1);
                        }
                        regSrc = allocReg(curr->arg1);
                        fprintf(output, "    lw $t%d, %d($sp)     # Load variable '%s'\n",
                                regSrc, srcOffset, curr->arg1);
                    }

                    // For real variables, we might need a different register
                    if (!isTempResult) {
                        regDest = allocReg(curr->result);
                        if (regDest != regSrc) {
                            fprintf(output, "    move $t%d, $t%d       # %s = %s\n",
                                    regDest, regSrc, curr->result, curr->arg1);
                        }
                    } else {
                        // For TAC temporaries, reuse the source register
                        regDest = regSrc;
                        // Update register descriptor
                        strncpy(regAlloc.regs[regDest].varName, curr->result, MAX_VAR_NAME - 1);
                    }
                }

                // Store to memory only for real variables (not TAC temporaries)
                if (!isTempResult) {
                    fprintf(output, "    sw $t%d, %d($sp)     # Store to '%s'\n",
                            regDest, offset, curr->result);
                    // Update register descriptor
                    strncpy(regAlloc.regs[regDest].varName, curr->result, MAX_VAR_NAME - 1);
                    regAlloc.regs[regDest].isDirty = 0;  /* No longer dirty after store */
                }

                break;
            }

            case TAC_PRINT: {
                // PRINT arg1 — float path: syscall 2 (print_float) via $f12
                if (isFloatVar(curr->arg1) || isFloatTemp(curr->arg1)) {
                    int fprint;
                    if (isFloatTemp(curr->arg1)) {
                        fprint = findFPUReg(curr->arg1);
                        if (fprint == -1) { fprintf(stderr,"Error: float temp %s not in FPU\n",curr->arg1); exit(1); }
                    } else {
                        int off = getVarOffset(curr->arg1);
                        fprint = allocFPUReg(curr->arg1);
                        fprintf(output, "    l.s $f%d, %d($sp)    # load float %s for print\n", fprint, off, curr->arg1);
                    }
                    fprintf(output, "    # Print float\n");
                    fprintf(output, "    mov.s $f12, $f%d\n", fprint);
                    fprintf(output, "    jal __print_float\n");
                    fprintf(output, "    # Print newline\n");
                    fprintf(output, "    li $v0, 11\n");
                    fprintf(output, "    li $a0, 10\n");
                    fprintf(output, "    syscall\n");
                    break;
                }

                // Integer print path
                int regPrint;

                // Load value to print
                if (isConstant(curr->arg1)) {
                    // Constant value
                    char tempName[32];
                    sprintf(tempName, "const_%s", curr->arg1);
                    regPrint = allocReg(tempName);
                    fprintf(output, "    li $t%d, %s         # Load constant %s for print\n",
                            regPrint, curr->arg1, curr->arg1);
                } else {
                    // Variable or TAC temporary
                    regPrint = findVarReg(curr->arg1);
                    if (regPrint == -1) {
                        // Not in register, need to load
                        if (isTACTemp(curr->arg1)) {
                            fprintf(stderr, "Error: TAC temporary %s not in register\n", curr->arg1);
                            exit(1);
                        }
                        int offset = getVarOffset(curr->arg1);
                        if (offset == -1) {
                            fprintf(stderr, "Error: Variable %s not declared\n", curr->arg1);
                            exit(1);
                        }
                        regPrint = allocReg(curr->arg1);
                        fprintf(output, "    lw $t%d, %d($sp)     # Load variable '%s' for print\n",
                                regPrint, offset, curr->arg1);
                    }
                }

                // Print the value
                fprintf(output, "    # Print integer\n");
                fprintf(output, "    move $a0, $t%d\n", regPrint);
                fprintf(output, "    li $v0, 1\n");
                fprintf(output, "    syscall\n");

                // Print newline
                fprintf(output, "    # Print newline\n");
                fprintf(output, "    li $v0, 11\n");
                fprintf(output, "    li $a0, 10\n");
                fprintf(output, "    syscall\n");

                // Free register after print
                freeReg(regPrint);

                break;
            }

            case TAC_ARG: {
                // Store argument at ARG_AREA_OFFSET + argIndex*4 on the stack
                int argOffset = ARG_AREA_OFFSET + argIndex * 4;
                fprintf(output, "    # Argument %d: %s\n", argIndex, curr->arg1);
                if (isConstant(curr->arg1)) {
                    int reg = allocReg("__arg_temp");
                    fprintf(output, "    li $t%d, %s\n", reg, curr->arg1);
                    fprintf(output, "    sw $t%d, %d($sp)     # Store arg %d\n", reg, argOffset, argIndex);
                    freeReg(reg);
                } else if (isVarDeclared(curr->arg1) && isArrayVar(curr->arg1)) {
                    // For arrays, pass the base address instead of loading a value
                    int baseOff = getVarOffset(curr->arg1);
                    int reg = allocReg("__arg_temp");
                    fprintf(output, "    addi $t%d, $sp, %d   # Address of array '%s'\n", reg, baseOff, curr->arg1);
                    fprintf(output, "    sw $t%d, %d($sp)     # Store arg %d (array address)\n", reg, argOffset, argIndex);
                    freeReg(reg);
                } else {
                    int reg = findVarReg(curr->arg1);
                    if (reg != -1) {
                        fprintf(output, "    sw $t%d, %d($sp)     # Store arg %d\n", reg, argOffset, argIndex);
                    } else {
                        int offset = getVarOffset(curr->arg1);
                        if (offset != -1) {
                            int tmpReg = allocReg("__arg_temp");
                            fprintf(output, "    lw $t%d, %d($sp)\n", tmpReg, offset);
                            fprintf(output, "    sw $t%d, %d($sp)     # Store arg %d\n", tmpReg, argOffset, argIndex);
                            freeReg(tmpReg);
                        }
                    }
                }
                argIndex++;
                break;
            }

            case TAC_CALL: {
                // Special built-in: output_string(addr) — MIPS print-string syscall
                if (curr->arg1 && strcmp(curr->arg1, "output_string") == 0) {
                    int reg = allocReg("__str_print");
                    fprintf(output, "    # output_string built-in\n");
                    fprintf(output, "    lw $t%d, %d($sp)     # Load string address from arg area\n",
                            reg, ARG_AREA_OFFSET);
                    fprintf(output, "    move $a0, $t%d\n", reg);
                    fprintf(output, "    li $v0, 4\n");
                    fprintf(output, "    syscall\n");
                    freeReg(reg);
                    argIndex = 0;
                    break;
                }

                // Special built-in: string_equal(s1, s2) — byte-by-byte comparison, returns 0 or 1
                if (curr->arg1 && strcmp(curr->arg1, "string_equal") == 0) {
                    static int seqId = 0;
                    int id = seqId++;
                    int r1 = allocReg("__seq_s1");
                    int r2 = allocReg("__seq_s2");
                    int rc = allocReg("__seq_ch");
                    int rr = allocReg(curr->result ? curr->result : "__seq_res");
                    fprintf(output, "    # string_equal built-in\n");
                    fprintf(output, "    lw $t%d, %d($sp)     # s1\n", r1, ARG_AREA_OFFSET);
                    fprintf(output, "    lw $t%d, %d($sp)     # s2\n", r2, ARG_AREA_OFFSET + 4);
                    fprintf(output, "_seq_loop%d:\n", id);
                    fprintf(output, "    lb $t%d, 0($t%d)\n", rc, r1);
                    fprintf(output, "    lb $t%d, 0($t%d)\n", rr, r2);
                    fprintf(output, "    bne $t%d, $t%d, _seq_ne%d\n", rc, rr, id);
                    fprintf(output, "    beqz $t%d, _seq_eq%d\n", rc, id);
                    fprintf(output, "    addi $t%d, $t%d, 1\n", r1, r1);
                    fprintf(output, "    addi $t%d, $t%d, 1\n", r2, r2);
                    fprintf(output, "    j _seq_loop%d\n", id);
                    fprintf(output, "_seq_eq%d:\n", id);
                    fprintf(output, "    li $t%d, 1\n", rr);
                    fprintf(output, "    j _seq_done%d\n", id);
                    fprintf(output, "_seq_ne%d:\n", id);
                    fprintf(output, "    li $t%d, 0\n", rr);
                    fprintf(output, "_seq_done%d:\n", id);
                    fprintf(output, "    move $v0, $t%d\n", rr);
                    freeReg(r1); freeReg(r2); freeReg(rc);
                    argIndex = 0;
                    break;
                }

                // Special built-in: string_concat(s1, s2) — heap-allocates result, returns address
                if (curr->arg1 && strcmp(curr->arg1, "string_concat") == 0) {
                    static int scId = 0;
                    int id = scId++;
                    int r1   = allocReg("__scat_s1");
                    int r2   = allocReg("__scat_s2");
                    /* Name base after curr->result so the following TAC_ASSIGN can find it */
                    int base = allocReg(curr->result ? curr->result : "__scat_base");
                    int dst  = allocReg("__scat_dst");
                    int ch   = allocReg("__scat_ch");
                    int hp   = allocReg("__scat_hp");
                    fprintf(output, "    # string_concat built-in\n");
                    fprintf(output, "    lw $t%d, %d($sp)         # s1\n", r1, ARG_AREA_OFFSET);
                    fprintf(output, "    lw $t%d, %d($sp)         # s2\n", r2, ARG_AREA_OFFSET + 4);
                    /* Save base address of result (current heap top) */
                    fprintf(output, "    la $t%d, _heap_ptr\n", hp);
                    fprintf(output, "    lw $t%d, 0($t%d)         # base = *_heap_ptr\n", base, hp);
                    fprintf(output, "    move $t%d, $t%d          # dst  = base\n", dst, base);
                    /* Copy s1 byte by byte */
                    fprintf(output, "_scat_cp1_%d:\n", id);
                    fprintf(output, "    lb $t%d, 0($t%d)\n", ch, r1);
                    fprintf(output, "    beqz $t%d, _scat_cp2_%d\n", ch, id);
                    fprintf(output, "    sb $t%d, 0($t%d)\n", ch, dst);
                    fprintf(output, "    addi $t%d, $t%d, 1\n", r1, r1);
                    fprintf(output, "    addi $t%d, $t%d, 1\n", dst, dst);
                    fprintf(output, "    j _scat_cp1_%d\n", id);
                    /* Copy s2 byte by byte */
                    fprintf(output, "_scat_cp2_%d:\n", id);
                    fprintf(output, "    lb $t%d, 0($t%d)\n", ch, r2);
                    fprintf(output, "    beqz $t%d, _scat_done_%d\n", ch, id);
                    fprintf(output, "    sb $t%d, 0($t%d)\n", ch, dst);
                    fprintf(output, "    addi $t%d, $t%d, 1\n", r2, r2);
                    fprintf(output, "    addi $t%d, $t%d, 1\n", dst, dst);
                    fprintf(output, "    j _scat_cp2_%d\n", id);
                    fprintf(output, "_scat_done_%d:\n", id);
                    fprintf(output, "    sb $zero, 0($t%d)        # null-terminate\n", dst);
                    fprintf(output, "    addi $t%d, $t%d, 1\n", dst, dst);
                    /* Advance heap pointer past the new string */
                    fprintf(output, "    sw $t%d, 0($t%d)         # *_heap_ptr = next free\n", dst, hp);
                    /* Return base address of new string */
                    fprintf(output, "    move $v0, $t%d           # return base\n", base);
                    freeReg(r1); freeReg(r2);
                    /* base/curr->result kept live — TAC_ASSIGN will read it next */
                    freeReg(dst); freeReg(ch); freeReg(hp);
                    argIndex = 0;
                    break;
                }

                // Save caller-saved registers ($t0-$t9) at offsets 200-236
                fprintf(output, "    # Call function %s\n", curr->arg1);
                for (int i = 0; i < 10; i++) {
                    fprintf(output, "    sw $t%d, %d($sp)\n", i, 200 + i * 4);
                }
                fprintf(output, "    jal %s\n", curr->arg1);
                // Restore caller-saved registers
                for (int i = 0; i < 10; i++) {
                    fprintf(output, "    lw $t%d, %d($sp)\n", i, 200 + i * 4);
                }
                // Move return value from $v0 into allocated register
                int regResult = allocReg(curr->result);
                fprintf(output, "    move $t%d, $v0\n", regResult);
                // Reset arg index for next call
                argIndex = 0;
                break;
            }

            case TAC_RETURN: {
                // Return statement
                if (curr->arg1) {
                    // Load return value into register
                    int regReturn;
                    if (isConstant(curr->arg1)) {
                        char tempName[32];
                        sprintf(tempName, "const_%s", curr->arg1);
                        regReturn = allocReg(tempName);
                        fprintf(output, "    li $t%d, %s         # Load return value\n",
                                regReturn, curr->arg1);
                    } else if (isArrayVar(curr->arg1)) {
                        // Returning an array - return its base address, not a value
                        int offset = getVarOffset(curr->arg1);
                        regReturn = allocReg(curr->arg1);
                        fprintf(output, "    addi $t%d, $sp, %d   # Load address of array '%s'\n",
                                regReturn, offset, curr->arg1);
                    } else {
                        regReturn = findVarReg(curr->arg1);
                        if (regReturn == -1) {
                            if (isTACTemp(curr->arg1)) {
                                fprintf(stderr, "Error: TAC temporary %s not in register\n", curr->arg1);
                                exit(1);
                            }
                            int offset = getVarOffset(curr->arg1);
                            if (offset == -1) {
                                fprintf(stderr, "Error: Variable %s not declared\n", curr->arg1);
                                exit(1);
                            }
                            regReturn = allocReg(curr->arg1);
                            fprintf(output, "    lw $t%d, %d($sp)     # Load return value '%s'\n",
                                    regReturn, offset, curr->arg1);
                        }
                    }
                    fprintf(output, "    move $v0, $t%d       # Move to return register\n", regReturn);
                    freeReg(regReturn);
                }
                // Emit return sequence (mirrors TAC_FUNC_END)
                if (strcmp(currentFuncName, "main") == 0) {
                    fprintf(output, "    addi $sp, $sp, 400\n");
                    fprintf(output, "    li $v0, 10\n");
                    fprintf(output, "    syscall\n");
                } else {
                    fprintf(output, "    lw $ra, 396($sp)     # Restore return address\n");
                    fprintf(output, "    addi $sp, $sp, 400\n");
                    fprintf(output, "    jr $ra               # Return from function\n");
                }
                break;
            }

            case TAC_ARRAY_DECL: {
                // Declare array in symbol table
                // arg1 holds the size as a string (from TAC generation)
                int size = curr->arg1 ? atoi(curr->arg1) : 10;
                int offset = addArrayVar(curr->result, curr->type ? curr->type : "int", size);
                if (offset == -1) {
                    fprintf(stderr, "Error: Array %s already declared\n", curr->result);
                    exit(1);
                }
                fprintf(output, "    # Declared array '%s[%d]' at offset %d\n", curr->result, size, offset);
                break;
            }

            case TAC_ARRAY_ASSIGN: {
                // ARRAY_ASSIGN result[arg1] = arg2
                int baseOffset = getVarOffset(curr->result);
                if (baseOffset == -1) {
                    fprintf(stderr, "Error: Array %s not declared\n", curr->result);
                    exit(1);
                }

                // Load index into register
                int regIndex;
                if (isConstant(curr->arg1)) {
                    char tempName[32];
                    sprintf(tempName, "idx_const_%s", curr->arg1);
                    regIndex = allocReg(tempName);
                    fprintf(output, "    li $t%d, %s         # Load index constant\n",
                            regIndex, curr->arg1);
                } else {
                    regIndex = findVarReg(curr->arg1);
                    if (regIndex == -1) {
                        if (isTACTemp(curr->arg1)) {
                            fprintf(stderr, "Error: TAC temporary %s not in register\n", curr->arg1);
                            exit(1);
                        }
                        int off = getVarOffset(curr->arg1);
                        regIndex = allocReg(curr->arg1);
                        fprintf(output, "    lw $t%d, %d($sp)     # Load index '%s'\n",
                                regIndex, off, curr->arg1);
                    }
                }

                // Detect float array assignment
                int isFloatArr = isFloatVar(curr->result) ||
                                 isFloatTemp(curr->arg2) ||
                                 (curr->arg2 && isTACTemp(curr->arg2) && findFPUReg(curr->arg2) != -1);

                // Calculate element address (always uses integer registers)
                int regAddr = allocReg("__arr_addr");
                fprintf(output, "    # Array assign: %s[%s] = %s\n", curr->result, curr->arg1, curr->arg2);
                fprintf(output, "    sll $t%d, $t%d, 2    # index * 4\n", regIndex, regIndex);
                if (isArrayVar(curr->result)) {
                    fprintf(output, "    addi $t%d, $sp, %d   # base address of '%s'\n", regAddr, baseOffset, curr->result);
                } else {
                    fprintf(output, "    lw $t%d, %d($sp)     # load pointer '%s'\n", regAddr, baseOffset, curr->result);
                }
                fprintf(output, "    add $t%d, $t%d, $t%d # element address\n", regAddr, regAddr, regIndex);

                if (isFloatArr) {
                    // Float store path: value lives in an FPU register
                    int fval;
                    if (isFloatTemp(curr->arg2) || (isTACTemp(curr->arg2) && findFPUReg(curr->arg2) != -1)) {
                        fval = findFPUReg(curr->arg2);
                        if (fval == -1) { fprintf(stderr, "Error: float temp %s not in FPU\n", curr->arg2); exit(1); }
                    } else {
                        int off = getVarOffset(curr->arg2);
                        fval = allocFPUReg(curr->arg2);
                        fprintf(output, "    l.s $f%d, %d($sp)    # load float '%s'\n", fval, off, curr->arg2);
                    }
                    fprintf(output, "    s.s $f%d, 0($t%d)    # store float element\n", fval, regAddr);
                } else {
                    // Integer store path
                    int regValue;
                    if (isConstant(curr->arg2)) {
                        char tempName[32];
                        sprintf(tempName, "val_const_%s", curr->arg2);
                        regValue = allocReg(tempName);
                        fprintf(output, "    li $t%d, %s         # Load value constant\n", regValue, curr->arg2);
                    } else {
                        regValue = findVarReg(curr->arg2);
                        if (regValue == -1) {
                            if (isTACTemp(curr->arg2)) {
                                fprintf(stderr, "Error: TAC temporary %s not in register\n", curr->arg2);
                                exit(1);
                            }
                            int off = getVarOffset(curr->arg2);
                            regValue = allocReg(curr->arg2);
                            fprintf(output, "    lw $t%d, %d($sp)     # Load value '%s'\n", regValue, off, curr->arg2);
                        }
                    }
                    fprintf(output, "    sw $t%d, 0($t%d)     # store value\n", regValue, regAddr);
                    freeReg(regValue);
                }

                freeReg(regIndex);
                freeReg(regAddr);
                break;
            }

            case TAC_ARRAY_ACCESS: {
                // result = arg1[arg2]  (result = arrayName[index])
                int baseOffset = getVarOffset(curr->arg1);
                if (baseOffset == -1) {
                    fprintf(stderr, "Error: Array %s not declared\n", curr->arg1);
                    exit(1);
                }

                // Load index into register
                int regIndex;
                if (isConstant(curr->arg2)) {
                    char tempName[32];
                    sprintf(tempName, "idx_const_%s", curr->arg2);
                    regIndex = allocReg(tempName);
                    fprintf(output, "    li $t%d, %s         # Load index constant\n",
                            regIndex, curr->arg2);
                } else if (isTACTemp(curr->arg2)) {
                    regIndex = findVarReg(curr->arg2);
                    if (regIndex == -1) {
                        fprintf(stderr, "Error: TAC temporary %s not in register\n", curr->arg2);
                        exit(1);
                    }
                } else {
                    // Named variable - always reload from memory for loop correctness
                    int off = getVarOffset(curr->arg2);
                    regIndex = allocReg(curr->arg2);
                    fprintf(output, "    lw $t%d, %d($sp)     # Load index '%s'\n",
                            regIndex, off, curr->arg2);
                }

                // Calculate element address
                int regShifted = allocReg("__idx_shift");
                int regAddr2   = allocReg("__arr_addr2");
                fprintf(output, "    # Array access: %s = %s[%s]\n", curr->result, curr->arg1, curr->arg2);
                fprintf(output, "    move $t%d, $t%d    # copy index to scratch\n", regShifted, regIndex);
                fprintf(output, "    sll $t%d, $t%d, 2    # index * 4\n", regShifted, regShifted);
                if (isArrayVar(curr->arg1)) {
                    fprintf(output, "    addi $t%d, $sp, %d   # base address of '%s'\n", regAddr2, baseOffset, curr->arg1);
                } else {
                    fprintf(output, "    lw $t%d, %d($sp)     # load pointer '%s'\n", regAddr2, baseOffset, curr->arg1);
                }
                fprintf(output, "    add $t%d, $t%d, $t%d # element address\n", regAddr2, regAddr2, regShifted);

                if (isFloatVar(curr->arg1)) {
                    // Float array: load element into FPU register
                    int fResult = allocFPUReg(curr->result);
                    fprintf(output, "    l.s $f%d, 0($t%d)    # load float element\n", fResult, regAddr2);
                } else {
                    // Integer array: load element into GPR
                    int regResult = allocReg(curr->result);
                    fprintf(output, "    move $t%d, $t%d    # point to element\n", regResult, regAddr2);
                    fprintf(output, "    lw $t%d, 0($t%d)     # load value\n", regResult, regResult);
                }

                freeReg(regShifted);
                freeReg(regAddr2);
                freeReg(regIndex);
                break;
            }

            case TAC_STRING_ASSIGN: {
                // result = address of string literal stored in .data as _strN
                const char* lbl = lookupStrLabel(curr->result);
                int reg = allocReg(curr->result);
                fprintf(output, "    la $t%d, %s       # Load address of string literal\n",
                        reg, lbl);
                break;
            }

            case TAC_FLOAT_ASSIGN: {
                // Load float literal from .data into an FPU register.
                // Look up by value (not temp name) to avoid temp-reuse aliasing.
                const char* lbl = lookupFloatLabelByValue(curr->arg1);
                int freg = allocFPUReg(curr->result);
                fprintf(output, "    l.s $f%d, %s      # load float literal %s\n",
                        freg, lbl, curr->arg1);
                break;
            }

            case TAC_LABEL:
                /* Emit label */
                fprintf(output, "%s:                    # Label\n", curr->result);
                break;

            case TAC_GOTO:
                /* Unconditional jump */
                fprintf(output, "    j %s              # Jump\n", curr->arg1);
                break;

            case TAC_IF_FALSE:
                /* Load condition into register */
                condReg = findVarReg(curr->arg1);
                if (condReg == -1) {
                    condReg = allocReg(curr->arg1);
                    fprintf(output, "    lw $t%d, %d($sp)\n", condReg, getVarOffset(curr->arg1));
                }
                /* Branch if zero (false) */
                fprintf(output, "    beqz $t%d, %s       # Jump if false\n",
                        condReg, curr->arg2);
                freeReg(condReg);
                break;

            default:
                break;
        }

        curr = curr->next;
    }

    // Spill any remaining dirty registers
    fprintf(output, "\n    # Spill any remaining registers\n");
    for (int i = 0; i < NUM_TEMP_REGS; i++) {
        if (regAlloc.regs[i].inUse && regAlloc.regs[i].isDirty) {
            spillReg(i);
        }
    }

    // Program exit
    fprintf(output, "\n    # Exit program\n");
    fprintf(output, "    addi $sp, $sp, 400\n");
    fprintf(output, "    li $v0, 10\n");
    fprintf(output, "    syscall\n");

    fclose(output);

    // Print register allocation statistics
    printRegAllocStats();
}

void init_register_allocator() {
    for(int i = 0; i < 8; i++) {
        char name[4];
        sprintf(name, "$t%d", i);
        reg_alloc.temp_regs[i].reg_name = strdup(name);
        reg_alloc.temp_regs[i].is_free = 1;
        reg_alloc.temp_regs[i].var_name = NULL;
        reg_alloc.temp_regs[i].last_used = 0;
    }
    reg_alloc.clock = 0;
}

char* allocate_register(char* var) {
    // First check if variable already in register
    for(int i = 0; i < 8; i++) {
        if(reg_alloc.temp_regs[i].var_name &&
           strcmp(reg_alloc.temp_regs[i].var_name, var) == 0) {
            reg_alloc.temp_regs[i].last_used = reg_alloc.clock++;
            return reg_alloc.temp_regs[i].reg_name;
        }
    }

    // Find free register
    for(int i = 0; i < 8; i++) {
        if(reg_alloc.temp_regs[i].is_free) {
            reg_alloc.temp_regs[i].is_free = 0;
            reg_alloc.temp_regs[i].var_name = strdup(var);
            reg_alloc.temp_regs[i].last_used = reg_alloc.clock++;
            return reg_alloc.temp_regs[i].reg_name;
        }
    }

    // Need to evict - find LRU
    int lru_idx = 0;
    int min_time = reg_alloc.temp_regs[0].last_used;
    for(int i = 1; i < 8; i++) {
        if(reg_alloc.temp_regs[i].last_used < min_time) {
            min_time = reg_alloc.temp_regs[i].last_used;
            lru_idx = i;
        }
    }

    // Spill old variable to stack
    if(reg_alloc.temp_regs[lru_idx].var_name) {
        int offset = getVarOffset(reg_alloc.temp_regs[lru_idx].var_name);
        fprintf(output, "    sw %s, %d($fp)\n",
                reg_alloc.temp_regs[lru_idx].reg_name, offset);
    }

    // Allocate to new variable
    reg_alloc.temp_regs[lru_idx].var_name = strdup(var);
    reg_alloc.temp_regs[lru_idx].last_used = reg_alloc.clock++;

    // Load from stack if needed
    if(isVarDeclared(var)) {
        int offset = getVarOffset(var);
        fprintf(output, "    lw %s, %d($fp)\n",
                reg_alloc.temp_regs[lru_idx].reg_name, offset);
    }

    return reg_alloc.temp_regs[lru_idx].reg_name;
}
