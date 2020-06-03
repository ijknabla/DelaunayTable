
#pragma once

#include "DelaunayTable.Common.h"

#include <stdbool.h>
#include <stddef.h>


/** # Sequcence
 * basic data types
 */
typedef void* Sequence__element;


/** # Vector
 * variable size array
 */
typedef struct {
    size_t size;
    size_t capacity;
    void* data;  /// Address where elements are saved
} Vector;

/// ## Vector methods
extern Vector* Vector__new(
    const size_t size,
    const size_t sizeofElement
);

extern Vector* Vector__copy(
    const Vector* this,
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

/**
 * `elements` is array that actually referenced.
 *  If type of each element is `element_type`,
 *  type of `elements` is `element_type*`.
 */
#define Vector__elements(this, element_type) ( (element_type*) (this)->data )


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
