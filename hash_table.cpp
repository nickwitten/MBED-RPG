/*
 Student Name:
 Date:

=======================
ECE 2035 Project 2-1:
=======================
This file provides definition for the structs and functions declared in the
header file. It also contains helper functions that are not accessible from
outside of the file.

FOR FULL CREDIT, BE SURE TO TRY MULTIPLE TEST CASES and DOCUMENT YOUR CODE.

===================================
Naming conventions in this file:
===================================
1. All struct names use camel case where the first letter is capitalized.
  e.g. "HashTable", or "HashTableEntry"

2. Variable names with a preceding underscore "_" will not be called directly.
  e.g. "_HashTable", "_HashTableEntry"

  Recall that in C, we have to type "struct" together with the name of the struct
  in order to initialize a new variable. To avoid this, in hash_table.h
  we use typedef to provide new "nicknames" for "struct _HashTable" and
  "struct _HashTableEntry". As a result, we can create new struct variables
  by just using:
    - "HashTable myNewTable;"
     or
    - "HashTableEntry myNewHashTableEntry;"

  The preceding underscore "_" simply provides a distinction between the names
  of the actual struct defition and the "nicknames" that we use to initialize
  new structs.
  [See Hidden Definitions section for more information.]

3. Functions, their local variables and arguments are named with camel case, where
  the first letter is lower-case.
  e.g. "createHashTable" is a function. One of its arguments is "numBuckets".
       It also has a local variable called "newTable".

4. The name of a struct member is divided by using underscores "_". This serves
  as a distinction between function local variables and struct members.
  e.g. "num_buckets" is a member of "HashTable".

*/

/****************************************************************************
* Include the Public Interface
*
* By including the public interface at the top of the file, the compiler can
* enforce that the function declarations in the the header are not in
* conflict with the definitions in the file. This is not a guarantee of
* correctness, but it is better than nothing!
***************************************************************************/
#include "hash_table.h"


/****************************************************************************
* Include other private dependencies
*
* These other modules are used in the implementation of the hash table module,
* but are not required by users of the hash table.
***************************************************************************/
#include <stdlib.h>   // For malloc and free
#include <stdio.h>    // For printf


/****************************************************************************
* Hidden Definitions
*
* These definitions are not available outside of this file. However, because
* the are forward declared in hash_table.h, the type names are
* available everywhere and user code can hold pointers to these structs.
***************************************************************************/
/**
 * This structure represents an a hash table.
 * Use "HashTable" instead when you are creating a new variable. [See top comments]
 */
struct _HashTable {
  /** The array of pointers to the head of a singly linked list, whose nodes
      are HashTableEntry objects */
  HashTableEntry** buckets;

  /** The hash function pointer */
  HashFunction hash;

  /** The number of buckets in the hash table */
  unsigned int num_buckets;
};

/**
 * This structure represents a hash table entry.
 * Use "HashTableEntry" instead when you are creating a new variable. [See top comments]
 */
struct _HashTableEntry {
  /** The key for the hash table entry */
  unsigned int key;

  /** The value associated with this hash table entry */
  void* value;

  /**
  * A pointer pointing to the next hash table entry
  * NULL means there is no next entry (i.e. this is the tail)
  */
  HashTableEntry* next;
};


/****************************************************************************
* Private Functions
*
* These functions are not available outside of this file, since they are not
* declared in hash_table.h.
***************************************************************************/
/**
* createHashTableEntry
*
* Helper function that creates a hash table entry by allocating memory for it on
* the heap. It initializes the entry with key and value, initialize pointer to
* the next entry as NULL, and return the pointer to this hash table entry.
*
* @param key The key corresponds to the hash table entry
* @param value The value stored in the hash table entry
* @return The pointer to the hash table entry
*/
static HashTableEntry* createHashTableEntry(unsigned int key, void* value) {

  // Allocate memory for the entry
  HashTableEntry* entryPtr = (HashTableEntry*)malloc(sizeof(HashTableEntry));

  // Initialize values with parameters.  Initialize next as NULL because it is
  // unknown before inserted.
  entryPtr->key = key;
  entryPtr->value = value;
  entryPtr->next = NULL;

  // Return pointer to the entry
  return entryPtr;
}

/**
* findItem
*
* Helper function that checks whether there exists the hash table entry that
* contains a specific key.
*
* @param hashTable The pointer to the hash table.
* @param key The key corresponds to the hash table entry
* @return The pointer to the hash table entry, or NULL if key does not exist
*/
static HashTableEntry* findItem(HashTable* hashTable, unsigned int key) {

  // Hash the key
  HashFunction hash = hashTable->hash;
  unsigned int bucket = hash(key);
  
  // Check if the bucket exists
  if (bucket >= hashTable->num_buckets) {
     return NULL;
  }

  // Loop through the bucket's linked list and check if the current key equals
  // the passed in key.  If it does return the pointer to the hash table entry.
  HashTableEntry* entryPtr = hashTable->buckets[bucket];
  while(entryPtr) {
    if (entryPtr->key == key) {
      return entryPtr;
    }
    entryPtr = entryPtr->next;
  }

  // If no match is found return NULL
  return NULL;

}

