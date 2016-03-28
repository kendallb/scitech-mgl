/****************************************************************************
*
*                         Techniques Class Library
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
* Language:     C++ 3.0
* Environment:  any
*
* Description:  Member functions for the dlist class, a class designed to
*               link a series of objects together into a doubly linked
*               list. All items placed in the list MUST be derived from
*               the class TCDListNode.
*
****************************************************************************/

#include "tcl/dlist.hpp"

/*--------------------------- Member functions ----------------------------*/

_TCGenDListCmp  TCGenDList::cmp;
TCDListNode     *TCGenDList::_z;

TCGenDList::TCGenDList()
/****************************************************************************
*
* Function:     TCGenDList::TCGenDList
*
* Description:  Constructor for the list class. We set the count of items
*               in the list to 0, and initialise the head and tail pointers
*               for the list. These both point to dummy nodes within the
*               list class, so that the dummy tail node always points to
*               itself ensuring we cannot iterate of the end of the list.
*               This also simplifies maintaining the head and tail pointers.
*
****************************************************************************/
{
    count = 0;
    head = &hz[0];              // Setup head and tail pointers
    z = &hz[1];
    head->next = z->next = z;
    z->prev = head->prev = head;
}

TCGenDList::~TCGenDList()
/****************************************************************************
*
* Function:     TCGenDList::~TCGenDList
*
* Description:  Destructor for the list class. All we do here is ask the
*               list to empty itself.
*
****************************************************************************/
{
    empty();
}

void TCGenDList::empty(void)
/****************************************************************************
*
* Function:     TCGenDList::empty
*
* Description:  Empties the list of all elements. We do this by stepping
*               through the list deleting all the elements as we go.
*
****************************************************************************/
{
    TCDListNode *temp;

    while (head->next != z) {
        temp = head->next;
        head->next = head->next->next;
        delete temp;
        }
    z->prev = head;
    count = 0;
}

TCDListNode* TCGenDList::merge(TCDListNode *a,TCDListNode *b,TCDListNode*& end)
/****************************************************************************
*
* Function:     TCGenDList::merge
* Parameters:   a,b     - Sublist's to merge
*               end     - Pointer to end of merged list
* Returns:      Pointer to the merged sublists.
*
* Description:  Merges two sorted lists of nodes together into a single
*               sorted list, and sets a pointer to the end of the newly
*               merged lists.
*
****************************************************************************/
{
    TCDListNode *c;

    // Go through the lists, merging them together in sorted order

    c = _z;
    while (a != _z && b != _z) {
        if (cmp(a,b) <= 0) {
            c->next = a; c = a; a = a->next;
            }
        else {
            c->next = b; c = b; b = b->next;
            }
        };

    // If one of the lists is not exhausted, then re-attach it to the end
    // of the newly merged list

    if (a != _z) c->next = a;
    if (b != _z) c->next = b;

    // Set end to point to the end of the newly merged list

    while (c->next != _z) c = c->next;
    end = c;

    // Determine the start of the merged lists, and reset _z to point to
    // itself

    c = _z->next; _z->next = _z;
    return c;
}

void TCGenDList::sort(_TCGenDListCmp cmp_func)
/****************************************************************************
*
* Function:     TCGenDList::sort
* Parameters:   cmp - Function to compare the contents of two ListNode's.
*
* Description:  Mergesort's all the nodes in the list. 'cmp' must point to
*               a comparison function that can compare the contents of
*               two ListNode's. 'cmp' should work the same as strcmp(), in
*               terms of the values it returns.
*
****************************************************************************/
{
    int         i,N;
    TCDListNode *a,*b;      // Pointers to sublists to merge
    TCDListNode *c;         // Pointer to end of sorted sublists
    TCDListNode *todo;      // Pointer to sublists yet to be sorted
    TCDListNode *t;         // Temporary

    // Set up globals required by DList::merge().
    TCDListNode *head = this->head;
    _z = z;
    cmp = cmp_func;

    for (N = 1,a = _z; a != head->next; N = N + N) {
        todo = head->next; c = head;
        while (todo != _z) {

            // Build first sublist to be merged, and splice from main list

            a = t = todo;
            for (i = 1; i < N; i++) t = t->next;
            b = t->next; t->next = _z; t = b;

            // Build second sublist to be merged and splice from main list

            for (i = 1; i < N; i++) t = t->next;
            todo = t->next; t->next = _z;

            // Merge the two sublists created, and set 'c' to point to the
            // end of the newly merged sublists.

            c->next = merge(a,b,t); c = t;
            }
        }

    // After sorting the list, traverse the list from head to tail fixing
    // all the previous pointers as we go.

    a = b = head;
    b = b->next;
    while (1) {
        b->prev = a;
        if (b == _z)
            break;
        a = a->next;
        b = b->next;
        }
}

// Virtual destructor for TCDListNode's. Does nothing.

TCDListNode::~TCDListNode()
{
}
