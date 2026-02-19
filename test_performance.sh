echo "=== Compiler Performance Benchmark ==="
echo "Platform: $(uname -s)"
echo "CPU: $(sysctl -n machdep.cpu.brand_string 2>/dev/null || lscpu | grep 'Model name' | cut -d: -f2)"
echo ""

# Test files of increasing complexity
cat > test_small.c << 'EOF'
int x;
int y;
x = 10;
y = 20;
print(x + y);
EOF

cat > test_medium.c << 'EOF'
int a; int b; int c; int d; int e;
a = 1; b = 2; c = 3; d = 4; e = 5;
int result;
result = a + b + c + d + e;
print(result);
# Repeat 50 times...
EOF

# Generate large test file
echo "Generating large test file..."
echo "" > test_large.c
for i in {1..1000}; do
    echo "int var$i;" >> test_large.c
done
for i in {1..1000}; do
    echo "var$i = $i;" >> test_large.c
done
echo "print(var1 + var1000);" >> test_large.c

# Run benchmarks
echo "=== BASELINE PERFORMANCE ==="
echo "Small file:"
time ./minicompiler_original < test_small.c > /dev/null

echo "Medium file:"
time ./minicompiler_original < test_medium.c > /dev/null

echo "Large file:"
time ./minicompiler_original < test_large.c > /dev/null

echo ""
echo "=== OPTIMIZED PERFORMANCE ==="
echo "Small file:"
time ./minicompiler_optimized < test_small.c > /dev/null

echo "Medium file:"
time ./minicompiler_optimized < test_medium.c > /dev/null

echo "Large file:"
time ./minicompiler_optimized < test_large.c > /dev/null

# Platform-specific profiling
if [[ "$OSTYPE" == "darwin"* ]]; then
    echo ""
    echo "=== macOS Instruments Profiling ==="
    echo "Run: instruments -t 'Time Profiler' ./minicompiler_optimized"
else
    echo ""
    echo "=== Linux perf Profiling ==="
    echo "Run: perf record -g ./minicompiler_optimized < test_large.c"
    echo "     perf report"
fi
