#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <sys/time.h>
#include <sys/resource.h>

#ifdef __APPLE__
    #include <mach/mach.h>
    #include <mach/mach_time.h>
#else
    #include <unistd.h>
#endif

typedef struct {
    double cpu_time;
    double wall_time;
    long memory_usage;
    long peak_memory;
} BenchmarkResult;

// Platform-specific memory measurement
long get_memory_usage() {
#ifdef __APPLE__
    struct task_basic_info info;
    mach_msg_type_number_t size = sizeof(info);
    kern_return_t kerr = task_info(mach_task_self(),
                                    TASK_BASIC_INFO,
                                    (task_info_t)&info, &size);
    if(kerr == KERN_SUCCESS) {
        return info.resident_size;
    }
#else
    FILE* file = fopen("/proc/self/status", "r");
    if (file) {
        char line[128];
        while (fgets(line, 128, file)) {
            if (strncmp(line, "VmRSS:", 6) == 0) {
                long memory;
                sscanf(line, "VmRSS: %ld", &memory);
                fclose(file);
                return memory * 1024; // Convert KB to bytes
            }
        }
        fclose(file);
    }
#endif
    return 0;
}

BenchmarkResult* start_benchmark() {
    BenchmarkResult* result = malloc(sizeof(BenchmarkResult));

    // Get initial CPU time
    clock_t start_cpu = clock();

    // Get initial wall time
    struct timeval start_wall;
    gettimeofday(&start_wall, NULL);

    // Store in result (temporarily using these fields)
    result->cpu_time = (double)start_cpu;
    result->wall_time = start_wall.tv_sec + start_wall.tv_usec / 1000000.0;
    result->memory_usage = get_memory_usage();

    return result;
}

void end_benchmark(BenchmarkResult* result, const char* phase) {
    // Get final times
    clock_t end_cpu = clock();
    struct timeval end_wall;
    gettimeofday(&end_wall, NULL);

    // Calculate elapsed times
    double start_cpu = result->cpu_time;
    result->cpu_time = ((double)(end_cpu - start_cpu)) / CLOCKS_PER_SEC;

    double start_wall = result->wall_time;
    double end_wall_time = end_wall.tv_sec + end_wall.tv_usec / 1000000.0;
    result->wall_time = end_wall_time - start_wall;

    // Get final memory
    long final_memory = get_memory_usage();
    result->peak_memory = final_memory - result->memory_usage;

    // Print results
    printf("\n=== %s Performance ===\n", phase);
    printf("CPU Time: %.6f seconds\n", result->cpu_time);
    printf("Wall Time: %.6f seconds\n", result->wall_time);
    printf("Memory Delta: %.2f KB\n", result->peak_memory / 1024.0);
}
