/****************************************************************************
*
*                   SciTech Multi-platform Graphics Library
*
*  ========================================================================
*
*   Copyright (C) 1991-2004 SciTech Software, Inc. All rights reserved.
*
*   This file may be distributed and/or modified under the terms of the
*   GNU General Public License version 2.0 as published by the Free
*   Software Foundation and appearing in the file LICENSE.GPL included
*   in the packaging of this file.
*
*   Licensees holding a valid Commercial License for this product from
*   SciTech Software, Inc. may use this file in accordance with the
*   Commercial License Agreement provided with the Software.
*
*   This file is provided AS IS with NO WARRANTY OF ANY KIND, INCLUDING
*   THE WARRANTY OF DESIGN, MERCHANTABILITY AND FITNESS FOR A PARTICULAR
*   PURPOSE.
*
*   See http://www.scitechsoft.com/license/ for information about
*   the licensing options available and how to purchase a Commercial
*   License Agreement.
*
*   Contact license@scitechsoft.com if any conditions of this licensing
*   are not clear to you, or you have questions about licensing options.
*
*  ========================================================================
*
* Language:     ANSI C
* Environment:  Any
*
* Description:  Linked list manipulation code for the MGL.
*
****************************************************************************/

#include "mgl.h"

/*------------------------- Implementation --------------------------------*/

/****************************************************************************
PARAMETERS:
size    - Amount of memory to allocate for node

RETURNS:
Pointer to the allocated node's user space.

REMARKS:
Allocates the memory required for a node, adding a small header at the start
of the node. We return a reference to the user space of the node, as if it
had been allocated via malloc().
****************************************************************************/
void *_LST_newNode(
    int size)
{
    LST_BUCKET  *node;

    if ((node = (LST_BUCKET*)PM_calloc(1,size + sizeof(LST_BUCKET))) == NULL) {
        FATALERROR(grNoMem);
        return NULL;
        }
    return LST_USERSPACE(node);
}

/****************************************************************************
PARAMETERS:
node    - Node to free.

REMARKS:
Frees a node previously allocated with _LST_newnode().
****************************************************************************/
void _LST_freeNode(
    void *node)
{
    if (node)
        PM_free(LST_HEADER(node));
}

/****************************************************************************
RETURNS:
Pointer to a newly created list.

REMARKS:
Initialises a list and returns a pointer to it.
****************************************************************************/
LIST *_LST_create(void)
{
    LIST    *l = (LIST*)PM_malloc(sizeof(LIST));

    if (l != NULL) {
        l->count = 0;
        l->head = &(l->hz[0]);
        l->z = &(l->hz[1]);
        l->head->next = l->z->next = l->z;
        }
    else {
        FATALERROR(grNoMem);
        return NULL;
        }

    return l;
}

/****************************************************************************
PARAMETERS:
l           - List to kill
freeNode    - Pointer to user routine to free a node

REMARKS:
Kills the list l, by deleting all of the elements contained within the list
one by one and then deleting the list itself. Note that we call the user
supplied routine freeNode)() to free each list node. This allows the user
program to perform any extra processing needed to kill each node (if each
node contains pointers to other items on the heap for example). If no extra
processing is required, just pass the address of _LST_freenode(), ie:

    _LST_destroy(myList,_LST_freenode);

****************************************************************************/
void _LST_destroy(
    LIST *l,
    void (*freeNode)(
        void *node))
{
    LST_BUCKET  *n,*p;

    n = l->head->next;
    while (n != l->z) {         /* Free all nodes in list               */
        p = n;
        n = n->next;
        (*freeNode)(LST_USERSPACE(p));
        }
    PM_free(l);                /* Free the list itself                 */
}

/****************************************************************************
PARAMETERS:
l       - List to insert node into
node    - Pointer to user space of node to insert
after   - Pointer to user space of node to insert node after

REMARKS:
Inserts a new node into the list after the node 'after'. To insert a new
node at the beginning of the list, user the macro LST_HEAD in place of
'after'. ie:

    _LST_addAfter(mylist,node,LST_HEAD(mylist));

****************************************************************************/
void _LST_addAfter(
    LIST *l,
    void *node,
    void *after)
{
    LST_BUCKET  *n = LST_HEADER(node),*a = LST_HEADER(after);

    n->next = a->next;
    a->next = n;
    l->count++;
}

/****************************************************************************
PARAMETERS:
l       - List to insert node into
node    - Pointer to user space of node to insert

REMARKS:
Inserts a new node at the head of the list.
****************************************************************************/
void _LST_addToHead(
    LIST *l,
    void *node)
{
    _LST_addAfter(l,node,LST_HEAD(l));
}

/****************************************************************************
PARAMETERS:
l       - List to insert node into
node    - Pointer to user space of node to insert

REMARKS:
Inserts a new node at the end of the list.
****************************************************************************/
void _LST_addToTail(
    LIST *l,
    void *node)
{
    LST_BUCKET  *tail = l->head;
    LST_BUCKET  *next = tail->next;

    while (next != l->z) {
        tail = next;
        next = next->next;
        }
    _LST_addAfter(l,node,LST_USERSPACE(tail));
}

/****************************************************************************
PARAMETERS:
l       - List to delete node from.
node    - Node to delete the next node from

RETURNS:
Pointer to the deleted node's userspace.

REMARKS:
Removes the node AFTER 'node' from the list l.
****************************************************************************/
void *_LST_removeNext(
    LIST *l,
    void *node)
{
    LST_BUCKET  *n = LST_HEADER(node);

    node = LST_USERSPACE(n->next);
    n->next = n->next->next;
    l->count--;
    return node;
}

/****************************************************************************
PARAMETERS:
l       - List to obtain first node from

RETURNS:
Pointer to first node in list, NULL if list is empty.

REMARKS:
Returns a pointer to the user space of the first node in the list. If the
list is empty, we return NULL.
****************************************************************************/
void *_LST_first(
    LIST *l)
{
    LST_BUCKET  *n;

    n = l->head->next;
    return (n == l->z ? NULL : LST_USERSPACE(n));
}

/****************************************************************************
PARAMETERS:
prev    - Previous node in list to obtain next node from

RETURNS:
Pointer to the next node in the list, NULL at end of list.

REMARKS:
Returns a pointer to the user space of the next node in the list given a
pointer to the user space of the previous node. If we have reached the end
of the list, we return NULL. The end of the list is detected when the next
pointer of a node points back to itself, as does the dummy last node's next
pointer. This enables us to detect the end of the list without needed access
to the list data structure itself.

Note:   We do no checking to ensure that 'prev' is NOT a NULL pointer.
****************************************************************************/
void *_LST_next(
    void *prev)
{
    LST_BUCKET  *n = LST_HEADER(prev);

    n = n->next;
    return (n == n->next ? NULL : LST_USERSPACE(n));
}

/****************************************************************************
PARAMETERS:
l   - List to search for the node in
n   - Address of node to search for

RETURNS:
Pointer to previous node, or NULL if not found.

REMARKS:
Searches the specified linked list for the node. If the
node is found, we return a pointer to the previous node
on the list, otherwise we return NULL.
****************************************************************************/
void *_LST_findNode(
    LIST *l,
    void *n)
{
    LST_BUCKET  *prev = l->head;
    LST_BUCKET  *cnt = prev->next;

    while (cnt != l->z) {       /* Scan through entire list */
        if (LST_USERSPACE(cnt) == n)
            return LST_USERSPACE(prev);
        prev = cnt;
        cnt = cnt->next;
        }
    return NULL;
}
