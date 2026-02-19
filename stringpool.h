#define STRING_POOL_SIZE 1024
#define STRING_HASH_SIZE 127

typedef struct StringNode {
    char* str;
    struct StringNode* next;
} StringNode;

typedef struct {
    StringNode* buckets[STRING_HASH_SIZE];
    char* pool;
    size_t pool_used;
    int unique_strings;
    int total_requests;
} StringPool;

extern StringPool string_pool;

void init_string_pool();
char* intern_string(const char* str);
void print_string_stats();
