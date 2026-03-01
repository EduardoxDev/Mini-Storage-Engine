#include "cache.h"
#include <stdlib.h>
#include <string.h>

typedef struct CacheNode {
    uint32_t page_num;
    Page* page;
    struct CacheNode* prev;
    struct CacheNode* next;
} CacheNode;

struct Cache {
    CacheNode* head;
    CacheNode* tail;
    CacheNode** hash_table;
    uint32_t capacity;
    uint32_t size;
};

Cache* cache_create(uint32_t capacity) {
    Cache* cache = malloc(sizeof(Cache));
    if (!cache) return NULL;
    
    cache->capacity = capacity;
    cache->size = 0;
    cache->head = NULL;
    cache->tail = NULL;
    cache->hash_table = calloc(capacity * 2, sizeof(CacheNode*));
    
    return cache;
}

void cache_destroy(Cache* cache) {
    if (!cache) return;
    
    CacheNode* current = cache->head;
    while (current) {
        CacheNode* next = current->next;
        free(current->page);
        free(current);
        current = next;
    }
    
    free(cache->hash_table);
    free(cache);
}

static void move_to_front(Cache* cache, CacheNode* node) {
    if (node == cache->head) return;
    
    if (node->prev) node->prev->next = node->next;
    if (node->next) node->next->prev = node->prev;
    if (node == cache->tail) cache->tail = node->prev;
    
    node->prev = NULL;
    node->next = cache->head;
    if (cache->head) cache->head->prev = node;
    cache->head = node;
    if (!cache->tail) cache->tail = node;
}

Page* cache_get(Cache* cache, uint32_t page_num) {
    uint32_t hash = page_num % (cache->capacity * 2);
    CacheNode* node = cache->hash_table[hash];
    
    while (node) {
        if (node->page_num == page_num) {
            move_to_front(cache, node);
            return node->page;
        }
        node = node->next;
    }
    
    return NULL;
}

void cache_put(Cache* cache, uint32_t page_num, Page* page) {
    uint32_t hash = page_num % (cache->capacity * 2);
    
    CacheNode* existing = cache->hash_table[hash];
    while (existing) {
        if (existing->page_num == page_num) {
            move_to_front(cache, existing);
            return;
        }
        existing = existing->next;
    }
    
    if (cache->size >= cache->capacity) {
        CacheNode* lru = cache->tail;
        if (lru) {
            cache_evict(cache, lru->page_num);
        }
    }
    
    CacheNode* node = malloc(sizeof(CacheNode));
    node->page_num = page_num;
    node->page = page;
    node->prev = NULL;
    node->next = cache->head;
    
    if (cache->head) cache->head->prev = node;
    cache->head = node;
    if (!cache->tail) cache->tail = node;
    
    node->next = cache->hash_table[hash];
    cache->hash_table[hash] = node;
    cache->size++;
}

void cache_evict(Cache* cache, uint32_t page_num) {
    uint32_t hash = page_num % (cache->capacity * 2);
    CacheNode* node = cache->hash_table[hash];
    CacheNode* prev = NULL;
    
    while (node) {
        if (node->page_num == page_num) {
            if (prev) prev->next = node->next;
            else cache->hash_table[hash] = node->next;
            
            if (node->prev) node->prev->next = node->next;
            if (node->next) node->next->prev = node->prev;
            if (node == cache->head) cache->head = node->next;
            if (node == cache->tail) cache->tail = node->prev;
            
            free(node->page);
            free(node);
            cache->size--;
            return;
        }
        prev = node;
        node = node->next;
    }
}
