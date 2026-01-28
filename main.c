/* MINIMAL C COMPILER - EDUCATIONAL VERSION
 * Demonstrates all phases of compilation with a simple language
 * Supports: int variables, addition, assignment, print
 */
#include <stdio.h>
#include <stdlib.h>
#include "ast.h"
#include "codegen.h"
#include "tac.h"
#include "semantic.h"
#include "symtab.h"

extern int yyparse();
extern FILE* yyin;
extern ASTNode* root;

/* Global flag to enable token display in lexer */
int displayTokens = 1;

int main(int argc, char* argv[]) {
    if (argc != 3) {
        printf("Usage: %s <input.c> <output.s>\n", argv[0]);
        printf("Example: ./minicompiler test.c output.s\n");
        return 1;
    }
    
    yyin = fopen(argv[1], "r");
    if (!yyin) {
        fprintf(stderr, "Error: Cannot open input file '%s'\n", argv[1]);
        return 1;
    }
    
    printf("\n");
    printf("╔════════════════════════════════════════════════════════════╗\n");
    printf("║          MINIMAL C COMPILER - EDUCATIONAL VERSION          ║\n");
    printf("╚════════════════════════════════════════════════════════════╝\n");
    printf("\n");
    
    /* PHASE 1: Lexical and Syntax Analysis */
    printf("┌──────────────────────────────────────────────────────────┐\n");
    printf("│ PHASE 1: LEXICAL & SYNTAX ANALYSIS                       │\n");
    printf("├──────────────────────────────────────────────────────────┤\n");
    printf("│ • Reading source file: %s\n", argv[1]);
    printf("│ • Tokenizing input (scanner.l)\n");
    printf("│ • Parsing grammar rules (parser.y)\n");
    printf("│ • Building Abstract Syntax Tree\n");
    printf("└──────────────────────────────────────────────────────────┘\n");
    printf("\nTokens recognized:\n");

    if (yyparse() == 0) {
        printf("\n✓ Parse successful - program is syntactically correct!\n\n");
        
        /* PHASE 2: AST Display */
        printf("┌──────────────────────────────────────────────────────────┐\n");
        printf("│ PHASE 2: ABSTRACT SYNTAX TREE (AST)                      │\n");
        printf("├──────────────────────────────────────────────────────────┤\n");
        printf("│ Tree structure representing the program hierarchy:       │\n");
        printf("└──────────────────────────────────────────────────────────┘\n");
        printAST(root, 0);
        printf("\n");

        /* PHASE 3: Semantic Analysis */
        printf("┌──────────────────────────────────────────────────────────┐\n");
        printf("│ PHASE 3: SEMANTIC ANALYSIS                               │\n");
        printf("├──────────────────────────────────────────────────────────┤\n");
        printf("│ Checking semantic correctness:                           │\n");
        printf("│ • Variables declared before use                          │\n");
        printf("│ • No duplicate declarations                              │\n");
        printf("│ • Type consistency (for future extensions)               │\n");
        printf("└──────────────────────────────────────────────────────────┘\n");
        initSemantic();
        if (!analyzeProgram(root)) {
            printf("\n✗ Compilation failed due to semantic errors!\n");
            fclose(yyin);
            return 1;
        }
        printf("\n");

        /* PHASE 4: Intermediate Code */
        printf("┌──────────────────────────────────────────────────────────┐\n");
        printf("│ PHASE 4: INTERMEDIATE CODE GENERATION                    │\n");
        printf("├──────────────────────────────────────────────────────────┤\n");
        printf("│ Three-Address Code (TAC) - simplified instructions:      │\n");
        printf("│ • Each instruction has at most 3 operands                │\n");
        printf("│ • Temporary variables (t0, t1, ...) for expressions      │\n");
        printf("└──────────────────────────────────────────────────────────┘\n");
        initTAC();
        generateTAC(root);
        printTAC();

        // Save TAC to file
        char tacFile[256];
        sprintf(tacFile, "%s.tac", argv[1]);
        saveTACToFile(tacFile);
        printf("✓ TAC saved to: %s\n\n", tacFile);

        /* PHASE 5: Optimization */
        printf("┌──────────────────────────────────────────────────────────┐\n");
        printf("│ PHASE 5: CODE OPTIMIZATION                               │\n");
        printf("├──────────────────────────────────────────────────────────┤\n");
        printf("│ Applying optimizations:                                  │\n");
        printf("│ • Constant folding (evaluate compile-time expressions)   │\n");
        printf("│ • Copy propagation (replace variables with values)       │\n");
        printf("└──────────────────────────────────────────────────────────┘\n");
        optimizeTAC();
        printOptimizedTAC();

        // Save optimized TAC to file
        char optTacFile[256];
        sprintf(optTacFile, "%s.opt.tac", argv[1]);
        saveOptimizedTACToFile(optTacFile);
        printf("✓ Optimized TAC saved to: %s\n\n", optTacFile);

        /* PHASE 6: Code Generation */
        printf("┌──────────────────────────────────────────────────────────┐\n");
        printf("│ PHASE 6: MIPS CODE GENERATION                            │\n");
        printf("├──────────────────────────────────────────────────────────┤\n");
        printf("│ Translating to MIPS assembly:                            │\n");
        printf("│ • Variables stored on stack                              │\n");
        printf("│ • Using $t0-$t7 for temporary values                     │\n");
        printf("│ • System calls for print operations                      │\n");
        printf("└──────────────────────────────────────────────────────────┘\n");
        generateMIPS(root, argv[2]);
        printf("✓ MIPS assembly code generated to: %s\n", argv[2]);
        printf("\n");
        
        printf("╔════════════════════════════════════════════════════════════╗\n");
        printf("║                  COMPILATION SUCCESSFUL!                   ║\n");
        printf("║         Run the output file in a MIPS simulator            ║\n");
        printf("╚════════════════════════════════════════════════════════════╝\n");
    } else {
        printf("✗ Parse failed - check your syntax!\n");
        printf("Common errors:\n");
        printf("  • Missing semicolon after statements\n");
        printf("  • Undeclared variables\n");
        printf("  • Invalid syntax for print statements\n");
        return 1;
    }
    
    fclose(yyin);
    return 0;
}
