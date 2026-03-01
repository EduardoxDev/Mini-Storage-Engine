#ifndef WAL_H
#define WAL_H

#include <stdint.h>
#include <stdbool.h>

typedef enum {
    WAL_INSERT,
    WAL_UPDATE,
    WAL_DELETE
} WALOpType;

typedef struct WAL WAL;

WAL* wal_open(const char* filename);
void wal_close(WAL* wal);
bool wal_append(WAL* wal, WALOpType op, const char* key, const char* value);
void wal_checkpoint(WAL* wal);
void wal_replay(WAL* wal, void (*callback)(WALOpType op, const char* key, const char* value));

#endif
