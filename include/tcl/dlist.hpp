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
* Description:  Header file for a class to link objects together into a
*               doubly linked list.
*
****************************************************************************/

#ifndef __TCL_DLIST_HPP
#define __TCL_DLIST_HPP

#ifndef __SCITECH_H
#include "scitech.h"
#endif

/*--------------------------- Class Definition ----------------------------*/

//---------------------------------------------------------------------------
// The TCDListNode class is a simple class used to link the objects in the
// list together. To put anything useful into the list, you must derive the
// object placed into the list from TCDListNode.
//---------------------------------------------------------------------------

class TCDListNode {
protected:
    TCDListNode *next;
    TCDListNode *prev;

    friend class TCGenDList;
    friend class TCGenDListIterator;
public:
            // Constructor to satisfy some compilers :-(
            TCDListNode() {};

            // Virtual destructor to delete a list node
    virtual ~TCDListNode();
    };

//---------------------------------------------------------------------------
// The list class is designed to manipulate a list of TCDListNode objects.
// In the simple form, TCDListNode objects contain nothing special. To add
// an arbitrary class to the list, you must derive the class from TCDListNode
// (either through single or multiple inheritance).
//---------------------------------------------------------------------------

typedef int (*_TCGenDListCmp)(TCDListNode*,TCDListNode*);

class TCGenDList {
protected:
    ulong       count;      // Number of objects in list
    TCDListNode *head;      // Pointer to first node in list
    TCDListNode *z;         // Pointer to last node in list
    TCDListNode hz[2];      // Space for head and z nodes

    static  _TCGenDListCmp cmp;
    static  TCDListNode *_z;

            // Protected member to merge two lists together
    static  TCDListNode* merge(TCDListNode* a,TCDListNode* b, TCDListNode*& end);

public:
            // Constructor
            TCGenDList();

            // Destructor
            ~TCGenDList();

            // Method to examine the first node in the List
            TCDListNode* peekHead() const;

            // Method to examine the last node in the List
            TCDListNode* peekTail() const;

            // Method to return the next node in the list
            TCDListNode* next(TCDListNode* node) const;

            // Method to return the prev node in the list
            TCDListNode* prev(TCDListNode* node) const;

            // Method to add a node to the head of the list
            void addToHead(TCDListNode* node);

            // Method to add a node to the tail of the list
            void addToTail(TCDListNode* node);

            // Method to add a node after another node in the list
            void addAfter(TCDListNode* node,TCDListNode* after);

            // Method to add a node before another node in the list
            void addBefore(TCDListNode* node,TCDListNode* before);

            // Method to detach a specified TCDListNode from the list.
            TCDListNode* remove(TCDListNode* node);

            // Method to detach a specified TCDListNode from the list.
            TCDListNode* removeNext(TCDListNode *prev);

            // Method to detach the first node from the list.
            TCDListNode* removeFromHead();

            // Method to detach the last node from the list.
            TCDListNode* removeFromTail();

            // Sort the linked list of objects
            void sort(_TCGenDListCmp cmp);

            // Empties the entire list by destroying all nodes
            void empty();

            // Returns the number of items in the list
            ulong numberOfItems() const { return count; };

            // Returns true if the list is empty
            ibool isEmpty() const   { return count == 0; };

private:
    friend class TCGenDListIterator;
    };

//---------------------------------------------------------------------------
// The list iterator is the class of iterator that is used to step through
// the elements in the list.
//---------------------------------------------------------------------------

class TCGenDListIterator {
protected:
    TCDListNode *cursor;
    TCGenDList  *beingIterated;
public:
            // Constructor
            TCGenDListIterator();

            // Constructor given a list reference
            TCGenDListIterator(const TCGenDList& l);

            // Intialise a list iterator from a list
            void operator = (const TCGenDList& l);

            // assignment operator between two listIterators
            void operator = (const TCGenDListIterator& i);

            // Overloaded cast to an integer
            operator int ();

            // Convert the iterator to the corresponding node
            TCDListNode* node();

            // Pre-increment operator for the iterator
            TCDListNode* operator ++ ();

            // Post-increment operator for the iterator
            TCDListNode* operator ++ (int);

            // Pre-decrement operator for the iterator
            TCDListNode* operator -- ();

            // Post-decrement operator for the iterator
            TCDListNode* operator -- (int);

            // Method to restart the iterator at head of list
            void restart();

            // Method to restart the iterator at tail of list
            void restartTail();
    };

//---------------------------------------------------------------------------
// Template wrapper class for declaring Type Safe doubly linked lists.
//---------------------------------------------------------------------------

template <class T> class TCDList : public TCGenDList {
public:
            T* peekHead() const
                { return (T*)TCGenDList::peekHead(); };
            T* peekTail() const
                { return (T*)TCGenDList::peekTail(); };
            T* next(T* node) const
                { return (T*)TCGenDList::next(node); };
            T* prev(T* node) const
                { return (T*)TCGenDList::prev(node); };
            T* remove(T* node)
                { return (T*)TCGenDList::remove(node); };
            T* removeNext(T* node)
                { return (T*)TCGenDList::removeNext(node); };
            T* removeFromHead()
                { return (T*)TCGenDList::removeFromHead(); };
            T* removeFromTail()
                { return (T*)TCGenDList::removeFromTail(); };
            void sort(int (*cmp)(T*,T*))
                { TCGenDList::sort((_TCGenDListCmp)cmp); };
    };

