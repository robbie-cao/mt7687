#include <stdio.h>
#include "gattc_utils.h"


#ifndef WIN32
#include "FreeRTOS.h"

#include "portable.h"
#endif


void *gattc_malloc(uint32_t size)
{
#ifndef WIN32
    return pvPortMalloc((size_t)size);
#else
    return malloc(size);
#endif
}

void gattc_free(void *ptr)
{
#ifndef WIN32
    vPortFree(ptr);
#else

    free(ptr);

#endif
}

/*****************************************************************************
 * FUNCTION
 *  IsListEmpty
 * DESCRIPTION
 *
 * PARAMETERS
 *  list        [IN]
 * RETURNS
 *
 *****************************************************************************/
uint8_t Is_List_Empty(ListNode *list)
{
    return ((list)->Flink == (list));
}

/*****************************************************************************
 * FUNCTION
 *  RemoveEntryList
 * DESCRIPTION
 *  Remove the given entry from the list.
 * PARAMETERS
 *  entry       [OUT]
 * RETURNS
 *  void
 *****************************************************************************/
void Remove_Entry_List(ListNode *entry)
{
    assert(Is_List_Circular(entry));
    entry->Blink->Flink = entry->Flink;
    entry->Flink->Blink = entry->Blink;
    assert(Is_List_Circular(entry->Blink));
    Init_List_Entry(entry);
}


/*****************************************************************************
 * FUNCTION
 *  InitializeListEntry
 * DESCRIPTION
 *
 * PARAMETERS
 *  list        [IN]
 * RETURNS
 *  void
 *****************************************************************************/
void Init_List_Entry(ListNode *list)
{
    list->Flink = 0;
    list->Blink = 0;
}

/*****************************************************************************
 * FUNCTION
 *  InitializeListHead
 * DESCRIPTION
 *
 * PARAMETERS
 *  list        [IN]
 * RETURNS
 *  void
 *****************************************************************************/
void Init_List_Head(ListNode *list)
{
    list->Flink = list;
    list->Blink = list;
}


/*****************************************************************************
 * FUNCTION
 *  _InsertTailList
 * DESCRIPTION
 *  Insert an entry at the tail of the list specified by head.
 * PARAMETERS
 *  head        [OUT]
 *  entry       [OUT]
 * RETURNS
 *  void
 *****************************************************************************/
void _Insert_Tail_List(ListNode *head, ListNode *entry)
{
    entry->Flink = head;
    entry->Blink = head->Blink;
    head->Blink->Flink = entry;
    head->Blink = entry;
    assert(Is_Node_Connected(entry));
}


/*****************************************************************************
 * FUNCTION
 *  InsertTailList
 * DESCRIPTION
 *
 * PARAMETERS
 *  head        [OUT]
 *  entry       [OUT]
 * RETURNS
 *  void
 *****************************************************************************/
void Insert_Tail_List(ListNode *head, ListNode *entry)
{
    assert(Is_List_Circular(head));
    _Insert_Tail_List(head, entry);
    assert(Is_List_Circular(head));
}

bool Is_List_Circular(ListNode *list)
{

    ListNode *tmp = list;

    if (!Is_Node_Connected(list)) {
        return (false);
    }

    for (tmp = tmp->Flink; tmp != list; tmp = tmp->Flink) {
        if (!Is_Node_Connected(tmp)) {
            return (false);
        }
    }
    return true;
}


/*****************************************************************************
 * FUNCTION
 *  _RemoveHeadList
 * DESCRIPTION
 *  Remove the first entry on the list specified by head.
 * PARAMETERS
 *  head        [OUT]
 * RETURNS
 *
 *****************************************************************************/
ListNode *_Remove_Head_List(ListNode *head)
{
    ListNode *first;

    first = head->Flink;
    first->Flink->Blink = head;
    head->Flink = first->Flink;
    assert(Is_List_Circular(head));
    return (first);

}


/*****************************************************************************
 * FUNCTION
 *  RemoveHeadList
 * DESCRIPTION
 *
 * PARAMETERS
 *  head        [OUT]
 * RETURNS
 *
 *****************************************************************************/
ListNode *Remove_Head_List(ListNode *head)
{
    assert(Is_List_Circular(head));
    return (_Remove_Head_List(head));
}


/*****************************************************************************
 * FUNCTION
 *  _InsertHeadList
 * DESCRIPTION
 *  Insert an entry at the head of the list specified by head.
 * PARAMETERS
 *  head        [OUT]
 *  entry       [OUT]
 * RETURNS
 *  void
 *****************************************************************************/
void _Insert_Head_List(ListNode *head, ListNode *entry)
{
    entry->Flink = head->Flink;
    entry->Blink = head;
    head->Flink->Blink = entry;
    head->Flink = entry;
    assert(Is_Node_Connected(entry));
}

bool Is_Node_OnList(ListNode *head, ListNode *node)
{
    ListNode *tmpNode;

    assert(Is_List_Circular(head));
    tmpNode = Get_Head_List(head);

    while (tmpNode != head) {
        if (tmpNode == node) {
            return (true);
        }

        tmpNode = tmpNode->Flink;
    }
    return (false);

}


