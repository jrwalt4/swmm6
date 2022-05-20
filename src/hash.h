// Simple hash table implemented in C.
// Stolen from https://benhoyt.com/writings/hash-table-in-c/
// https://github.com/benhoyt/ht

#ifndef SWMM_HASH_H
#define SWMM_HASH_H

#include <stdbool.h>
#include <stddef.h>

// Hash table structure: create with hash_create, free with hash_destroy.
typedef struct Hash Hash;

typedef struct HashEntry HashEntry;

// Create hash table and return pointer to it, or NULL if out of memory.
Hash* hash_create(void);

// Create hash with initial capacity
Hash* hash_with_capacity(size_t capacity);

// Free memory allocated for hash table, including allocated keys.
void hash_destroy(Hash* table);

// Get item with given key (NUL-terminated) from hash table. Return
// value (which was set with hash_set), or NULL if key not found.
void* hash_get(Hash* table, const char* key);

// test if hash table has key
bool hash_has(Hash* table, const char* key);

// Set item with given key (NUL-terminated) to value (which must not
// be NULL). If not already present in table, key is copied to newly
// allocated memory (keys are freed automatically when hash_destroy is
// called). Return address of copied key, or NULL if out of memory.
const char* hash_set(Hash* table, const char* key, void* value);

// Return number of items in hash table.
size_t hash_length(Hash* table);

// Hash table iterator: create with hash_iterator, iterate with hash_next.
typedef struct HashIter
{
    const char* key;  // current key
    void* value;      // current value

    // Don't use these fields directly.
    Hash* _table;       // reference to hash table being iterated
    size_t _index;      // current index into ht._entries
    HashEntry* _entry;
} HashIter;

// Return new hash table iterator (for use with hash_next).
HashIter hash_iterator(Hash* table);

// Move iterator to next item in hash table, update iterator's key
// and value to current item, and return true. If there are no more
// items, return false. Don't call hash_set during iteration.
bool hash_next(HashIter* it);

#endif // SWMM_HASH_H
