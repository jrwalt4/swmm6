// Simple hash table implemented in C.
// Stolen from https://benhoyt.com/writings/hash-table-in-c/
// https://github.com/benhoyt/ht

#include "hash.h"

#include <assert.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define SWMM_DEBUG 1
#if SWMM_DEBUG
#define debug(...) printf(__VA_ARGS__)
#else
#define debug(...) 
#endif

// Hash table entry (slot may be filled or empty).
struct HashEntry {
    const char* key;  // key is NULL if this slot is empty
    void* value;
    HashEntry* next;
};

// Hash table structure: create with hash_create, free with hash_destroy.
struct Hash {
    HashEntry** entries; // hash slots
    size_t capacity;    // size of _entries array
    size_t length;      // number of items in hash table
};

#define INITIAL_CAPACITY 16  // must not be zero

Hash* hash_with_capacity(size_t capacity) {
    // Allocate space for hash table struct.
    Hash* table = malloc(sizeof(*table));
    if (table == NULL) {
        return NULL;
    }
    table->length = 0;
    table->capacity = capacity < INITIAL_CAPACITY ? INITIAL_CAPACITY : capacity;

    // Allocate (zero'd) space for entry buckets.
    table->entries = calloc(table->capacity, sizeof(HashEntry*));
    if (table->entries == NULL) {
        free(table); // error, free table before we return!
        return NULL;
    }
    return table;
}

Hash* hash_create(void) {
    return hash_with_capacity(INITIAL_CAPACITY);
}

void hash_entry_destroy(HashEntry* entry) {
  HashEntry* next;
  while(NULL != entry) {
    next = entry->next;
    free((char*)entry->key);
    free(entry);
    entry = next;
  }
}

void hash_destroy(Hash* table) {
    // First free allocated keys.
    for (size_t i = 0; i < table->capacity; i++) {
        hash_entry_destroy(table->entries[i]);
        //free((void*)table->entries[i].key);
    }
    
    // Then free entries array and table itself.
    free(table->entries);
    free(table);
}

#define FNV_OFFSET 14695981039346656037UL
#define FNV_PRIME  1099511628211UL

// Return 64-bit FNV-1a hash for key (NUL-terminated). See description:
// https://en.wikipedia.org/wiki/Fowler–Noll–Vo_hash_function
static uint64_t hash_key(const char* key) {
    uint64_t hash = FNV_OFFSET;
    for (const char* p = key; *p; p++) {
        hash ^= (uint64_t)(unsigned char)(*p);
        hash *= FNV_PRIME;
    }
    return hash;
}

static size_t hash_index(const char* key, size_t capacity) {
  // AND hash with capacity-1 to ensure it's within entries array.
  uint64_t hash = hash_key(key);
  return (size_t)(hash & (uint64_t)(capacity - 1));
}

static HashEntry** hash_get_location(
    HashEntry* entries[],
    size_t capacity,
    const char* key) {
  size_t index = hash_index(key, capacity);
  HashEntry* entry;;
  HashEntry** loc = entries + index;

  while (*loc != NULL) {
    entry = *loc;
    if (strcmp(key, entry->key) == 0) {
        // Found key, loc pointing to entry
        break;
    }
    // Key wasn't in this slot, move to next.
    loc = &entry->next;
  }
  return loc;
}

void* hash_get(Hash* table, const char* key) {
  HashEntry** loc = hash_get_location(table->entries, table->capacity, key);
  if(*loc == NULL) {
    return NULL;
  }
  return (*loc)->value;
}

bool hash_has(Hash* table, const char* key) {
  return hash_get(table, key) != NULL;
  /*
  size_t index = hash_index(key, table->capacity);
  // Loop till we find an empty entry.
  HashEntry* entry = table->entries[index];
  while (entry != NULL) {
      if (strcmp(key, entry->key) == 0) {
          // Found key, return true.
          return true;
      }
      // Key wasn't in this slot, move to next.
      entry = entry->next;
  }
  return false;
  */
}

