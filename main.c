/* MINIMAL C COMPILER - EDUCATIONAL VERSION
 * Demonstrates all phases of compilation with a simple language
 * Supports: int variables, addition, assignment, print
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "ast.h"
#include "semantic.h"
#include "codegen.h"
#include "tac.h"
#include "benchmark.c"
#include "symtab.h"

extern int yyparse();
extern FILE* yyin;
extern ASTNode* root;

/* Helper function to generate TAC filenames from output filename */
void getTACFilename(const char* outputFile, char* tacFile, char* optimizedTacFile) {
    // Find the last dot in the filename
    const char* dot = strrchr(outputFile, '.');
    const char* slash = strrchr(outputFile, '/');

    if (dot && (!slash || dot > slash)) {
        // Copy everything before the extension
        int baseLen = dot - outputFile;
        strncpy(tacFile, outputFile, baseLen);
        tacFile[baseLen] = '\0';
        strcat(tacFile, ".tac");

        strncpy(optimizedTacFile, outputFile, baseLen);
        optimizedTacFile[baseLen] = '\0';
        strcat(optimizedTacFile, ".optimized.tac");
    } else {
        // No extension found, just append
        sprintf(tacFile, "%s.tac", outputFile);
        sprintf(optimizedTacFile, "%s.optimized.tac", outputFile);
    }
}

void test_symbol_table_performance() {
    BenchmarkResult* bench = start_benchmark();

    // Test with 1000 variables
    for(int i = 0; i < 1000; i++) {
        char varname[20];
        sprintf(varname, "var_%d", i);
        addVar(varname, NULL);
    }

    // Test lookups
    for(int i = 0; i < 10000; i++) {
        char varname[20];
        sprintf(varname, "var_%d", rand() % 1000);
        getVarOffset(varname);
    }

    end_benchmark(bench, "Symbol Table");
    printf("Collisions: %d\n", symtab.collisions);
    free(bench);
}

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
    
    init_ast_memory();
    if (yyparse() == 0) {
        printf("✓ Parse successful - program is syntactically correct!\n\n");
        
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
        printf("│ Checking program semantics:                              │\n");
        printf("│ • Variable declarations and usage                        │\n");
        printf("│ • Type compatibility                                     │\n");
        printf("│ • Duplicate declarations                                 │\n");
        printf("└──────────────────────────────────────────────────────────┘\n");
        initSemantic();
        int semResult = performSemanticAnalysis(root);
        printSemanticSummary();

        if (semResult != 0) {
            printf("✗ Compilation failed due to semantic errors\n");
            fclose(yyin);
            return 1;
        }

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
        printTempAllocatorState();

        // Generate TAC filename and save to file
        char tacFile[256], optimizedTacFile[256];
        getTACFilename(argv[2], tacFile, optimizedTacFile);
        saveTACToFile(tacFile);

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
        printf("\n");

        // Save optimized TAC to file
        saveOptimizedTACToFile(optimizedTacFile);

        /* PHASE 6: Code Generation */
        printf("┌──────────────────────────────────────────────────────────┐\n");
        printf("│ PHASE 6: MIPS CODE GENERATION                            │\n");
        printf("├──────────────────────────────────────────────────────────┤\n");
        printf("│ Translating OPTIMIZED TAC to MIPS assembly:              │\n");
        printf("│ • Using optimized TAC (with constant folding)            │\n");
        printf("│ • Variables stored on stack                              │\n");
        printf("│ • Register allocation with LRU spilling                  │\n");
        printf("│ • System calls for print operations                      │\n");
        printf("└──────────────────────────────────────────────────────────┘\n");
        generateMIPSFromTAC(argv[2]);
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
