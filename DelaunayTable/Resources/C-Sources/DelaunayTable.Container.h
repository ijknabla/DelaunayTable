
#pragma once

#include "DelaunayTable.h"

#include <stdbool.h>
#include <stddef.h>


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

extern void List__pop(
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
