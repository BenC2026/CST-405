# Minimal C Compiler - Educational Version

A fully functional compiler demonstrating all phases of compilation with extensive educational features. Perfect for teaching compiler design concepts with real, working code.

## 🎯 Purpose

This compiler strips away complexity to show the **essential components** of compilation:
- **Minimal Language**: Just enough features to demonstrate key concepts
- **Clear Phases**: Each compilation phase is visible and well-documented
- **Real Output**: Generates actual MIPS assembly that runs on simulators
- **Educational Focus**: Extensive comments and explanatory output

## 📚 Language Features

Our minimal C-like language supports:
- **Integer variable declarations**: `int x;`
- **Assignment statements**: `x = 10;`
- **Addition operator**: `x + y`
- **Print statement**: `print(x);`

That's it! No loops, no conditions, no functions - just the bare essentials.

## 🔧 Compiler Architecture

### Complete Compilation Pipeline

```
Source Code (.c)
      ↓
┌─────────────────┐
│ LEXICAL ANALYSIS│ → Tokens (INT, ID, NUM, +, =, etc.)
│   (scanner.l)   │
└─────────────────┘
      ↓
┌─────────────────┐
│ SYNTAX ANALYSIS │ → Abstract Syntax Tree (AST)
│   (parser.y)    │
└─────────────────┘
      ↓
┌─────────────────┐
│  AST BUILDING   │ → Hierarchical program structure
│    (ast.c)      │
└─────────────────┘
      ↓
┌─────────────────┐
│ SEMANTIC CHECK  │ → Symbol table, type checking
│   (symtab.c)    │
└─────────────────┘
      ↓
┌─────────────────┐
│ INTERMEDIATE    │ → Three-Address Code (TAC)
│ CODE (tac.c)    │
└─────────────────┘
      ↓
┌─────────────────┐
│  OPTIMIZATION   │ → Constant folding, propagation
│   (tac.c)       │
└─────────────────┘
      ↓
┌─────────────────┐
│ CODE GENERATION │ → MIPS Assembly (.s)
│  (codegen.c)    │
└─────────────────┘
      ↓
MIPS Assembly (.s)
```

## 💾 Understanding the Stack

### What is the Stack?

The stack is a **real region of memory** that every program uses for storing local variables. It's not just an educational concept - it's how actual computers work!

### Stack Memory Layout

```
High Memory Address (0xFFFFFFFF)
         ↑
    ┌──────────┐
    │          │
    │  UNUSED  │
    │          │
    ├──────────┤
    │          │
    │  STACK   │ ← Your variables live here!
    │    ↓     │   (grows downward)
    │          │
$sp →├──────────┤ ← Stack Pointer points to top
    │          │
    │   FREE   │
    │  SPACE   │
    │          │
    ├──────────┤
    │    ↑     │
    │   HEAP   │ ← Dynamic memory (malloc)
    │          │   (grows upward)
    ├──────────┤
    │  GLOBALS │ ← Global variables
    ├──────────┤
    │   CODE   │ ← Your program
    └──────────┘
         ↓
Low Memory Address (0x00000000)
```

### How Our Compiler Uses the Stack

For this program:
```c
int x;
int y; 
int z;
x = 10;
y = 20;
z = x + y;
```

The stack layout becomes:

```
        Stack Memory
    ┌─────────────────┐
    │      ...        │
    ├─────────────────┤ ← Old $sp (before allocation)
    │   z (offset 8)  │ → Will store 30
    ├─────────────────┤
    │   y (offset 4)  │ → Stores 20
    ├─────────────────┤
    │   x (offset 0)  │ → Stores 10
    └─────────────────┘ ← $sp after "addi $sp, $sp, -400"
```

### MIPS Instructions for Stack Operations

