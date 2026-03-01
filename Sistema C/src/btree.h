/**
 * @file btree.h
 * @brief B-Tree index implementation
 * 
 * Implements a self-balancing B-Tree for efficient key-value indexing.
 * The B-Tree maintains sorted order and guarantees O(log n) operations.
 * 
 * Properties:
 * - Order: 128 (up to 127 keys per node, 128 children)
 * - Self-balancing through node splitting
 * - All leaves at same depth
 * - Efficient range scans through leaf linking
 * 
 * @author Mini Storage Engine Contributors
 * @date 2026
 */

#ifndef BTREE_H
#define BTREE_H

#include "pager.h"
#include <stdint.h>
#include <stdbool.h>

/** 
 * B-Tree order (maximum children per node)
 * Higher order = shorter tree but larger nodes
 * Order 128 provides good balance for 4KB pages
 */
#define BTREE_ORDER 128

/** Maximum key size in bytes */
#define MAX_KEY_SIZE 256

/** Opaque B-Tree handle */
typedef struct BTree BTree;

/**
 * @brief Create a new B-Tree index
 * 
 * Initializes a B-Tree with an empty root node. The root is
 * allocated as a leaf node and will be converted to internal
 * node as the tree grows.
 * 
 * @param pager Pager instance for page management
 * @return B-Tree handle, or NULL on failure
 */
BTree* btree_create(Pager* pager);

/**
 * @brief Destroy B-Tree and release resources
 * 
 * Frees the B-Tree structure. Does not free pages (managed by pager).
 * 
 * @param tree B-Tree handle
 */
void btree_destroy(BTree* tree);

/**
 * @brief Insert key-value pair into B-Tree
 * 
 * Inserts a new key-value pair, maintaining B-Tree properties.
 * If nodes become full during insertion, they are split to
 * maintain balance.
 * 
 * Algorithm:
 * 1. Search for insertion point
 * 2. If leaf is full, split recursively up to root
 * 3. Insert key-value in sorted position
 * 
 * @param tree B-Tree handle
 * @param key Key to insert (must be unique)
 * @param value Page number where record is stored
 * @return true on success, false on failure
 * 
 * @note Time complexity: O(log n) average, O(log n + split cost) worst
 * @note Does not check for duplicate keys (caller's responsibility)
 */
bool btree_insert(BTree* tree, const char* key, uint32_t value);

/**
 * @brief Search for a key in the B-Tree
 * 
 * Performs binary search within nodes while traversing from root
 * to leaf. Returns the page number where the record is stored.
 * 
 * @param tree B-Tree handle
 * @param key Key to search for
 * @param value_out Output parameter for page number
 * @return true if found, false otherwise
 * 
 * @note Time complexity: O(log n)
 */
bool btree_search(BTree* tree, const char* key, uint32_t* value_out);

/**
 * @brief Delete a key from the B-Tree
 * 
 * @param tree B-Tree handle
 * @param key Key to delete
 * @return true on success, false if key not found
 * 
 * @note Currently not implemented (returns false)
 * @todo Implement deletion with node merging/redistribution
 */
bool btree_delete(BTree* tree, const char* key);

/**
 * @brief Scan all keys in sorted order
 * 
 * Performs in-order traversal of the B-Tree, calling the callback
 * function for each key-value pair.
 * 
 * @param tree B-Tree handle
 * @param callback Function to call for each key-value pair
 * 
 * @note Time complexity: O(n)
 * @note Keys are returned in sorted order
 */
void btree_scan(BTree* tree, void (*callback)(const char* key, uint32_t value));

#endif /* BTREE_H */