/****************************************************************************
* Public Interface Functions
*
* These functions implement the public interface as specified in the header
* file, and make use of the private functions and hidden definitions in the
* above sections.
****************************************************************************/
// The createHashTable is provided for you as a starting point.
HashTable* createHashTable(HashFunction hashFunction, unsigned int numBuckets) {
  // The hash table has to contain at least one bucket. Exit gracefully if
  // this condition is not met.
  if (numBuckets==0) {
    printf("Hash table has to contain at least 1 bucket...\n");
    exit(1);
  }

  // Allocate memory for the new HashTable struct on heap.
  HashTable* newTable = (HashTable*)malloc(sizeof(HashTable));

  // Initialize the components of the new HashTable struct.
  newTable->hash = hashFunction;
  newTable->num_buckets = numBuckets;
  newTable->buckets = (HashTableEntry**)malloc(numBuckets*sizeof(HashTableEntry*));

  // As the new buckets contain indeterminant values, init each bucket as NULL.
  unsigned int i;
  for (i=0; i<numBuckets; ++i) {
    newTable->buckets[i] = NULL;
  }

  // Return the new HashTable struct.
  return newTable;
}




void destroyHashTable(HashTable* hashTable) {

  // Loop through each bucket
  unsigned int numBuckets = hashTable->num_buckets;
  unsigned int i;
  for (i=0; i<numBuckets; ++i) {

    // Loop through the linked list and free it along with its value
    HashTableEntry* entryPtr = hashTable->buckets[i];
    HashTableEntry* nextPtr;
    while(entryPtr) {
      nextPtr = entryPtr->next;
      free(entryPtr->value);
      free(entryPtr);
      entryPtr = nextPtr;
    }
  }

  //Free the buckets array and the Hash Table
  free(hashTable->buckets);
  free(hashTable);

}




void* insertItem(HashTable* hashTable, unsigned int key, void* value) {

  // Check if this key already exists.  If it does replace the value and return
  // the old value.
  HashTableEntry* entryPtr = findItem(hashTable, key);
  if (entryPtr) {
    void* oldValue = entryPtr->value;
    entryPtr->value = value;
    return oldValue;
  }

  // If the key doesn't exist create a new entry
  entryPtr = createHashTableEntry(key, value);

  // Hash the key
  HashFunction hash = hashTable->hash;
  unsigned int bucket = hash(key);

  // Get the head of the linked list and insert the new entry.  Return NULL.
  HashTableEntry* oldHeadPtr = hashTable->buckets[bucket];
  entryPtr->next = oldHeadPtr;
  hashTable->buckets[bucket] = entryPtr;
  return NULL;
}




void* getItem(HashTable* hashTable, unsigned int key) {

  // Find the entry with the passed in key
  HashTableEntry* entryPtr = findItem(hashTable, key);

  // If entryPtr has a value there was a match and return the value in the
  // entry.  Otherwise return NULL.
  if (entryPtr) {
    void* value = entryPtr->value;
    return value;
  } else {
    return NULL;
  }
}




void* removeItem(HashTable* hashTable, unsigned int key) {

  // Initialize variables
  void* value;
  HashTableEntry* entryPtr;
  HashTableEntry* nextEntryPtr;

  // Hash the key
  HashFunction hash = hashTable->hash;
  unsigned int bucket = hash(key);

  entryPtr = hashTable->buckets[bucket];

  // Check if the bucket contains anything.  If not return NULL.
  if (entryPtr) {

    // Get pointer to next entry
    nextEntryPtr = entryPtr->next;

    // Handles the case in which the first entry's key matches the input key.
    // If there is an entry after, set that as the head to the linked list and
    // if there isn't set the head to NULL.
    if (entryPtr->key == key) {
      value = entryPtr->value;
      if (nextEntryPtr) {
        hashTable->buckets[bucket] = nextEntryPtr;
      } else {
        hashTable->buckets[bucket] = NULL;
      }
      free(entryPtr);
      return value;
    }

    // Loop through the linked list checking the key of the next entry.  If
    // the next entry's key matches the passed in key, remove the next entry
    // from the linked list and return its value.
    while(nextEntryPtr) {
      if (nextEntryPtr->key == key) {
        entryPtr->next = nextEntryPtr->next;
        value = nextEntryPtr->value;
        free(nextEntryPtr);
        return value;
      }
      entryPtr = nextEntryPtr;
      nextEntryPtr = nextEntryPtr->next;
    }
  }

  // If key isn't found return NULL
  return NULL;
}




void deleteItem(HashTable* hashTable, unsigned int key) {

  // If the entry is found free it and its value
  void* value = removeItem(hashTable, key);
  if (value) {
    free(value);
  }
}