```mips
# Allocate space (at program start)
addi $sp, $sp, -400    # Move stack pointer down 400 bytes

# Store value 10 in variable x (offset 0)
li $t0, 10            # Load immediate 10 into register $t0
sw $t0, 0($sp)        # Store Word: memory[$sp + 0] = $t0

# Load variable x into register
lw $t1, 0($sp)        # Load Word: $t1 = memory[$sp + 0]

# Deallocate space (at program end)
addi $sp, $sp, 400    # Restore stack pointer
```

## 🚀 Build & Run

### Prerequisites
- `flex` (lexical analyzer generator)
- `bison` (parser generator)
- `gcc` (C compiler)
- MIPS simulator (MARS, SPIM, or QtSPIM) for running output

### Compilation
```bash
# Build the compiler
make

# Compile a source file
./minicompiler test.c output.s

# Clean build files
make clean
```

### Example Session
```bash
$ ./minicompiler test.c output.s

╔════════════════════════════════════════════════════════════╗
║          MINIMAL C COMPILER - EDUCATIONAL VERSION         ║
╚════════════════════════════════════════════════════════════╝

┌──────────────────────────────────────────────────────────┐
│ PHASE 1: LEXICAL & SYNTAX ANALYSIS                       │
├──────────────────────────────────────────────────────────┤
│ • Reading source file: test.c
│ • Tokenizing input (scanner.l)
│ • Parsing grammar rules (parser.y)
│ • Building Abstract Syntax Tree
└──────────────────────────────────────────────────────────┘
✓ Parse successful - program is syntactically correct!

[... followed by AST, TAC, optimizations, and MIPS generation ...]
```

## 📝 Example Programs

### Simple Addition
```c
int a;
int b;
int sum;
a = 5;
b = 7;
sum = a + b;
print(sum);    // Output: 12
```

### Multiple Operations
```c
int x;
int y;
int z;
x = 10;
y = 20;
z = x + y;     // z = 30
print(z);
x = z + 5;     // x = 35
print(x);
```

## 🎓 Educational Features

### 1. **Extensive Comments**
Every source file contains detailed explanations of:
- What each component does
- Why design decisions were made
- How pieces fit together

### 2. **Visual Output**
The compiler shows:
- Beautiful ASCII boxes for each phase
- Line-by-line TAC with explanations
- Optimization steps clearly marked
- Success/error messages with helpful tips

### 3. **Phase Separation**
Each compilation phase is clearly separated:
- Lexical Analysis → Tokens
- Syntax Analysis → AST
- Intermediate Code → TAC
- Optimization → Improved TAC
- Code Generation → MIPS

### 4. **Real-World Concepts**
Students learn:
- How variables are stored in memory
- What three-address code looks like
- How optimizations work (constant folding)
- How high-level code becomes assembly

## 🗃️ Symbol Table Implementation

### Overview
The symbol table is the compiler's "memory" for tracking all declared variables. It maps variable names to their memory locations and enables semantic checking (detecting undeclared or redeclared variables).

### Core Data Structures

**Symbol Entry:**
```c
typedef struct {
    char* name;     // Variable identifier (e.g., "x", "sum")
    int offset;     // Stack offset in bytes (0, 4, 8, 12...)
} Symbol;
```

**Symbol Table:**
```c
typedef struct {
    Symbol vars[MAX_VARS];  // Array of all variables (max 100)
    int count;              // Number of variables declared
    int nextOffset;         // Next available stack offset
} SymbolTable;
```

### Key Operations

| Function | Purpose | Location |
|----------|---------|----------|
| `initSymTab()` | Initialize empty table | `symtab.c:15` |
| `addVar(name)` | Add new variable, return offset | `symtab.c:23` |
| `getVarOffset(name)` | Look up variable's stack offset | `symtab.c:46` |
| `isVarDeclared(name)` | Check if variable exists | `symtab.c:58` |
| `printSymTab()` | Debug: print table contents | `symtab.c:55` |

### Symbol Table Evolution

**1. Initialization** (`codegen.c:104`):
```
=== SYMBOL TABLE STATE ===
Count: 0, Next Offset: 0
(empty)
==========================
```

