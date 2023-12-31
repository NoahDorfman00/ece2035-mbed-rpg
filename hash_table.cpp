/*
 Student Name: Noah Dorfman
 Date: 3/29/19

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
  //Allocate memory for entry
  HashTableEntry* this_node = (HashTableEntry*)malloc(sizeof(HashTableEntry));
  
  //Return NULL if malloc fails
  if(!this_node) return NULL;

  //Initialize components of entry struct
  this_node->value = value;
  this_node->key = key;
  this_node->next = NULL;

  //Return pointer to entry
  return this_node;
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
  //Returns NULL if hashTable DNE
  if(!hashTable) return NULL;
  //Start searching with head of LL
  HashTableEntry* this_node = hashTable->buckets[(*(hashTable->hash))(key)];

  //Search through the LL
  while(this_node){
    if(this_node->key == key){
      //Return pointer to found entry
      return this_node;
    }
    //Walking the LL
    this_node = this_node->next;
  }

  //Return NULL if item DNE
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
    printf("\nERROR:Hash Table must contain at least 1 bucket...\n");
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
  //If hashTable exists
  if(hashTable){
    //Iterate through buckets
    for(int i=0; i<hashTable->num_buckets; i++){
      //Start iterating each LL at the head
      HashTableEntry* this_node = hashTable->buckets[i];

      //Iterate the LL
      while(this_node){
        //Free value
        if(this_node->value) free(this_node->value);
        //Preserve next item
        HashTableEntry* temp = this_node->next;
        //Free item
        free(this_node);
        //Walk the LL
        this_node = temp;
      }
    }
  }

  //Free buckets if it exists
  if(hashTable->buckets) free(hashTable->buckets);

  free(hashTable);
}

void* insertItem(HashTable* hashTable, unsigned int key, void* value) {
  //Returns NULL if hashTable DNE
  if(!hashTable) return NULL;

  HashTableEntry* this_node;
  //Check for existing item with same key
  if((this_node = findItem(hashTable, key))){
    //Preserve value about to be overwritten
    void* overwritten = this_node->value;
    //Overwrite value
    this_node->value = value;

    //Return overwritten value
    return overwritten;
  }

  //If item DNE, create item
  this_node = createHashTableEntry(key, value);

  if(this_node){
    //Insert at head by assigning previous head to next
    this_node->next = hashTable->buckets[(*(hashTable->hash))(key)];
    //Reassign head to be address of new item
    hashTable->buckets[(*(hashTable->hash))(key)] = this_node;
  }
  
  //Return NULL because no value was overwritten
  return NULL;
}

void* getItem(HashTable* hashTable, unsigned int key) {
  HashTableEntry* this_node;
  //If entry exists, return its value
  if((this_node = findItem(hashTable, key))) return this_node->value;

  //Else, return NULL
  return NULL;
}

void* removeItem(HashTable* hashTable, unsigned int key) {
  //Returns NULL if hashTable DNE
  if(!hashTable) return NULL;
  
  //Start searching with head of LL
  HashTableEntry* this_node = hashTable->buckets[(*(hashTable->hash))(key)];

  //If LL is empty return NULL
  if(!this_node) return NULL;

  //If first item matches
  if(this_node->key == key){
    //Preserve value about to be removed
    void* removed = this_node->value;
    //Reassign head to be the address of the following item in the LL
    hashTable->buckets[(*(hashTable->hash))(key)] = this_node->next;
    //Free removed item if not NULL
    if(this_node) free(this_node);
    //Return removed value
    return removed;
  }

  //Search through the rest of the LL
  while(this_node->next){
    //If next item matches
    if(this_node->next->key == key){
      //Preserve value about to be removed
      void* removed = this_node->next->value;
      //Preserve matching item
      HashTableEntry* temp = this_node->next;
      //Cut matching item out of LL
      this_node->next = this_node->next->next;
      //Free removed item if not NULL
      if(temp) free(temp);
      //Return removed value
      return removed;
    }
    //Walk the LL
    this_node = this_node->next;
  }
  //Return NULL if item DNE
  return NULL;
}

void deleteItem(HashTable* hashTable, unsigned int key) {
  void* this_value;
  //removes item
  if((this_value = removeItem(hashTable, key))){
    //Frees value pointer if not NULL
    free(this_value);
  }
  printf("\nERROR:Hash Table item DNE\n");
}