// Internal function to set an entry (without expanding table).
static const char* hash_set_entry(HashEntry** entries, size_t capacity,
        const char* key, void* value, size_t* plength) {
    size_t index = hash_index(key, capacity);

    // Loop till we find an empty entry.
    HashEntry* entry;
    // set insertion pointer to address of index in entries array
    HashEntry** insert_ptr = entries + index;

    while (*insert_ptr != NULL) {
      entry = *insert_ptr;
        if (strcmp(key, entry->key) == 0) {
            // Found key (it already exists), update value.
            entry->value = value;
            return entry->key;
        }
        // Key wasn't in this slot, move to next.
        insert_ptr = &entry->next;
        //entry = entry->next;
    }

    // Didn't find key, allocate+copy if needed, then insert it.
    // only copy if this is an expand operation, key already allocated
    if (plength != NULL) {
      
      key = strdup(key);
      if (key == NULL) {
          return NULL;
      }
      (*plength)++;
    }
    
    entry = malloc(sizeof(HashEntry));
    entry->key = key;
    entry->value = value;
    *insert_ptr = entry;
    
    return key;
}

// Expand hash table to twice its current size. Return true on success,
// false if out of memory.
static bool hash_expand(Hash* table) {
    // Allocate new entries array.
    size_t new_capacity = table->capacity * 2;
    if (new_capacity < table->capacity) {
        return false;  // overflow (capacity would be too big)
    }
    HashEntry** new_entries = calloc(new_capacity, sizeof(HashEntry*));
    if (new_entries == NULL) {
        return false;
    }
    debug("Expanding from %li to %li\n", table->capacity, new_capacity);
    // Iterate entries, move all non-empty ones to new table's entries.
    HashIter it = hash_iterator(table);
    HashEntry** new_location;
    // have to keep track of previous entry during iteration
    // because need to clear `entry->next` during move
    HashEntry* current;
    HashEntry* next;
    bool has_next = hash_next(&it);
    if(has_next) {
      current = it._entry;
    } else {
      return false;
    }
    do {
      new_location = hash_get_location(new_entries, new_capacity, current->key);
      assert(*new_location == NULL);
      has_next = hash_next(&it);
      next = it._entry;
      current->next = NULL;
      // move the entry to the new location
      *new_location = current;
      current = next;
    } while(has_next);
    /*
    for (size_t i = 0; i < table->capacity; i++) {
        HashEntry* entry = table->entries[i];
        if (entry != NULL) {
            hash_set_entry(new_entries, new_capacity, entry->key,
                         entry.value, NULL);
        }
    }
    */

    // Free old entries array and update this table's details.
    free(table->entries);
    table->entries = new_entries;
    table->capacity = new_capacity;
    return true;
}

const char* hash_set(Hash* table, const char* key, void* value) {
    assert(value != NULL);
    if (value == NULL) {
        return NULL;
    }

    // If length will exceed half of current capacity, expand it.
    if (table->length >= table->capacity / 2) {
        if (!hash_expand(table)) {
            return NULL;
        }
    }

    // Set entry and update length.
    return hash_set_entry(table->entries, table->capacity, key, value,
                        &table->length);
}

size_t hash_length(Hash* table) {
    return table->length;
}

HashIter hash_iterator(Hash* table) {
    HashIter it;
    it._table = table;
    it._index = 0;
    it._entry = NULL;
    return it;
}

bool hash_next(HashIter* it) {
  // Loop till we've hit end of entries array.
  Hash* table = it->_table;
  HashEntry* entry = it->_entry;
  if(entry != NULL) {
    // previous iteration still had an entry at the
    // end of the linked list.
    entry = entry->next;
  }
  if(entry == NULL) {
    // either at the beginning of iteration or the previous
    // 'if()' set entry to a NULL (we're at the end of the bucket),
    // so loop through array until we find a non-empty bucket.
    while (++(it->_index) < table->capacity) {
      entry = table->entries[it->_index];
      if (entry != NULL) {
        break;
      }
    }
  }
  if(entry != NULL) {
    // we found another entry, so set iterator
    it->_entry = entry;
    it->key = entry->key;
    it->value = entry->value;
    // leave _index the same
    return true;
  }
  return false;
}