**2. After `int x;`**:
```
=== SYMBOL TABLE STATE ===
Count: 1, Next Offset: 4
Variables:
  [0] x -> offset 0
==========================
```

**3. After `int y;`**:
```
=== SYMBOL TABLE STATE ===
Count: 2, Next Offset: 8
Variables:
  [0] x -> offset 0
  [1] y -> offset 4
==========================
```

### Memory Layout Connection

The symbol table directly maps to stack memory:

```
Stack Memory Layout:
┌─────────────────┐
│  y (offset 4)   │ ← vars[1] points here
├─────────────────┤
│  x (offset 0)   │ ← vars[0] points here
└─────────────────┘ ← $sp (stack pointer)
```

### Error Detection

- **Undeclared variable**: `getVarOffset()` returns -1
- **Redeclaration**: `addVar()` returns -1
- **Semantic errors** stop compilation with clear messages

### Compiler Integration Points

| Phase | Usage | File Location |
|-------|-------|---------------|
| Code Generation | Initialize table | `codegen.c:104` |
| Variable Declaration | Add to table | `codegen.c:52` |
| Variable Usage | Look up offset | `codegen.c:24` |
| Assignment | Verify exists | `codegen.c:62` |

### Tracing Feature
The symbol table now includes debug tracing to show its evolution:
- Prints when initialized, variables added, and lookups performed
- Use `printSymTab()` to see complete table state at any time
- Helps understand how variables are stored and accessed during compilation

## 📁 File Structure

```
CST-405-minimal/
├── scanner.l      # Lexical analyzer (tokenizer)
├── parser.y       # Grammar rules and parser
├── ast.h/c        # Abstract Syntax Tree
├── symtab.h/c     # Symbol table for variables
├── tac.h/c        # Three-address code generation
├── codegen.h/c    # MIPS code generator
├── main.c         # Driver program
├── Makefile       # Build configuration
├── test.c         # Example program
└── README.md      # This file
```

## 🔄 Three-Address Code (TAC) Generation

### Overview
Three-Address Code is an intermediate representation where each instruction has at most three operands (result = arg1 op arg2). This simplifies both optimization and final code generation.

### TAC Data Structures

**TAC Instruction Types:**
```c
typedef enum {
    TAC_ADD,     // Addition: result = arg1 + arg2
    TAC_ASSIGN,  // Assignment: result = arg1
    TAC_PRINT,   // Print: print(arg1)
    TAC_DECL     // Declaration: declare result
} TACOp;
```

**TAC Instruction Structure:**
```c
typedef struct TACInstr {
    TACOp op;               // Operation type
    char* arg1;             // First operand
    char* arg2;             // Second operand (for binary ops)
    char* result;           // Result/destination
    struct TACInstr* next;  // Linked list pointer
} TACInstr;
```

### TAC Generation Process

**1. AST to TAC Conversion** (`tac.c:82`):
- **Declarations**: `NODE_DECL` → `TAC_DECL`
- **Assignments**: `NODE_ASSIGN` → `TAC_ASSIGN`
- **Expressions**: `NODE_BINOP` → `TAC_ADD` with temp variables
- **Print statements**: `NODE_PRINT` → `TAC_PRINT`

**2. Expression Handling** (`tac.c:52`):
- **Numbers**: Converted to string literals
- **Variables**: Referenced by name
- **Binary operations**: Create temporary variables (t0, t1, t2...)

### TAC Generation Example

**Source Code:**
```c
int x;
int y;
x = 10;
y = 20;
print(x + y);
```

**Generated TAC:**
```
1: DECL x          // Declare variable 'x'
2: DECL y          // Declare variable 'y'
3: x = 10          // Assign constant to x
4: y = 20          // Assign constant to y
5: t0 = x + y      // Add: store result in temp t0
6: PRINT t0        // Output value of t0
```

## ⚡ TAC Optimization

### Optimization Techniques Implemented

**1. Constant Folding** (`tac.c:183`):
- Evaluates compile-time constant expressions
- `10 + 20` becomes `30` directly

