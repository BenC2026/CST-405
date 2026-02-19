#include "stringpool.h"

StringPool string_pool = {0};

void init_string_pool() {
    string_pool.pool = malloc(STRING_POOL_SIZE);
    string_pool.pool_used = 0;
    string_pool.unique_strings = 0;
    string_pool.total_requests = 0;
    memset(string_pool.buckets, 0, sizeof(string_pool.buckets));
}

char* intern_string(const char* str) {
    string_pool.total_requests++;

    unsigned int hash = 0;
    const char* p = str;
    while(*p) {
        hash = hash * 31 + *p++;
    }
    hash %= STRING_HASH_SIZE;

    // Check if already interned
    StringNode* node = string_pool.buckets[hash];
    while(node) {
        if(strcmp(node->str, str) == 0) {
            return node->str;  // Return existing
        }
        node = node->next;
    }

    // Add new string
    size_t len = strlen(str) + 1;
    if(string_pool.pool_used + len > STRING_POOL_SIZE) {
        // In production, would allocate new pool
        return strdup(str);  // Fallback
    }

    char* interned = string_pool.pool + string_pool.pool_used;
    strcpy(interned, str);
    string_pool.pool_used += len;

    // Add to hash table
    StringNode* new_node = malloc(sizeof(StringNode));
    new_node->str = interned;
    new_node->next = string_pool.buckets[hash];
    string_pool.buckets[hash] = new_node;

    string_pool.unique_strings++;
    return interned;
}
