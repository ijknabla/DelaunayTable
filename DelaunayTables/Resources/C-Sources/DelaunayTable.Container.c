
#include "DelaunayTable.Container.h"

#include <string.h>
#include <stdlib.h>


// # capacity assumption
static const size_t default_capacity = 1;

static inline size_t assume_capacity(
    const size_t needed_size,
    const size_t current_capacity
) {
    size_t capacity = current_capacity;
    while (capacity < needed_size) {
        capacity *= 2;
    }
    return capacity;
}


/// # Vector
/// ## Vector static functions
static int Vector__reserve(
    Vector* this,
    const size_t capacity,
    const size_t sizeofElement
) {
    const size_t old_capacity = this->capacity;

    if (capacity < (this->size)) {return FAILURE;}

    void* const new_data = (void*) REALLOC(
        this->data, capacity*sizeofElement
    );
    if (!new_data) {return FAILURE;}

    if (old_capacity < capacity) {
        memset(
            new_data + old_capacity * sizeofElement,
            0,
            (capacity - old_capacity) * sizeofElement
        );
    }

    this->capacity = capacity;
    this->data = new_data;

    return SUCCESS;
}

/// ## Vector methods
Vector* Vector__new(
    const size_t size,
    const size_t sizeofElement
) {
    const size_t capacity = (size == 0) ? default_capacity : size;

    Vector* const this = (Vector*) MALLOC(sizeof(Vector));
    if (!this) {goto error;}

    this->data = (void*) CALLOC(capacity, sizeofElement);
    if (!(this->data)) {goto error;}

    this->size     = size;
    this->capacity = capacity;

    return this;

error:

    if (this) {
        if (this->data) {
            FREE(this->data);
        }
        FREE(this);
    }

    return NULL;
}

Vector* Vector__copy(
    const Vector* const this,
    const size_t sizeofElement
) {
    Vector* copied = Vector__new(
        this->size,
        sizeofElement
    );
    if (!copied) {return NULL;}

    copied->size = this->size;
    memcpy(
        copied->data,
        this->data,
        (this->size) * sizeofElement
    );

    return copied;
}

void Vector__delete(
    Vector* const this
) {
    FREE(this->data);
    FREE(this);
}

extern int Vector__append(
    Vector* const this,
    ConstObject element,
    const size_t sizeofElement
) {
    int status = SUCCESS;

    const size_t prev_capacity = this->capacity;
    const size_t prev_size     = this->size;
    const size_t next_size     = prev_size+1;

    if (next_size > prev_capacity) {
        status = Vector__reserve(
            this,
            assume_capacity(next_size, prev_capacity),
            sizeofElement
        );
        if (status) {return status;}
    }

    this->size = next_size;
    memcpy(
        this->data + (prev_size) * sizeofElement,
        element,
        sizeofElement
    );

    return status;
}

void Vector__sort(
    Vector* this,
    Object__compare element_compare,
    const size_t sizeofElement
) {
    qsort(this->data, this->size, sizeofElement, element_compare);
}


/// ## HashMap methods
/// ### static functions for HashMap
static inline bool Map__Pair__empty(
    const Map__Pair* const pair
) {
    return pair->key == NULL;
}

static inline bool Map__Pair__removed(
    const Map__Pair* const pair
) {
    return Map__Pair__empty(pair) && pair->value;
}

static Map__Pair* HashMap__find_pair(
    const size_t capacity,
    Map__Pair*  const pairs,
    ConstObject const key,
    const bool skip_removed,
    Object__hash  const key_hash,
    Object__equal const key_equal
) {
    size_t hash = key_hash(key);

    for (size_t i = 0 ; i < capacity ; i++) {
        const size_t index = (hash + i) % capacity;
        Map__Pair* const pair = &pairs[index];

        if (skip_removed && Map__Pair__removed(pair)) {continue;}

        if (Map__Pair__empty(pair)) {return pair;}
        if (key_equal(key, pair->key)) {return pair;}
    }
    return NULL;
}

int HashMap__reserve(
    HashMap* const this,
    const size_t capacity,
    Object__hash  const key_hash,
    Object__equal const key_equality
) {
    if ( capacity < (this->size) ) {
        return FAILURE;
    }

    Map__Pair* new_pairs = (Map__Pair*) CALLOC(
        capacity, sizeof(Map__Pair)
    );
    if (!new_pairs) {
        return FAILURE;
    }

    for (size_t i = 0 ; i < (this->capacity) ; i++) {
        const Map__Pair* const old_pair = &this->pairs[i];

        if (Map__Pair__empty(old_pair)) {
            continue;
        }

        Map__Pair* const new_pair = HashMap__find_pair(
            capacity,
            new_pairs,
            old_pair->key,
            true,  // skip_removed
            key_hash,
            key_equality
        );

        new_pair->key   = old_pair->key;
        new_pair->value = old_pair->value;
    }

    FREE(this->pairs);

    this->capacity = capacity;
    this->pairs = new_pairs;

    return SUCCESS;
}

