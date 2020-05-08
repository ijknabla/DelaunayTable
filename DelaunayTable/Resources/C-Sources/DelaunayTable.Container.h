
#pragma once


/** # List
 * bidirectional linked list
 */
typedef void* List__element_t;
typedef void List__element__delete_function (List__element_t);

typedef struct ListNode_tag {
    List__element_t element;
    struct ListNode_tag* prev;
    struct ListNode_tag* next;
} ListNode;

typedef struct{
    ListNode* first;
    ListNode* last;
} List;
