/**
 * @file storage.h
 * @brief Main storage engine interface
 * 
 * This module provides the high-level API for the Mini Storage Engine.
 * It coordinates between the B-Tree index, pager, cache, and WAL to
 * provide ACID-compliant database operations.
 * 
 * Architecture:
 *   Storage Engine (this layer)
 *        ↓
 *   B-Tree Index + WAL
 *        ↓
 *   Pager + Cache
 *        ↓
 *   Disk Storage
 * 
 * @author Mini Storage Engine Contributors
 * @date 2026
 */

#ifndef STORAGE_H
#define STORAGE_H

#include <stdint.h>
#include <stdbool.h>

/** Page size in bytes (matches typical OS page size) */
#define PAGE_SIZE 4096

/** Maximum key length in bytes */
#define MAX_KEY_SIZE 256

/** Maximum value length in bytes */
#define MAX_VALUE_SIZE 1024

/**
 * @brief Database record structure
 * 
 * Records are stored in pages and indexed by the B-Tree.
 * Soft deletes are implemented using the is_deleted flag.
 */
typedef struct {
    char key[MAX_KEY_SIZE];      /**< Record key (null-terminated) */
    char value[MAX_VALUE_SIZE];  /**< Record value (null-terminated) */
    bool is_deleted;             /**< Tombstone flag for soft deletes */
} Record;

/** Opaque storage engine handle */
typedef struct StorageEngine StorageEngine;

/**
 * @brief Open or create a database file
 * 
 * Opens an existing database or creates a new one. Automatically
 * replays the Write-Ahead Log if present for crash recovery.
 * 
 * @param filename Path to database file
 * @return Storage engine handle, or NULL on failure
 * 
 * @note Caller must call storage_close() to ensure data is persisted
 */
StorageEngine* storage_open(const char* filename);

/**
 * @brief Close database and flush all changes
 * 
 * Flushes all dirty pages, checkpoints the WAL, and releases resources.
 * 
 * @param engine Storage engine handle
 */
void storage_close(StorageEngine* engine);

/**
 * @brief Insert a new key-value pair
 * 
 * Inserts a record into the database. The operation is logged to the
 * WAL before being applied to ensure durability.
 * 
 * @param engine Storage engine handle
 * @param key Record key (must be unique)
 * @param value Record value
 * @return true on success, false if key already exists
 * 
 * @note Time complexity: O(log n) average case
 */
bool storage_insert(StorageEngine* engine, const char* key, const char* value);

/**
 * @brief Retrieve value for a given key
 * 
 * Searches the B-Tree index for the key and retrieves the value.
 * Benefits from cache if the page is in memory.
 * 
 * @param engine Storage engine handle
 * @param key Record key to search for
 * @param value_out Buffer to store retrieved value (must be >= MAX_VALUE_SIZE)
 * @return true if found, false if key doesn't exist or is deleted
 * 
 * @note Time complexity: O(log n) average case, O(1) if cached
 */
bool storage_select(StorageEngine* engine, const char* key, char* value_out);

/**
 * @brief Delete a record (soft delete)
 * 
 * Marks the record as deleted without removing it from the B-Tree.
 * The operation is logged to the WAL.
 * 
 * @param engine Storage engine handle
 * @param key Record key to delete
 * @return true on success, false if key doesn't exist
 * 
 * @note Time complexity: O(log n)
 * @note Deleted records can be reclaimed by vacuum (future feature)
 */
bool storage_delete(StorageEngine* engine, const char* key);

/**
 * @brief Update an existing record
 * 
 * Updates the value for an existing key. The operation is logged
 * to the WAL before being applied.
 * 
 * @param engine Storage engine handle
 * @param key Record key to update
 * @param value New value
 * @return true on success, false if key doesn't exist
 * 
 * @note Time complexity: O(log n)
 */
bool storage_update(StorageEngine* engine, const char* key, const char* value);

/**
 * @brief Scan all records in the database
 * 
 * Performs an in-order traversal of the B-Tree, printing all
 * non-deleted records.
 * 
 * @param engine Storage engine handle
 * 
 * @note Time complexity: O(n)
 * @note Output is sorted by key
 */
void storage_scan(StorageEngine* engine);

#endif /* STORAGE_H */
