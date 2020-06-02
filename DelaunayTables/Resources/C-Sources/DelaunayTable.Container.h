
#pragma once

#include "DelaunayTable.h"

#include <stdbool.h>
#include <stddef.h>


/** # Sequcence
 * basic data types and APIs
 */
typedef void* Sequence__element;
typedef void Sequence__element__delete_function (Sequence__element);


/** # Vector
 * variable size array
 */
typedef struct {
    size_t size;
    size_t capacity;
    void* data;
} Vector;

/// ## Vector methods
extern Vector* Vector__new(
    const size_t capacity,
    const size_t sizeofElement
);

extern void Vector__delete(
    Vector* this
);

extern int Vector__append(
    Vector* this,
    const Sequence__element element,
    const size_t sizeofElement
);

#define Vector__data(this, type) ( (type*) (this)->data )


/** # List
 * bidirectional linked list
 */
typedef void* List__element;
typedef void List__element__delete_function (List__element);

typedef struct ListNode_tag {
    List__element element;
    struct ListNode_tag* prev;
    struct ListNode_tag* next;
} ListNode;

typedef struct{
    ListNode* first;
    ListNode* last;
} List;

/// ## List methods
#define List__INITIALIZER {NULL, NULL}

extern void List__initialize(
    List* this
);

extern void List__clear(
    List* this,
    List__element__delete_function* element__delete
);

extern int List__append(
    List* this,
    const List__element element
);

extern int List__insert(
    List* this,
    const List__element element
);

extern void List__remove(
    List* this,
    ListNode* node,
    List__element__delete_function* element__delete
);

extern bool List__pop(
    List* this,
    List__element__delete_function* element__delete
);

/// ## List properties
static inline bool List__empty(
    const List* const this
) {
    return !(this->first) && !(this->last);
}

static inline size_t List__size(
    const List* const this
) {
    size_t size = 0;
    for (
        const ListNode* node = this->first;
        node;
        node = node->next
    ) {
        size++;
    }
    return size;
}


/**
 * # Map
 * basic data types and APIs
 */
typedef void* Map__key;
typedef void* Map__value;

typedef struct {
    Map__key   key;
    Map__value value;
} Map__Pair;

typedef Map__key Map__key__copy_function (
    const Map__key
);
typedef void Map__key__delete_function (
    Map__key
);
typedef size_t Map__key__hash_function (
    const Map__key
);
typedef bool Map__key__equality_function (
    const Map__key, const Map__key
);

typedef Map__value Map__value__copy_function (
    const Map__value
);
typedef void Map__value__delete_function (
    Map__value
);


/** # HashMap
 * open-hash map {key => value}
 */
typedef struct {
    size_t size;
    size_t capacity;
    Map__Pair* pairs;
} HashMap;

/// ## HashMap methods
extern HashMap* HashMap__new(
);

extern void HashMap__clear(
    HashMap* this,
    Map__key__delete_function* key__delete,
    Map__value__delete_function* value__delete
);

extern void HashMap__delete(
    HashMap* this,
    Map__key__delete_function* key__delete,
    Map__value__delete_function* value__delete
);

extern bool HashMap__get(
    const HashMap* this,
    const Map__key key,
    Map__value* value,
    Map__key__hash_function* key__hash,
    Map__key__equality_function* key__equality
);

extern int HashMap__set(
    HashMap* this,
    const Map__key key,
    const Map__value value,
    Map__key__copy_function* key__copy,
    Map__key__delete_function* key__delete,
    Map__key__hash_function* key__hash,
    Map__key__equality_function* key__equality,
    Map__value__copy_function* value__copy,
    Map__value__delete_function* value__delete
);

extern bool HashMap__remove(
    HashMap* this,
    const Map__key key,
    Map__key__delete_function* key__delete,
    Map__key__hash_function* key__hash,
    Map__key__equality_function* key__equality,
    Map__value__delete_function* value__delete
);
