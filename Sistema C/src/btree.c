#include "btree.h"
#include <stdlib.h>
#include <string.h>

typedef struct BTreeNode {
    bool is_leaf;
    uint32_t num_keys;
    char keys[BTREE_ORDER - 1][MAX_KEY_SIZE];
    uint32_t values[BTREE_ORDER - 1];
    uint32_t children[BTREE_ORDER];
    uint32_t page_num;
} BTreeNode;

struct BTree {
    Pager* pager;
    uint32_t root_page;
};

BTree* btree_create(Pager* pager) {
    BTree* tree = malloc(sizeof(BTree));
    if (!tree) return NULL;
    
    tree->pager = pager;
    tree->root_page = pager_allocate_page(pager);
    
    Page* root_page = pager_get_page(pager, tree->root_page);
    BTreeNode* root = (BTreeNode*)root_page->data;
    root->is_leaf = true;
    root->num_keys = 0;
    root->page_num = tree->root_page;
    
    return tree;
}

void btree_destroy(BTree* tree) {
    if (tree) {
        free(tree);
    }
}

static int key_compare(const char* k1, const char* k2) {
    return strcmp(k1, k2);
}

static BTreeNode* get_node(BTree* tree, uint32_t page_num) {
    Page* page = pager_get_page(tree->pager, page_num);
    return (BTreeNode*)page->data;
}

static void split_child(BTree* tree, BTreeNode* parent, int index) {
    BTreeNode* full_child = get_node(tree, parent->children[index]);
    uint32_t new_page = pager_allocate_page(tree->pager);
    BTreeNode* new_child = get_node(tree, new_page);
    
    new_child->is_leaf = full_child->is_leaf;
    new_child->num_keys = BTREE_ORDER / 2 - 1;
    new_child->page_num = new_page;
    
    for (int i = 0; i < BTREE_ORDER / 2 - 1; i++) {
        strcpy(new_child->keys[i], full_child->keys[i + BTREE_ORDER / 2]);
        new_child->values[i] = full_child->values[i + BTREE_ORDER / 2];
    }
    
    if (!full_child->is_leaf) {
        for (int i = 0; i < BTREE_ORDER / 2; i++) {
            new_child->children[i] = full_child->children[i + BTREE_ORDER / 2];
        }
    }
    
    full_child->num_keys = BTREE_ORDER / 2 - 1;
    
    for (int i = parent->num_keys; i > index; i--) {
        parent->children[i + 1] = parent->children[i];
    }
    parent->children[index + 1] = new_page;
    
    for (int i = parent->num_keys - 1; i >= index; i--) {
        strcpy(parent->keys[i + 1], parent->keys[i]);
        parent->values[i + 1] = parent->values[i];
    }
    
    strcpy(parent->keys[index], full_child->keys[BTREE_ORDER / 2 - 1]);
    parent->values[index] = full_child->values[BTREE_ORDER / 2 - 1];
    parent->num_keys++;
}

static void insert_non_full(BTree* tree, BTreeNode* node, const char* key, uint32_t value) {
    int i = node->num_keys - 1;
    
    if (node->is_leaf) {
        while (i >= 0 && key_compare(key, node->keys[i]) < 0) {
            strcpy(node->keys[i + 1], node->keys[i]);
            node->values[i + 1] = node->values[i];
            i--;
        }
        strcpy(node->keys[i + 1], key);
        node->values[i + 1] = value;
        node->num_keys++;
    } else {
        while (i >= 0 && key_compare(key, node->keys[i]) < 0) {
            i--;
        }
        i++;
        
        BTreeNode* child = get_node(tree, node->children[i]);
        if (child->num_keys == BTREE_ORDER - 1) {
            split_child(tree, node, i);
            if (key_compare(key, node->keys[i]) > 0) {
                i++;
            }
            child = get_node(tree, node->children[i]);
        }
        insert_non_full(tree, child, key, value);
    }
}

bool btree_insert(BTree* tree, const char* key, uint32_t value) {
    BTreeNode* root = get_node(tree, tree->root_page);
    
    if (root->num_keys == BTREE_ORDER - 1) {
        uint32_t new_root_page = pager_allocate_page(tree->pager);
        BTreeNode* new_root = get_node(tree, new_root_page);
        new_root->is_leaf = false;
        new_root->num_keys = 0;
        new_root->children[0] = tree->root_page;
        new_root->page_num = new_root_page;
        
        split_child(tree, new_root, 0);
        tree->root_page = new_root_page;
        insert_non_full(tree, new_root, key, value);
    } else {
        insert_non_full(tree, root, key, value);
    }
    
    return true;
}

bool btree_search(BTree* tree, const char* key, uint32_t* value_out) {
    BTreeNode* node = get_node(tree, tree->root_page);
    
    while (node) {
        int i = 0;
        while (i < node->num_keys && key_compare(key, node->keys[i]) > 0) {
            i++;
        }
        
        if (i < node->num_keys && key_compare(key, node->keys[i]) == 0) {
            *value_out = node->values[i];
            return true;
        }
        
        if (node->is_leaf) {
            return false;
        }
        
        node = get_node(tree, node->children[i]);
    }
    
    return false;
}

static void scan_node(BTree* tree, BTreeNode* node, void (*callback)(const char* key, uint32_t value)) {
    if (!node) return;
    
    for (int i = 0; i < node->num_keys; i++) {
        if (!node->is_leaf) {
            BTreeNode* child = get_node(tree, node->children[i]);
            scan_node(tree, child, callback);
        }
        callback(node->keys[i], node->values[i]);
    }
    
    if (!node->is_leaf) {
        BTreeNode* child = get_node(tree, node->children[node->num_keys]);
        scan_node(tree, child, callback);
    }
}

void btree_scan(BTree* tree, void (*callback)(const char* key, uint32_t value)) {
    BTreeNode* root = get_node(tree, tree->root_page);
    scan_node(tree, root, callback);
}

bool btree_delete(BTree* tree, const char* key) {
    return false;
}
