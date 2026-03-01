#include "pager.h"
#include "cache.h"
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>

struct Pager {
    FILE* file;
    uint32_t num_pages;
    Cache* cache;
};

Pager* pager_open(const char* filename) {
    Pager* pager = malloc(sizeof(Pager));
    if (!pager) return NULL;
    
    pager->file = fopen(filename, "rb+");
    if (!pager->file) {
        pager->file = fopen(filename, "wb+");
        if (!pager->file) {
            free(pager);
            return NULL;
        }
    }
    
    fseek(pager->file, 0, SEEK_END);
    long file_size = ftell(pager->file);
    pager->num_pages = file_size / PAGE_SIZE;
    
    pager->cache = cache_create(100);
    
    return pager;
}

void pager_close(Pager* pager) {
    if (!pager) return;
    
    pager_flush_all(pager);
    cache_destroy(pager->cache);
    
    if (pager->file) {
        fclose(pager->file);
    }
    free(pager);
}

Page* pager_get_page(Pager* pager, uint32_t page_num) {
    Page* page = cache_get(pager->cache, page_num);
    if (page) return page;
    
    page = malloc(sizeof(Page));
    if (!page) return NULL;
    
    if (page_num < pager->num_pages) {
        fseek(pager->file, page_num * PAGE_SIZE, SEEK_SET);
        fread(page->data, PAGE_SIZE, 1, pager->file);
    } else {
        memset(page->data, 0, PAGE_SIZE);
    }
    
    cache_put(pager->cache, page_num, page);
    return page;
}

void pager_flush_page(Pager* pager, uint32_t page_num) {
    Page* page = cache_get(pager->cache, page_num);
    if (!page) return;
    
    fseek(pager->file, page_num * PAGE_SIZE, SEEK_SET);
    fwrite(page->data, PAGE_SIZE, 1, pager->file);
    fflush(pager->file);
}

void pager_flush_all(Pager* pager) {
    for (uint32_t i = 0; i < pager->num_pages; i++) {
        pager_flush_page(pager, i);
    }
}

uint32_t pager_allocate_page(Pager* pager) {
    return pager->num_pages++;
}
