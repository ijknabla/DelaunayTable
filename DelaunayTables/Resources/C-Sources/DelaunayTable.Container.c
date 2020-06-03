
#include "DelaunayTable.Container.h"

#include <string.h>

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
    const size_t capacity,
    const size_t sizeofElement
) {
    if (capacity == 0) {return NULL;}

    Vector* const this = (Vector*) MALLOC(sizeof(Vector));
    if (!this) {goto error;}

    this->data = (void*) CALLOC(capacity, sizeofElement);
    if (!(this->data)) {goto error;}

    this->size     = 0;
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
    const Sequence__element element,
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
    Map__Pair* const pairs,
    const Map__key key,
    const bool skip_removed,
    Map__key__hash_function* const key__hash,
    Map__key__equality_function* const key__equality
) {
    size_t hash = key__hash(key);

    for (size_t i = 0 ; i < capacity ; i++) {
        const size_t index = (hash + i) % capacity;
        Map__Pair* const pair = &pairs[index];

        if (skip_removed && Map__Pair__removed(pair)) {continue;}

        if (Map__Pair__empty(pair)) {return pair;}
        if (key__equality(key, pair->key)) {return pair;}
    }
    return NULL;
}

int HashMap__reserve(
    HashMap* const this,
    const size_t capacity,
    Map__key__hash_function* const key__hash,
    Map__key__equality_function* const key__equality
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
            key__hash,
            key__equality
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
    this->capacity = (2 << 6) - 1;
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
    Map__key__delete_function* const key__delete,
    Map__value__delete_function* const value__delete
) {
    for (size_t i = 0 ; i < (this->capacity) ; i++) {
        Map__Pair* const pair = &this->pairs[i];
        if (!Map__Pair__empty(pair)) {
            if (key__delete && pair->key) {
                key__delete(pair->key);
            }
            if (value__delete && pair->value) {
                value__delete(pair->value);
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
    Map__key__delete_function* const key__delete,
    Map__value__delete_function* const value__delete
) {
    HashMap__clear(
        this,
        key__delete,
        value__delete
    );

    FREE(this->pairs);
    FREE(this);
}

bool HashMap__get(
    const HashMap* const this,
    const Map__key key,
    Map__value* const value,
    Map__key__hash_function* const key__hash,
    Map__key__equality_function* const key__equality
) {
    Map__Pair* pair = HashMap__find_pair(
        this->capacity,
        this->pairs,
        key,
        true,  // skip_removed
        key__hash,
        key__equality
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
    const Map__key key,
    const Map__value value,
    Map__key__copy_function* const key__copy,
    Map__key__delete_function* const key__delete,
    Map__key__hash_function* const key__hash,
    Map__key__equality_function* const key__equality,
    Map__value__copy_function* const value__copy,
    Map__value__delete_function* const value__delete
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
            key__hash,
            key__equality
        )) {
            return FAILURE;
        }
    }

    pair = HashMap__find_pair(
        this->capacity,
        this->pairs,
        key,
        true,  // skip_removed
        key__hash,
        key__equality
    );
    if (Map__Pair__empty(pair)) {
        pair = HashMap__find_pair(
            this->capacity,
            this->pairs,
            key,
            false,  // not skip_removed
            key__hash,
            key__equality
        );
    }

    // try
    const bool key_already_exists = !Map__Pair__empty(pair);
    if (!key_already_exists) {
        if (key__copy) {
            pair->key = key__copy(key);
            if (!pair->key) {
                goto error;
            }
        } else {
            pair->key = key;
        }
    }

    Map__value new_value;
    if (value__copy && value) {
        new_value = value__copy(value);
        if (!new_value) {
            goto error;
        }
    } else {
        new_value = value;
    }
    // end try

    this->size++;

    if (value__delete && key_already_exists && pair->value) {
        value__delete(pair->value);
    }
    pair->value = new_value;

    return SUCCESS;

error:

    if (!key_already_exists) {
        if (key__delete && pair->key) {
            key__delete(pair->key);
        }
        pair->key = NULL;
    }

    return FAILURE;
}

bool HashMap__remove(
    HashMap* this,
    const Map__key key,
    Map__key__delete_function* key__delete,
    Map__key__hash_function* key__hash,
    Map__key__equality_function* key__equality,
    Map__value__delete_function* value__delete
) {
    Map__Pair* const pair = HashMap__find_pair(
        this->capacity,
        this->pairs,
        key,
        true,  // skip_removed
        key__hash,
        key__equality
    );

    if (Map__Pair__empty(pair)) {
        return false;
    }

    if (key__delete && pair->key) {
        key__delete(pair->key);
    }
    if (value__delete && pair->value) {
        value__delete(pair->value);
    }

    pair->key = NULL;
    pair->value = (void*) -1;

    this->size--;
    return true;
}
