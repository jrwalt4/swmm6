#include "hash.h"

#include <assert.h>
#include <stdio.h>
#include <string.h>

int simple_test()
{
  Hash* ht = hash_create();
  const char* key = hash_set(ht, "One", (void*)1);
  assert(strcmp(key, "One") == 0);
  assert(hash_length(ht) == 1);
  key = hash_set(ht, "Two", (void*)2);
  assert(strcmp(key, "Two") == 0);
  assert(hash_length(ht) == 2);

  assert(hash_has(ht, "One"));
  int val = (int) hash_get(ht, "One");
  assert(val == 1);
  assert(hash_has(ht, "Two"));
  val = (int) hash_get(ht, "Two");
  assert(val == 2);

  HashIter it = hash_iterator(ht);
  while(hash_next(&it)) {
    printf("Iterator key: '%s' = %i\n", it.key, (int) it.value);
  }

  hash_destroy(ht);

  return 0;
}

int expand_test()
{
  char* keys[] = {
    "One",
    "Two",
    "Three",
    "Four",
    "Five",
    "Six",
    "Seven",
    "Eight",
    "Nine"
  };
  const int keys_length = sizeof(keys)/sizeof(keys[0]);
  Hash* ht = hash_with_capacity(16);
  const char* key;
  for(int i = 0 ; i < keys_length ; i++) {
    hash_set(ht, keys[i], keys[i]);
  }
  assert(hash_length(ht) == keys_length);
  
  HashIter it = hash_iterator(ht);
  while(hash_next(&it)) {
    printf("Iterator key: '%s' = '%s'\n", it.key, (char*) it.value);
  }

  hash_destroy(ht);

  return 0;
}

int main(int argc, char* argv[])
{
  int rc = simple_test();
  rc = expand_test();
  
  return rc;
}
