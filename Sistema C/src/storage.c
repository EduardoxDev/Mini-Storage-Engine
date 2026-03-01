#include "storage.h"
#include "pager.h"
#include "btree.h"
#include "wal.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

struct StorageEngine {
    Pager* pager;
    BTree* index;
    WAL* wal;
    uint32_t next_record_id;
};

StorageEngine* storage_open(const char* filename) {
    StorageEngine* engine = malloc(sizeof(StorageEngine));
    if (!engine) return NULL;
    
    engine->pager = pager_open(filename);
    if (!engine->pager) {
        free(engine);
        return NULL;
    }
    
    engine->index = btree_create(engine->pager);
    if (!engine->index) {
        pager_close(engine->pager);
        free(engine);
        return NULL;
    }
    
    char wal_filename[512];
    snprintf(wal_filename, sizeof(wal_filename), "%s.wal", filename);
    engine->wal = wal_open(wal_filename);
    
    engine->next_record_id = 0;
    
    return engine;
}

void storage_close(StorageEngine* engine) {
    if (!engine) return;
    
    wal_checkpoint(engine->wal);
    wal_close(engine->wal);
    pager_flush_all(engine->pager);
    btree_destroy(engine->index);
    pager_close(engine->pager);
    free(engine);
}

bool storage_insert(StorageEngine* engine, const char* key, const char* value) {
    uint32_t existing;
    if (btree_search(engine->index, key, &existing)) {
        return false;
    }
    
    uint32_t record_page = pager_allocate_page(engine->pager);
    Page* page = pager_get_page(engine->pager, record_page);
    
    Record* record = (Record*)page->data;
    strncpy(record->key, key, MAX_KEY_SIZE - 1);
    record->key[MAX_KEY_SIZE - 1] = '\0';
    strncpy(record->value, value, MAX_VALUE_SIZE - 1);
    record->value[MAX_VALUE_SIZE - 1] = '\0';
    record->is_deleted = false;
    
    btree_insert(engine->index, key, record_page);
    pager_flush_page(engine->pager, record_page);
    
    wal_append(engine->wal, WAL_INSERT, key, value);
    
    return true;
}

bool storage_select(StorageEngine* engine, const char* key, char* value_out) {
    uint32_t record_page;
    if (!btree_search(engine->index, key, &record_page)) {
        return false;
    }
    
    Page* page = pager_get_page(engine->pager, record_page);
    Record* record = (Record*)page->data;
    
    if (record->is_deleted) {
        return false;
    }
    
    strcpy(value_out, record->value);
    return true;
}

bool storage_delete(StorageEngine* engine, const char* key) {
    uint32_t record_page;
    if (!btree_search(engine->index, key, &record_page)) {
        return false;
    }
    
    Page* page = pager_get_page(engine->pager, record_page);
    Record* record = (Record*)page->data;
    record->is_deleted = true;
    
    pager_flush_page(engine->pager, record_page);
    wal_append(engine->wal, WAL_DELETE, key, NULL);
    
    return true;
}

bool storage_update(StorageEngine* engine, const char* key, const char* value) {
    uint32_t record_page;
    if (!btree_search(engine->index, key, &record_page)) {
        return false;
    }
    
    Page* page = pager_get_page(engine->pager, record_page);
    Record* record = (Record*)page->data;
    
    if (record->is_deleted) {
        return false;
    }
    
    strncpy(record->value, value, MAX_VALUE_SIZE - 1);
    record->value[MAX_VALUE_SIZE - 1] = '\0';
    
    pager_flush_page(engine->pager, record_page);
    wal_append(engine->wal, WAL_UPDATE, key, value);
    
    return true;
}

static void scan_callback(const char* key, uint32_t record_page) {
    printf("Key: %s (Page: %u)\n", key, record_page);
}

void storage_scan(StorageEngine* engine) {
    btree_scan(engine->index, scan_callback);
}