**2. Copy Propagation** (`tac.c:147`):
- Replaces variable references with their known values
- Uses a value propagation table to track variable-value mappings

### Optimization Process

**Value Propagation Table:**
```c
typedef struct {
    char* var;    // Variable name
    char* value;  // Known value (constant or variable)
} VarValue;
```

**Optimization Steps:**
1. **Track assignments**: Store variable-value mappings
2. **Substitute references**: Replace variables with known values
3. **Fold constants**: Evaluate constant expressions at compile-time
4. **Propagate results**: Update the propagation table

### Optimization Example

**Original TAC:**
```
1: DECL x
2: DECL y
3: x = 10          // x now maps to "10"
4: y = 20          // y now maps to "20"
5: t0 = x + y      // Substitute: t0 = 10 + 20
6: PRINT t0
```

**Optimized TAC:**
```
1: DECL x
2: DECL y
3: x = 10          // Constant value: 10
4: y = 20          // Constant value: 20
5: t0 = 30         // Folded: 10 + 20 = 30
6: PRINT 30        // Propagated constant
```

## 🖥️ MIPS Code Generation

### Overview
The final phase converts the AST directly to MIPS assembly, using the symbol table for variable memory management and register allocation for temporary values.

### MIPS Architecture Used

**Registers:**
- `$sp`: Stack pointer (points to top of stack)
- `$t0-$t7`: Temporary registers for computations
- `$a0`: Argument register for system calls
- `$v0`: System call number register

**Memory Layout:**
- Variables stored on stack with negative offsets from `$sp`
- Stack grows downward (decreasing addresses)
- Each integer variable occupies 4 bytes

### Code Generation Process

**1. Initialization** (`codegen.c:96`):
```mips
.data                    # Data section (empty for our simple language)
.text                    # Code section
.globl main             # Make main globally visible
main:                   # Program entry point
    addi $sp, $sp, -400 # Allocate 400 bytes stack space
```

**2. Variable Operations:**

**Declaration** (`codegen.c:51`):
```mips
# int x; -> adds to symbol table, generates comment
# Declared x at offset 0
```

**Assignment** (`codegen.c:61`):
```mips
# x = 10;
li $t0, 10        # Load immediate value 10
sw $t0, 0($sp)    # Store word at stack offset 0
```

**Variable Access** (`codegen.c:23`):
```mips
# Reading variable x
lw $t1, 0($sp)    # Load word from stack offset 0
```

**Addition** (`codegen.c:34`):
```mips
# x + y
lw $t0, 0($sp)    # Load x into $t0
lw $t1, 4($sp)    # Load y into $t1
add $t0, $t0, $t1 # Add: $t0 = $t0 + $t1
```

**Print Statement** (`codegen.c:73`):
```mips
# print(value)
move $a0, $t0     # Move value to argument register
li $v0, 1         # System call number for print integer
syscall           # Execute system call
li $v0, 11        # System call for print character
li $a0, 10        # ASCII newline character
syscall           # Print newline
```

**3. Program Termination** (`codegen.c:120`):
```mips
addi $sp, $sp, 400    # Deallocate stack space
li $v0, 10            # System call number for exit
syscall               # Terminate program
```

### Complete MIPS Example

**Source Code:**
```c
int x;
int y;
x = 10;
y = 20;
print(x + y);
```

**Generated MIPS:**
```mips
.data
.text
.globl main
main:
    # Allocate stack space
    addi $sp, $sp, -400

    # Declared x at offset 0
    # Declared y at offset 4

    # x = 10;
    li $t0, 10
    sw $t0, 0($sp)

    # y = 20;
    li $t1, 20
    sw $t1, 4($sp)

    # print(x + y);
    lw $t0, 0($sp)        # Load x
    lw $t1, 4($sp)        # Load y
    add $t0, $t0, $t1     # x + y
    move $a0, $t0         # Prepare for print
    li $v0, 1             # Print integer
    syscall
    li $v0, 11            # Print newline
    li $a0, 10
    syscall

    # Exit program
    addi $sp, $sp, 400
    li $v0, 10
    syscall
```

