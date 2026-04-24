
/* =========================================================
 * test_final.c  ���  CST-405 Final Compiler Test
 *
 * Exercises every major feature of the extended C-minus
 * compiler.  Compile with:
 *
 *   ./minicompiler test_final.c test_final.s
 *
 * Run on MIPS simulator:
 *
 *   spim -file test_final.s        (unoptimized)
 *   ./minicompiler --optimize test_final.c test_final_opt.s
 *   spim -file test_final_opt.s    (optimized)
 *
 * Expected output is annotated on each print() call.
 * =========================================================*/


/* =========================================================
 * SECTION 1 ��� VARIABLES & TYPES
 * =========================================================*/

/* Global integers */
int gCount;
int gTotal;

/* Global int array */
int gScores[5];

int main() {

    /* ������ 1a. Integer variables ������������������������������������������������������������������������������������ */
    int a;
    int b;
    int c;
    a = 42;
    b = 8;
    c = a - b;
    print(a);        /* 42  */
    print(b);        /* 8   */
    print(c);        /* 34  */

    /* ������ 1b. Float variables ������������������������������������������������������������������������������������������ */
    float x;
    float y;
    float z;
    x = 3.14;
    y = 2.0;
    z = x * y;
    print(x);        /* 3.14  */
    print(y);        /* 2.0   */
    print(z);        /* 6.28  */

    /* ������ 1c. Boolean variables ������������������������������������������������������������������������������������ */
    boolean flag;
    boolean done;
    flag = true;
    done = false;
    print(flag);     /* 1 (true)  */
    print(done);     /* 0 (false) */

    /* ������ 1d. Char variables ��������������������������������������������������������������������������������������������� */
    char grade;
    char init;
    grade = 'A';
    init  = 'Z';
    print(grade);    /* A */
    print(init);     /* Z */

    /* ������ 1e. String variables ��������������������������������������������������������������������������������������� */
    string name;
    string msg;
    name = "Alice\n";
    msg  = "Hello\n";
    output_string(name);     /* Alice */
    output_string(msg);      /* Hello */

    /* ������ 1f. Integer array ������������������������������������������������������������������������������������������������ */
    int nums[6];
    nums[0] = 10;
    nums[1] = 20;
    nums[2] = 30;
    nums[3] = 40;
    nums[4] = 50;
    nums[5] = 60;
    print(nums[0]);  /* 10 */
    print(nums[3]);  /* 40 */
    print(nums[5]);  /* 60 */

    /* ������ 1g. Float array ������������������������������������������������������������������������������������������������������ */
    float fArr[3];
    fArr[0] = 1.1;
    fArr[1] = 2.2;
    fArr[2] = 3.3;
    print(fArr[0]);  /* 1.1 */
    print(fArr[2]);  /* 3.3 */


/* =========================================================
 * SECTION 2 ��� FUNCTIONS
 * =========================================================*/

    /* ������ 2a. void function (no parameters, no return) ��������������� */
    greet();

    /* ������ 2b. int function with no parameters ������������������������������������������ */
    int magic;
    magic = getAnswer();
    print(magic);    /* 42 */

    /* ������ 2c. int function with int parameters ��������������������������������������� */
    int sq;
    sq = square(7);
    print(sq);       /* 49 */

    /* ������ 2d. float function with mixed parameters ������������������������������ */
    float avg;
    avg = average(10, 3);
    print(avg);      /* ~3.33  (10 / 3 as float) */

    /* ������ 2e. Function that modifies a global ��������������������������������������������� */
    gCount = 0;
    increment();
    increment();
    increment();
    print(gCount);   /* 3 */

    /* ������ 2f. Recursive function ������������������������������������������������������������������������������������ */
    int fact;
    fact = factorial(5);
    print(fact);     /* 120 */

    /* ������ 2g. Function receiving an array ��������������������������������������������������������� */
    int arr[5];
    int s;
    arr[0] = 1;  arr[1] = 2;  arr[2] = 3;  arr[3] = 4;  arr[4] = 5;
    s = sumArray(arr, 5);
    print(s);        /* 15 */

    /* ������ 2h. Function returning a boolean ��������������������������������������������������� */
    boolean even;
    even = isEven(4);
    print(even);     /* 1 (true) */
    even = isEven(7);
    print(even);     /* 0 (false) */


/* =========================================================
 * SECTION 3 ��� LOOPS
 * =========================================================*/

    /* ������ 3a. while loop: sum 1..10 ��������������������������������������������������������������������������� */
    int i;
    int total;
    i = 1;
    total = 0;
    while (i <= 10) {
        total = total + i;
        i = i + 1;
    }
    print(total);    /* 55 */

    /* ������ 3b. for loop: sum 1..5 ������������������������������������������������������������������������������������ */
    int j;
    int fsum;
    fsum = 0;
    for (j = 1; j <= 5; j = j + 1) {
        fsum = fsum + j;
    }
    print(fsum);     /* 15 */

    /* ������ 3c. for loop: fill + print array ������������������������������������������������������ */
    int data[5];
    for (j = 0; j < 5; j = j + 1) {
        data[j] = j * j;
    }
    for (j = 0; j < 5; j = j + 1) {
        print(data[j]);   /* 0 1 4 9 16 */
    }

    /* ������ 3d. nested loops: multiplication table (2x3) ��������������� */
    int row;
    int col;
    int product;
    for (row = 1; row <= 2; row = row + 1) {
        for (col = 1; col <= 3; col = col + 1) {
            product = row * col;
            print(product);  /* 1 2 3  2 4 6 */
        }
    }

    /* ������ 3e. while loop inside a for loop ������������������������������������������������������ */
    int k;
    int acc;
    for (j = 1; j <= 3; j = j + 1) {
        k = 0;
        acc = 0;
        while (k < j) {
            acc = acc + 1;
            k = k + 1;
        }
        print(acc);   /* 1  2  3 */
    }


/* =========================================================
 * SECTION 4 ��� DECISIONS
 * =========================================================*/

    /* ������ 4a. simple if ��������������������������������������������������������������������������������������������������������������� */
    int val;
    val = 10;
    if (val > 5) {
        print(100);  /* 100 */
    }

    /* ������ 4b. if-else ��������������������������������������������������������������������������������������������������������������������� */
    val = 3;
    if (val > 5) {
        print(200);
    } else {
        print(201);  /* 201 */
    }

    /* ������ 4c. if-else-if chain ������������������������������������������������������������������������������������������ */
    val = 75;
    if (val >= 90) {
        print(300);      /* A */
    } else if (val >= 80) {
        print(301);      /* B */
    } else if (val >= 70) {
        print(302);  /* 302 ��� C range */
    } else {
        print(303);      /* F */
    }

    /* ������ 4d. nested if ��������������������������������������������������������������������������������������������������������������� */
    int p;
    int q;
    p = 5;
    q = 10;
    if (p < q) {
        if (p > 0) {
            print(400);  /* 400 */
        } else {
            print(401);
        }
    } else {
        print(402);
    }

    /* ������ 4e. switch: simple dispatch ������������������������������������������������������������������ */
    int day;
    day = 3;
    switch (day) {
        case 1: print(501); break;
        case 2: print(502); break;
        case 3: print(503); break;  /* 503 */
        default: print(500); break;
    }

    /* ������ 4f. switch: fall-through ������������������������������������������������������������������������������ */
    int season;
    season = 2;
    switch (season) {
        case 1:
        case 2:
        case 3: print(510); break;   /* 510 ��� spring/summer/fall */
        case 4: print(511); break;
        default: print(512); break;
    }

    /* ������ 4g. switch: default clause ��������������������������������������������������������������������� */
    int code;
    code = 99;
    switch (code) {
        case 1: print(520); break;
        case 2: print(521); break;
        default: print(522); break;  /* 522 */
    }

    /* ������ 4h. nested switch ��������������������������������������������������������������������������������������������������� */
    int outer;
    int inner;
    outer = 2;
    inner = 1;
    switch (outer) {
        case 1:
            switch (inner) {
                case 1: print(531); break;
                default: print(530); break;
            }
            break;
        case 2:
            switch (inner) {
                case 1: print(532); break;  /* 532 */
                default: print(530); break;
            }
            break;
        default: print(530); break;
    }


/* =========================================================
 * SECTION 5 ��� INTEGRATION: puts everything together
 *   Fibonacci sequence via loop, then via recursion
 * =========================================================*/

    /* ������ 5a. Fibonacci loop (first 7 terms) ������������������������������������������������ */
    int fibA;
    int fibB;
    int fibNext;
    int n;
    fibA = 0;
    fibB = 1;
    print(fibA);  /* 0 */
    print(fibB);  /* 1 */
    n = 2;
    while (n < 7) {
        fibNext = fibA + fibB;
        print(fibNext);   /* 1 2 3 5 8 */
        fibA = fibB;
        fibB = fibNext;
        n = n + 1;
    }

    /* ������ 5b. Fibonacci via recursion (fib(6) = 8) ������������������������������ */
    int fibR;
    fibR = fib(6);
    print(fibR);  /* 8 */


/* =========================================================
 * SECTION 6 ��� PERFORMANCE COMPARISON
 *   The compiler must emit TAC for both unoptimized and
 *   optimized passes.  This section provides a loop-heavy
 *   computation so the difference in instruction count is
 *   clearly visible.
 * =========================================================*/

    /* ������ 6a. unoptimized target: constant-heavy expression ��� */
    int perf;
    perf = 2 * 3 + 4 * 5 - 1;    /* constant folding opportunity */
    print(perf);   /* 25 */

    /* ������ 6b. loop with invariant calculation ��������������������������������������������� */
    int limit;
    int lcount;
    limit = 100;
    lcount = 0;
    for (i = 0; i < limit; i = i + 1) {
        lcount = lcount + 1;
    }
    print(lcount);  /* 100 */

    /* ������ 6c. dead-code target (unreachable branch) ������������������������������ */
    int dc;
    dc = 1;
    if (dc == 1) {
        print(601);  /* 601 ��� only this executes */
    } else {
        print(602);  /* dead code */
    }

    return 0;
}