template <class T> class TCDListIterator : public TCGenDListIterator {
public:
            TCDListIterator()
                : TCGenDListIterator() {};
            TCDListIterator(const TCDList<T>& l)
                : TCGenDListIterator(l) {};
            void operator = (const TCDList<T>& l)
                { TCGenDListIterator::operator=(l); };
            void operator = (const TCDListIterator<T>& i)
                { TCGenDListIterator::operator=(i); };
            T* node()
                { return (T*)TCGenDListIterator::node(); };
            T* operator ++ ()
                { return (T*)TCGenDListIterator::operator++(); };
            T* operator ++ (int)
                { return (T*)TCGenDListIterator::operator++(1); };
            T* operator -- ()
                { return (T*)TCGenDListIterator::operator--(); };
            T* operator -- (int i)
                { return (T*)TCGenDListIterator::operator--(i); };
    };

/*------------------------ Inline member functions ------------------------*/

inline TCDListNode* TCGenDList::peekHead() const
/****************************************************************************
*
* Function:     TCGenDList::peekHead
* Returns:      Returns a pointer to the head node on the list, or NULL if
*               the list is empty.
*
****************************************************************************/
{
    return (head->next == z ? NULL : head->next);
}

inline TCDListNode* TCGenDList::peekTail() const
/****************************************************************************
*
* Function:     TCGenDList::peekTail
* Returns:      Returns a pointer to the tail node on the list, or NULL if
*               the list is empty.
*
****************************************************************************/
{
    return (z->prev == head ? NULL : z->prev);
}

inline TCDListNode* TCGenDList::next(TCDListNode *node) const
/****************************************************************************
*
* Function:     TCGenDList::next
* Parameters:   node    - Node to obtain next from
* Returns:      Pointer to the next node in the list, NULL if none.
*
****************************************************************************/
{
    return (node->next == z ? NULL : node->next);
}

inline TCDListNode* TCGenDList::prev(TCDListNode *node) const
/****************************************************************************
*
* Function:     TCGenDList::prev
* Parameters:   node    - Node to obtain prev from
* Returns:      Pointer to the previous node in the list, NULL if none.
*
****************************************************************************/
{
    return (node->prev == head ? NULL : node->prev);
}

inline void TCGenDList::addAfter(TCDListNode* node,TCDListNode* after)
/****************************************************************************
*
* Function:     TCGenDList::addAfter
* Parameters:   node    - New node to attach to list
*               after   - Node to attach new node after in list
*
* Description:  Attaches a new node after a specified node in the list.
*               The list must contain at least one node, and after may
*               be the tail node of the list.
*
****************************************************************************/
{
    node->next = after->next;
    after->next = node;
    node->prev = after;
    node->next->prev = node;
    count++;
}

inline void TCGenDList::addBefore(TCDListNode* node,TCDListNode* before)
/****************************************************************************
*
* Function:     TCGenDList::addBefore
* Parameters:   node    - New node to attach to list
*               before  - Node to attach new node before in list
*
* Description:  Attaches a new node before a specified node in the list.
*               The list must contain at least one node, and before may
*               be the tail node of the list.
*
****************************************************************************/
{
    node->next = before;
    before->prev->next = node;
    node->prev = before->prev;
    before->prev = node;
    count++;
}

inline void TCGenDList::addToHead(TCDListNode* node)
/****************************************************************************
*
* Function:     TCGenDList::addToHead
* Parameters:   node    - Node to add to list
*
* Description:  Attaches the node to the head of the list.
*
****************************************************************************/
{
    addAfter(node,head);
}

inline void TCGenDList::addToTail(TCDListNode* node)
/****************************************************************************
*
* Function:     TCGenDList::addToTail
* Parameters:   node    - Node to add to list
*
* Description:  Attaches the node to the tail of the list.
*
****************************************************************************/
{
    addAfter(node,z->prev);
}

inline TCDListNode* TCGenDList::remove(TCDListNode* node)
/****************************************************************************
*
* Function:     TCGenDList::remove
* Parameters:   node    - Pointer to node remove from the list
* Returns:      Node removed from list.
*
* Description:  Removes the specified node from the list.
*
****************************************************************************/
{
    node->next->prev = node->prev;
    node->prev->next = node->next;
    count--;
    return node;
}

inline TCDListNode* TCGenDList::removeNext(TCDListNode* prev)
/****************************************************************************
*
* Function:     TCGenDList::removeNext
* Parameters:   prev    - Pointer to the previous node in the list
* Returns:      Node removed from list, or NULL if prev is the last node.
*
* Description:  Removes the specified node from the list.
*
****************************************************************************/
{
    TCDListNode*    node;

    if ((node = prev->next) != z)
        return remove(node);
    else
        return NULL;
}