### Integration with Symbol Table

The MIPS generator uses the symbol table for:
- **Variable declarations**: Add to symbol table, get stack offset
- **Variable access**: Look up stack offset for load/store operations
- **Error detection**: Verify variables are declared before use

## 🔍 Understanding the Output

### Compilation Phases in Action

**Phase 1**: Source → AST (syntax tree)
**Phase 2**: AST → TAC (intermediate code)
**Phase 3**: TAC → Optimized TAC (improved efficiency)
**Phase 4**: AST → MIPS (final machine code)

## 📖 Deep Dive: Lexical Analysis

### What is Lexical Analysis?
Lexical analysis (scanning) is the first phase of compilation. It reads the source code character by character and groups them into meaningful units called **tokens**.

### Token Types in Our Language
```c
// Input: int x = 10 + y;
// Tokens produced:
INT       "int"      // Keyword token
ID        "x"        // Identifier token
ASSIGN    "="        // Assignment operator
NUM       "10"       // Number literal
PLUS      "+"        // Addition operator
ID        "y"        // Identifier token
SEMICOLON ";"        // Statement terminator
```

### Scanner Implementation (`scanner.l`)
```flex
%{
#include "tokens.h"    // Token definitions
%}

DIGIT    [0-9]
LETTER   [a-zA-Z]
ID       {LETTER}({LETTER}|{DIGIT})*
NUMBER   {DIGIT}+

%%
"int"       { return INT; }
"print"     { return PRINT; }
{ID}        { yylval.string = strdup(yytext); return ID; }
{NUMBER}    { yylval.num = atoi(yytext); return NUM; }
"="         { return ASSIGN; }
"+"         { return PLUS; }
";"         { return SEMICOLON; }
[ \t\n]     { /* skip whitespace */ }
.           { printf("Unexpected character: %s\n", yytext); }
%%
```

### How the Scanner Works
1. **Pattern Matching**: Uses regular expressions to match character sequences
2. **Token Creation**: When a pattern matches, create appropriate token
3. **State Management**: Maintains position in source file
4. **Error Handling**: Reports unrecognized characters

## 🌳 Deep Dive: Parsing & AST Construction

### What is Parsing?
Parsing (syntax analysis) takes the stream of tokens from the lexer and builds a hierarchical structure (Abstract Syntax Tree) that represents the program's structure.

### Grammar Rules (`parser.y`)
```yacc
program : stmt_list                    { root = $1; }

stmt_list : stmt_list stmt            { $$ = createStmtList($1, $2); }
          | stmt                      { $$ = $1; }

stmt : declaration                    { $$ = $1; }
     | assignment                     { $$ = $1; }
     | print_stmt                     { $$ = $1; }

declaration : INT ID SEMICOLON        { $$ = createDecl($2); }

assignment : ID ASSIGN expr SEMICOLON { $$ = createAssign($1, $3); }

print_stmt : PRINT '(' expr ')' SEMICOLON { $$ = createPrint($3); }

expr : expr PLUS term                 { $$ = createBinOp($1, '+', $3); }
     | term                           { $$ = $1; }

term : NUM                            { $$ = createNum($1); }
     | ID                             { $$ = createVar($1); }
     | '(' expr ')'                   { $$ = $2; }
```

### AST Node Types
```c
typedef enum {
    NODE_DECL,      // Variable declaration
    NODE_ASSIGN,    // Assignment statement
    NODE_PRINT,     // Print statement
    NODE_BINOP,     // Binary operation
    NODE_NUM,       // Number literal
    NODE_VAR,       // Variable reference
    NODE_STMT_LIST  // Statement sequence
} NodeType;
```

### Parse Tree vs AST
```
Source: x = 2 + 3

Parse Tree (detailed):          AST (simplified):
    assignment                      ASSIGN
   /    |    \                     /      \
  ID   '='   expression          VAR:x   BINOP:+
  |           |                          /      \
 "x"      addition                   NUM:2    NUM:3
          /   |   \
       term  '+'  term
        |          |
     NUM:2      NUM:3
```