/// ### extern functions for HashMap
HashMap* HashMap__new(
) {
    HashMap* this = (HashMap*) malloc(sizeof(HashMap));
    if (!this) {goto error;}

    this->size = 0;
    this->capacity = (2 << 2) - 1;
    this->pairs = (Map__Pair*) CALLOC(
        this->capacity, sizeof(Map__Pair)
    );
    if (!this->pairs) {goto error;}

    return this;

error:

    if (this) {
        if (this->pairs) {
            FREE(this->pairs);
        }
        FREE(this);
    }

    return NULL;
}

void HashMap__clear(
    HashMap* const this,
    Object__delete const key_delete,
    Object__delete const value_delete
) {
    for (size_t i = 0 ; i < (this->capacity) ; i++) {
        Map__Pair* const pair = &this->pairs[i];
        if (!Map__Pair__empty(pair)) {
            if (key_delete && pair->key) {
                key_delete(pair->key);
            }
            if (value_delete && pair->value) {
                value_delete(pair->value);
            }
        }

        pair->key = NULL;
        pair->value = NULL;
    }

    this->size = 0;

    return;
}

void HashMap__delete(
    HashMap* const this,
    Object__delete const key_delete,
    Object__delete const value_delete
) {
    HashMap__clear(
        this,
        key_delete,
        value_delete
    );

    FREE(this->pairs);
    FREE(this);
}

bool HashMap__get(
    const HashMap* const this,
    ConstObject  const key,
         Object* const value,
    Object__hash  const key_hash,
    Object__equal const key_equal
) {
    Map__Pair* pair = HashMap__find_pair(
        this->capacity,
        this->pairs,
        key,
        true,  // skip_removed
        key_hash,
        key_equal
    );

    if (!Map__Pair__empty(pair)) {
        *value = pair->value;
        return true;
    } else {
        *value = NULL;
        return false;
    }
}

int HashMap__set(
    HashMap* const this,
    ConstObject const key,
    ConstObject const value,
    Object__copy   const key_copy,
    Object__delete const key_delete,
    Object__hash   const key_hash,
    Object__equal  const key_equal,
    Object__copy   const value_copy,
    Object__delete const value_delete
) {
    Map__Pair* pair;

    size_t new_size = this->size+1;

    if ( (new_size*2) > (this->capacity) ) {
        size_t new_capacity = (
            (this->capacity + 1) * 2 - 1
        );
        if (HashMap__reserve(
            this,
            new_capacity,
            key_hash,
            key_equal
        )) {
            return FAILURE;
        }
    }

    pair = HashMap__find_pair(
        this->capacity,
        this->pairs,
        key,
        true,  // skip_removed
        key_hash,
        key_equal
    );
    if (Map__Pair__empty(pair)) {
        pair = HashMap__find_pair(
            this->capacity,
            this->pairs,
            key,
            false,  // not skip_removed
            key_hash,
            key_equal
        );
    }

    // try
    const bool key_already_exists = !Map__Pair__empty(pair);
    if (!key_already_exists) {
        if (key_copy) {
            pair->key = key_copy(key);
            if (!pair->key) {
                goto error;
            }
        } else {
            pair->key = (Object) key;
        }
    }

    Object new_value;
    if (value_copy && value) {
        new_value = value_copy(value);
        if (!new_value) {
            goto error;
        }
    } else {
        new_value = (Object) value;
    }
    // end try

    this->size++;

    if (value_delete && key_already_exists && pair->value) {
        value_delete(pair->value);
    }
    pair->value = new_value;

    return SUCCESS;

error:

    if (!key_already_exists) {
        if (key_delete && pair->key) {
            key_delete(pair->key);
        }
        pair->key = NULL;
    }

    return FAILURE;
}

bool HashMap__remove(
    HashMap* this,
    ConstObject const key,
    Object__delete const key_delete,
    Object__hash   const key_hash,
    Object__equal  const key_equal,
    Object__delete const value_delete
) {
    Map__Pair* const pair = HashMap__find_pair(
        this->capacity,
        this->pairs,
        key,
        true,  // skip_removed
        key_hash,
        key_equal
    );

    if (Map__Pair__empty(pair)) {
        return false;
    }

    if (key_delete && pair->key) {
        key_delete(pair->key);
    }
    if (value_delete && pair->value) {
        value_delete(pair->value);
    }

    pair->key = NULL;
    pair->value = (void*) -1;

    this->size--;
    return true;
}