inline TCDListNode* TCGenDList::removeFromHead()
/****************************************************************************
*
* Function:     TCGenDList::removeFromHead
* Returns:      Pointer to the node removed from the head of the list,
*               or NULL if the list is empty.
*
****************************************************************************/
{
    return removeNext(head);
}

inline TCDListNode* TCGenDList::removeFromTail()
/****************************************************************************
*
* Function:     TCGenDList::removeFromTail
* Returns:      Pointer to the node removed from the tail of the list,
*               or NULL if the list is empty.
*
****************************************************************************/
{
    return removeNext(z->prev->prev);
}

inline TCGenDListIterator::TCGenDListIterator()
/****************************************************************************
*
* Function:     TCGenDListIterator::TCGenDListIterator
*
* Description:  Default constructor for a dlist iterator.
*
****************************************************************************/
{
    cursor = NULL;
    beingIterated = NULL;
}

inline TCGenDListIterator::TCGenDListIterator(const TCGenDList& l)
/****************************************************************************
*
* Function:     TCGenDListIterator::TCGenDListIterator
* Parameters:   l   - DList to construct iterator from
*
* Description:  Constructor for a TCGenDListIterator given a reference to a list
*               to iterate.
*
****************************************************************************/
{
    beingIterated = (TCGenDList*)&l;
    cursor = l.head->next;
}

inline void TCGenDListIterator::operator = (const TCGenDList& l)
/****************************************************************************
*
* Function:     TCGenDListIterator::operator =
* Parameters:   l   - TCGenDList to assign to iterator
*
* Description:  Assignment operator for a DListIterator given a reference to
*               a list to iterate.
*
****************************************************************************/
{
    beingIterated = (TCGenDList*)&l;
    cursor = l.head->next;
}

inline void TCGenDListIterator::operator = (const TCGenDListIterator& i)
/****************************************************************************
*
* Function:     TCGenDListIterator::operator =
* Parameters:   i   - Iterator to assign from
*
* Description:  Assignment operator for a DListIterator given a reference to
*               another DListIterator.
*
****************************************************************************/
{
    beingIterated = i.beingIterated;
    cursor = i.cursor;
}

inline TCGenDListIterator::operator int()
/****************************************************************************
*
* Function:     TCGenDListIterator::operator int
*
* Description:  Overloaded cast to integer for the list iterator. Evaluates
*               to 0 when the end of the list is reached.
*
****************************************************************************/
{
    return (cursor != beingIterated->z && cursor != beingIterated->head);
}

inline TCDListNode* TCGenDListIterator::node()
/****************************************************************************
*
* Function:     TCGenDListIterator::node
* Returns:      Returns a reference to the node in the list.
*
****************************************************************************/
{
    return ((int)*this ? cursor : NULL);
}

inline TCDListNode* TCGenDListIterator::operator ++ ()
/****************************************************************************
*
* Function:     TCGenDListIterator::operator ++
* Returns:      Pointer to node after incrementing
*
* Description:  Increments the iterator by moving it to the next object
*               in the list. We return a pointer to the node pointed to
*               after the increment takes place.
*
****************************************************************************/
{
    cursor = cursor->next;
    return (cursor == beingIterated->z ? NULL : cursor);
}

inline TCDListNode* TCGenDListIterator::operator ++ (int)
/****************************************************************************
*
* Function:     TCGenDListIterator::operator ++ (int)
* Returns:      Pointer to node before incrementing
*
* Description:  Increments the iterator by moving it to the next object
*               in the list. We return a pointer to the node pointed to
*               before the increment takes place.
*
****************************************************************************/
{
    TCDListNode *prev = cursor;

    cursor = cursor->next;
    return (prev == beingIterated->z ? NULL : prev);
}

inline TCDListNode* TCGenDListIterator::operator -- ()
/****************************************************************************
*
* Function:     TCGenDListIterator::operator --
* Returns:      Pointer to node after decrementing
*
* Description:  Decrements the iterator by moving it to the next object
*               in the list. We return a pointer to the node pointed to
*               after the decrement takes place.
*
****************************************************************************/
{
    cursor = cursor->prev;
    return (cursor == beingIterated->head ? NULL : cursor);
}

inline TCDListNode* TCGenDListIterator::operator -- (int)
/****************************************************************************
*
* Function:     TCGenDListIterator::operator -- (int)
* Returns:      Pointer to node before decrementing
*
* Description:  Decrements the iterator by moving it to the next object
*               in the list. We return a pointer to the node pointed to
*               before the decrement takes place.
*
****************************************************************************/
{
    TCDListNode *prev = cursor;

    cursor = cursor->prev;
    return (prev == beingIterated->head ? NULL : prev);
}

inline void TCGenDListIterator::restart()
/****************************************************************************
*
* Function:     TCGenDListIterator::restart
*
* Description:  Restart the iterator at the beginning of the list.
*
****************************************************************************/
{
    cursor = beingIterated->head->next;
}

inline void TCGenDListIterator::restartTail()
/****************************************************************************
*
* Function:     TCGenDListIterator::restartTail
*
* Description:  Restart the iterator at the end of the list.
*
****************************************************************************/
{
    cursor = beingIterated->z->prev;
}

#endif  // __TCL_DLIST_HPP
