#ifndef CACHE_H
#define CACHE_H

#include "pager.h"
#include <stdint.h>

typedef struct Cache Cache;

Cache* cache_create(uint32_t capacity);
void cache_destroy(Cache* cache);
Page* cache_get(Cache* cache, uint32_t page_num);
void cache_put(Cache* cache, uint32_t page_num, Page* page);
void cache_evict(Cache* cache, uint32_t page_num);

#endif