/* =========================================================
 * FUNCTION DEFINITIONS
 * =========================================================*/

/* void ��� no parameters, no return value */
void greet() {
    print(999);   /* sentinel: 999 */
}

/* int ��� no parameters */
int getAnswer() {
    return 42;
}

/* int ��� int parameter */
int square(int n) {
    int result;
    result = n * n;
    return result;
}

/* float ��� int parameters, float return */
float average(int numerator, int denominator) {
    float r;
    r = numerator / denominator;
    return r;
}

/* void ��� modifies global */
void increment() {
    gCount = gCount + 1;
}

/* int ��� recursive (factorial) */
int factorial(int n) {
    if (n <= 1) {
        return 1;
    }
    int sub;
    sub = n - 1;
    return n * factorial(sub);
}

/* int ��� array parameter */
int sumArray(int arr[], int size) {
    int total;
    int idx;
    total = 0;
    for (idx = 0; idx < size; idx = idx + 1) {
        total = total + arr[idx];
    }
    return total;
}

/* boolean ��� return type */
boolean isEven(int n) {
    if (n == n / 2 * 2) {
        return true;
    }
    return false;
}

/* int ��� recursive Fibonacci */
int fib(int n) {
    if (n <= 0) { return 0; }
    if (n == 1) { return 1; }
    int a;
    int b;
    a = n - 1;
    b = n - 2;
    return fib(a) + fib(b);
}