## 🔬 Deep Dive: Semantic Analysis

### What is Semantic Analysis?
Semantic analysis ensures the program makes **sense** - variables are declared before use, types match, etc. This is where the symbol table becomes crucial.

### Semantic Checks Performed
1. **Declaration before use**: Every variable must be declared before being used
2. **No redeclaration**: Can't declare the same variable twice
3. **Type consistency**: All our variables are integers (simplified)

### Symbol Table as a Hash Map
```c
// Real compilers often use hash tables for O(1) lookup
// Our simple version uses linear search for clarity

int getVarOffset(char* name) {
    for (int i = 0; i < symtab.count; i++) {        // O(n) lookup
        if (strcmp(symtab.vars[i].name, name) == 0) {
            return symtab.vars[i].offset;
        }
    }
    return -1;  // Not found
}

// Production compiler would use:
// hash_table_lookup(symbol_table, name) -> O(1) average case
```

### Scope Management (Advanced)
```c
// Our minimal compiler has only global scope
// Real compilers manage nested scopes:

typedef struct Scope {
    SymbolTable* table;
    struct Scope* parent;  // Outer scope
    int level;             // Nesting depth
} Scope;

// Symbol lookup walks up the scope chain:
// 1. Check current scope
// 2. Check parent scope
// 3. Continue until found or reach global scope
```

## ⚙️ Deep Dive: TAC Intermediate Representation

### Why Use Intermediate Code?
1. **Platform Independence**: Same IR can target multiple architectures
2. **Optimization**: Easier to optimize than high-level or assembly code
3. **Modularity**: Separates front-end parsing from back-end code generation

### TAC Instruction Format
```
result = operand1 operator operand2
```

### Example: Complex Expression
```c
// Source: result = (a + b) * (c + d);

// TAC Generation:
t1 = a + b          // Evaluate first subexpression
t2 = c + d          // Evaluate second subexpression
t3 = t1 * t2        // Combine results
result = t3         // Final assignment
```

### TAC vs Other IRs
```
High-Level:  result = (a + b) * (c + d);

TAC:         t1 = a + b
             t2 = c + d
             t3 = t1 * t2
             result = t3

SSA Form:    t1 = a₁ + b₁        // Single Static Assignment
             t2 = c₁ + d₁        // Each variable assigned once
             t3 = t1 * t2
             result₁ = t3

Assembly:    lw $t0, a_offset($sp)    // Much more detailed
             lw $t1, b_offset($sp)
             add $t0, $t0, $t1
             lw $t2, c_offset($sp)
             lw $t3, d_offset($sp)
             add $t2, $t2, $t3
             mul $t0, $t0, $t2
             sw $t0, result_offset($sp)
```

## 🎯 Deep Dive: Optimization Theory

### Data Flow Analysis
Optimization requires understanding how data flows through the program:

```c
x = 5;        // Definition of x
y = x + 3;    // Use of x, definition of y
z = y * 2;    // Use of y, definition of z
print(z);     // Use of z
```

### Reaching Definitions
Which definitions of variables can reach each program point?

```c
x = 5;        // D1: x defined here
if (...) {
    x = 10;   // D2: x redefined here
}
y = x + 1;    // Both D1 and D2 can reach here
```

### Live Variable Analysis
Which variables are "live" (will be used again) at each program point?

```c
x = 5;        // x is live (used below)
y = 10;       // y is live (used below)
z = x + y;    // x,y dead after this, z is live
print(z);     // z dead after this
```

### Optimization Opportunities
```c
// Dead Code Elimination
x = 5;
y = 10;       // y is never used - can be eliminated
z = x + 3;
print(z);

// Constant Propagation
x = 5;        // x is constant
y = x + 3;    // becomes: y = 5 + 3
print(y);     // becomes: print(8)

// Common Subexpression Elimination
a = b + c;
d = b + c;    // Same as a - can reuse
// Optimized: d = a;
```

