#ifndef PAGER_H
#define PAGER_H

#include <stdint.h>
#include <stdio.h>

#define PAGE_SIZE 4096

typedef struct {
    uint8_t data[PAGE_SIZE];
} Page;

typedef struct Pager Pager;

Pager* pager_open(const char* filename);
void pager_close(Pager* pager);
Page* pager_get_page(Pager* pager, uint32_t page_num);
void pager_flush_page(Pager* pager, uint32_t page_num);
void pager_flush_all(Pager* pager);
uint32_t pager_allocate_page(Pager* pager);

#endif
