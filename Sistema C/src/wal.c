#include "wal.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_KEY_SIZE 256
#define MAX_VALUE_SIZE 1024

typedef struct {
    WALOpType op;
    char key[MAX_KEY_SIZE];
    char value[MAX_VALUE_SIZE];
} WALEntry;

struct WAL {
    FILE* file;
    uint64_t offset;
};

WAL* wal_open(const char* filename) {
    WAL* wal = malloc(sizeof(WAL));
    if (!wal) return NULL;
    
    wal->file = fopen(filename, "ab+");
    if (!wal->file) {
        free(wal);
        return NULL;
    }
    
    fseek(wal->file, 0, SEEK_END);
    wal->offset = ftell(wal->file);
    
    return wal;
}

void wal_close(WAL* wal) {
    if (!wal) return;
    
    if (wal->file) {
        fflush(wal->file);
        fclose(wal->file);
    }
    free(wal);
}

bool wal_append(WAL* wal, WALOpType op, const char* key, const char* value) {
    WALEntry entry;
    entry.op = op;
    strncpy(entry.key, key, MAX_KEY_SIZE - 1);
    entry.key[MAX_KEY_SIZE - 1] = '\0';
    
    if (value) {
        strncpy(entry.value, value, MAX_VALUE_SIZE - 1);
        entry.value[MAX_VALUE_SIZE - 1] = '\0';
    } else {
        entry.value[0] = '\0';
    }
    
    size_t written = fwrite(&entry, sizeof(WALEntry), 1, wal->file);
    fflush(wal->file);
    
    return written == 1;
}

void wal_checkpoint(WAL* wal) {
    if (!wal || !wal->file) return;
    
    fflush(wal->file);
    fclose(wal->file);
    
    wal->file = fopen("temp.wal", "wb");
    wal->offset = 0;
}

void wal_replay(WAL* wal, void (*callback)(WALOpType op, const char* key, const char* value)) {
    if (!wal || !wal->file) return;
    
    fseek(wal->file, 0, SEEK_SET);
    
    WALEntry entry;
    while (fread(&entry, sizeof(WALEntry), 1, wal->file) == 1) {
        callback(entry.op, entry.key, entry.value);
    }
    
    fseek(wal->file, 0, SEEK_END);
}
