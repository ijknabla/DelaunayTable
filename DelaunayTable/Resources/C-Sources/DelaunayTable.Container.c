
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

/// ### extern functions for HashMap
HashMap* HashMap__new(
) {
    HashMap* this = (HashMap*) malloc(sizeof(HashMap));
    if (!this) {goto error;}

    this->size = 0;
    this->max_size = (2 << 6) - 1;
    this->pairs = (Map__Pair*) CALLOC(
        this->max_size, sizeof(Map__Pair)
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

/*
void HashMap__clear(
    HashMap* const this,
    Map__key__delete_function* const key__delete,
    Map__value__delete_function* const value__delete
) {
    return;
}
*/

void HashMap__delete(
    HashMap* const this,
    Map__key__delete_function* const key__delete,
    Map__value__delete_function* const value__delete
) {
    for (size_t i = 0 ; i < this->max_size ; i++) {
        const Map__Pair pair = this->pairs[i];
        if (!Map__Pair__empty(&pair)) {
            if (key__delete && pair.key) {
                key__delete(pair.key);
            }
            if (value__delete && pair.value) {
                value__delete(pair.value);
            }
        }
    }

    FREE(this->pairs);
    FREE(this);
}
