
#pragma once

#include "DelaunayTable.Common.h"

#include <stdbool.h>
#include <stddef.h>

/** # Object & API
 * - element of sequence
 * - key of map
 * - value of map
 */
typedef       void* Object;
typedef const void* ConstObject;

typedef Object (*Object__copy) (
    ConstObject
);

typedef void (*Object__delete) (
    Object
);

typedef size_t (*Object__hash) (
    ConstObject
);

typedef bool (*Object__equal) (
    ConstObject,
    ConstObject
);


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
    ConstObject element,
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
typedef struct {
    Object key;
    Object value;
} Map__Pair;


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
    Object__delete key__delete,
    Object__delete value_delete
);

extern void HashMap__delete(
    HashMap* this,
    Object__delete key_delete,
    Object__delete value_delete
);

extern bool HashMap__get(
    const HashMap* this,
    ConstObject  key,
         Object* value,
    Object__hash  key_hash,
    Object__equal key_equal
);

extern int HashMap__set(
    HashMap* this,
    ConstObject key,
    ConstObject value,
    Object__copy   key_copy,
    Object__delete key_delete,
    Object__hash   key_hash,
    Object__equal  key_equal,
    Object__copy   value_copy,
    Object__delete value_delete
);

extern bool HashMap__remove(
    HashMap* this,
    ConstObject key,
    Object__delete key_delete,
    Object__hash   key_hash,
    Object__equal  key_equality,
    Object__delete value_delete
);