## 💻 Deep Dive: Target Code Generation

### Register Allocation
Real compilers must decide which variables/temporaries go in registers vs memory:

```c
// Many variables, few registers
int a, b, c, d, e, f, g, h, i, j;

// MIPS has only:
// $t0-$t7: 8 temporary registers
// $s0-$s7: 8 saved registers

// Must "spill" some variables to memory
// Graph coloring algorithm assigns registers optimally
```

### Instruction Selection
Multiple ways to generate code for the same operation:

```c
// x = y + 1;

// Option 1: Load-Add-Store
lw $t0, y_offset($sp)    // Load y
addi $t0, $t0, 1         // Add immediate 1
sw $t0, x_offset($sp)    // Store to x

// Option 2: Direct increment (if x == y)
lw $t0, y_offset($sp)    // Load y
addi $t0, $t0, 1         // Increment
sw $t0, y_offset($sp)    // Store back to y (now x)
```

### Calling Conventions
How functions call each other (not in our minimal language):

```mips
# MIPS calling convention
# Arguments: $a0-$a3
# Return value: $v0-$v1
# Caller-saved: $t0-$t9
# Callee-saved: $s0-$s7

function_call:
    # Save caller-saved registers
    sw $t0, 0($sp)
    sw $t1, 4($sp)

    # Set up arguments
    li $a0, 42

    # Call function
    jal my_function

    # Use return value
    move $t0, $v0

    # Restore caller-saved registers
    lw $t1, 4($sp)
    lw $t0, 0($sp)
```

## 🏗️ Compiler Construction Best Practices

### Error Recovery
Our compiler stops on first error, but production compilers continue:

```c
int x;
int y;
x = 10;
y = z + 5;    // Error: z undeclared
print(y);     // Continue parsing anyway
```

### Error Messages
Good compilers provide helpful error messages:

```
Bad:  "Syntax error on line 4"

Good: "Error on line 4: Variable 'z' used but not declared
       Suggestion: Did you mean variable 'x'?"
```

### Testing Strategy
1. **Unit Tests**: Test individual phases
2. **Integration Tests**: Test full compilation pipeline
3. **Regression Tests**: Ensure fixes don't break existing code
4. **Stress Tests**: Large programs, edge cases

### Performance Considerations
1. **Memory Management**: Avoid memory leaks in AST construction
2. **Algorithm Complexity**: Use efficient data structures
3. **Compilation Speed**: Balance optimization quality vs compile time

## 🔬 Research Connections

### Advanced Optimization
- **Loop Optimization**: Unrolling, vectorization, parallelization
- **Interprocedural Analysis**: Whole-program optimization
- **Profile-Guided Optimization**: Use runtime data to guide optimization

### Modern Compiler Techniques
- **Just-In-Time Compilation**: Compile at runtime (Java HotSpot, V8)
- **Ahead-of-Time Compilation**: Full compilation before deployment
- **Cross-Compilation**: Target different architectures

### Domain-Specific Languages
- **SQL Compilers**: Optimize database queries
- **Shader Compilers**: GPU programming languages
- **Configuration Languages**: Terraform, Kubernetes YAML

## 🎯 Learning Objectives

Students will understand:
1. **Lexical Analysis**: How text becomes tokens using finite automata
2. **Parsing**: How tokens become syntax trees using context-free grammars
3. **Semantic Analysis**: How meaning is verified through symbol tables
4. **Intermediate Representations**: Platform-independent code design
5. **Optimization**: Data flow analysis and code transformation
6. **Code Generation**: Instruction selection and register allocation
7. **Memory Management**: Stack layout and variable storage
8. **Compilation Pipeline**: How phases connect and information flows
9. **Software Engineering**: Error handling, testing, and modularity
10. **Computer Systems**: How high-level code becomes machine instructions

## 🤝 Contributing

This is an educational project. Suggestions for making concepts clearer are welcome!

## 📜 License

Educational use - free to use and modify for teaching purposes.