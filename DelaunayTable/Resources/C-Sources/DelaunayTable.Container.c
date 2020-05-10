
#include "DelaunayTable.Container.h"


void List__initialize(
    List* const this
) {
    this->first = NULL;
    this->last = NULL;
}

void List__clear(
    List* const this,
    List__element__delete_function* const element__delete
) {
    ListNode* node = this->first;
    while (node != NULL) {
        ListNode* next = node->next;

        if (element__delete && node->element) {
            element__delete(node->element);
        }
        FREE(node);

        node = next;
    }

    List__initialize(this);
}

int List__append(
    List* const this,
    const List__element element
) {
    ListNode* new_node = (ListNode*) MALLOC(
        sizeof(ListNode)
    );
    if (new_node == NULL) return FAILURE;

    new_node->element = element;
    new_node->prev = this->last;
    new_node->next = NULL;

    if (List__empty(this)) {
        this->first       = new_node;
        this->last        = new_node;
    } else {
        this->last->next  = new_node;
        this->last        = new_node;
    }

    return SUCCESS;
}

int List__insert(
    List* const this,
    const List__element element
) {
    ListNode* new_node = (ListNode*) MALLOC(
        sizeof(ListNode)
    );
    if (new_node == NULL) return FAILURE;

    new_node->element = element;
    new_node->prev = NULL;
    new_node->next = this->first;

    if (List__empty(this)) {
        this->first       = new_node;
        this->last        = new_node;
    } else {
        this->first->prev = new_node;
        this->first       = new_node;
    }

    return SUCCESS;
}

void List__remove(
    List* const this,
    ListNode* const node,
    List__element__delete_function* const element__delete
) {
    if (this->first == this->last) {
        this->first = NULL;
        this->last  = NULL;
    } else if (node == this->first) {
        this->first = node->next;
        this->first->prev = NULL;
    } else if (node == this->last) {
        this->last = node->prev;
        this->last->next = NULL;
    } else {
        node->next->prev = node->prev;
        node->prev->next = node->next;
    }

    if (element__delete && node->element) {
        element__delete(node->element);
    }
    FREE(node);
}

bool List__pop(
    List* this,
    List__element__delete_function* const element__delete
) {
    if (List__empty(this)) {
        return false;
    }
    List__remove(this, this->first, element__delete);
    return true;
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
    bool key_exists;
    if (Map__Pair__empty(pair)) {
        if (key__copy) {
            pair->key = key__copy(key);
            if (!pair->key) {
                goto error;
            }
        } else {
            pair->key = key;
        }
        key_exists = false;
    } else {
        key_exists = true;
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

    if (value__delete && key_exists && pair->value) {
        value__delete(pair->value);
    }
    pair->value = new_value;

    return SUCCESS;

error:

    if (!key_exists) {
        if (key__delete && pair->key) {
            key__delete(pair->key);
        }
        pair->key = NULL;
    }

    return FAILURE;
}
