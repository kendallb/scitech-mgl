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
* Language:     C++ 3.0
* Environment:  any
*
* Description:  Header file for a class to link objects together into a
*               doubly linked list.
*
****************************************************************************/

#ifndef __GM_DLIST_HPP
#define __GM_DLIST_HPP

#ifndef __SCITECH_H
#include "scitech.h"
#endif

/*--------------------------- Class Definition ----------------------------*/

//---------------------------------------------------------------------------
// The FX_DListNode class is a simple class used to link the objects in the
// list together. To put anything useful into the list, you must derive the
// object placed into the list from FX_DListNode.
//---------------------------------------------------------------------------

class FX_DListNode {
protected:
    FX_DListNode    *next;
    FX_DListNode    *prev;

    friend class FX_GenDList;
    friend class FX_GenDListIterator;
public:
            // Constructor to satisfy some compilers :-(
            FX_DListNode() {};

            // Virtual destructor to delete a list node
    virtual ~FX_DListNode();
    };

//---------------------------------------------------------------------------
// The list class is designed to manipulate a list of FX_DListNode objects.
// In the simple form, FX_DListNode objects contain nothing special. To add
// an arbitrary class to the list, you must derive the class from FX_DListNode
// (either through single or multiple inheritance).
//---------------------------------------------------------------------------

typedef int (*_FX_GenDListCmp)(FX_DListNode*,FX_DListNode*);

class FX_GenDList {
protected:
    ulong       count;      // Number of objects in list
    FX_DListNode    *head;      // Pointer to first node in list
    FX_DListNode    *z;         // Pointer to last node in list
    FX_DListNode    hz[2];      // Space for head and z nodes

    static  _FX_GenDListCmp cmp;
    static  FX_DListNode    *_z;

            // Protected member to merge two lists together
    static  FX_DListNode* merge(FX_DListNode* a,FX_DListNode* b, FX_DListNode*& end);

public:
            // Constructor
            FX_GenDList();

            // Destructor
            ~FX_GenDList();

            // Method to examine the first node in the List
            FX_DListNode* peekHead() const;

            // Method to examine the last node in the List
            FX_DListNode* peekTail() const;

            // Method to return the next node in the list
            FX_DListNode* next(FX_DListNode* node) const;

            // Method to return the prev node in the list
            FX_DListNode* prev(FX_DListNode* node) const;

            // Method to add a node to the head of the list
            void addToHead(FX_DListNode* node);

            // Method to add a node to the tail of the list
            void addToTail(FX_DListNode* node);

            // Method to add a node after another node in the list
            void addAfter(FX_DListNode* node,FX_DListNode* after);

            // Method to add a node before another node in the list
            void addBefore(FX_DListNode* node,FX_DListNode* before);

            // Method to detach a specified FX_DListNode from the list.
            FX_DListNode* remove(FX_DListNode* node);

            // Method to detach a specified FX_DListNode from the list.
            FX_DListNode* removeNext(FX_DListNode *prev);

            // Method to detach the first node from the list.
            FX_DListNode* removeFromHead();

            // Method to detach the last node from the list.
            FX_DListNode* removeFromTail();

            // Sort the linked list of objects
            void sort(_FX_GenDListCmp cmp);

            // Empties the entire list by destroying all nodes
            void empty();

            // Returns the number of items in the list
            ulong numberOfItems() const { return count; };

            // Returns true if the list is empty
            ibool isEmpty() const   { return count == 0; };

private:
    friend class FX_GenDListIterator;
    };

//---------------------------------------------------------------------------
// The list iterator is the class of iterator that is used to step through
// the elements in the list.
//---------------------------------------------------------------------------

class FX_GenDListIterator {
protected:
    FX_DListNode    *cursor;
    FX_GenDList *beingIterated;
public:
            // Constructor
            FX_GenDListIterator();

            // Constructor given a list reference
            FX_GenDListIterator(const FX_GenDList& l);

            // Intialise a list iterator from a list
            void operator = (const FX_GenDList& l);

            // assignment operator between two listIterators
            void operator = (const FX_GenDListIterator& i);

            // Overloaded cast to an integer
            operator int ();

            // Convert the iterator to the corresponding node
            FX_DListNode* node();

            // Pre-increment operator for the iterator
            FX_DListNode* operator ++ ();

            // Post-increment operator for the iterator
            FX_DListNode* operator ++ (int);

            // Pre-decrement operator for the iterator
            FX_DListNode* operator -- ();

            // Post-decrement operator for the iterator
            FX_DListNode* operator -- (int);

            // Method to restart the iterator at head of list
            void restart();

            // Method to restart the iterator at tail of list
            void restartTail();
    };

//---------------------------------------------------------------------------
// Template wrapper class for declaring Type Safe doubly linked lists.
//---------------------------------------------------------------------------

template <class T> class FX_DList : public FX_GenDList {
public:
            T* peekHead() const
                { return (T*)FX_GenDList::peekHead(); };
            T* peekTail() const
                { return (T*)FX_GenDList::peekTail(); };
            T* next(T* node) const
                { return (T*)FX_GenDList::next(node); };
            T* prev(T* node) const
                { return (T*)FX_GenDList::prev(node); };
            T* remove(T* node)
                { return (T*)FX_GenDList::remove(node); };
            T* removeNext(T* node)
                { return (T*)FX_GenDList::removeNext(node); };
            T* removeFromHead()
                { return (T*)FX_GenDList::removeFromHead(); };
            T* removeFromTail()
                { return (T*)FX_GenDList::removeFromTail(); };
            void sort(int (*cmp)(T*,T*))
                { FX_GenDList::sort((_FX_GenDListCmp)cmp); };
    };

template <class T> class FX_DListIterator : public FX_GenDListIterator {
public:
            FX_DListIterator()
                : FX_GenDListIterator() {};
            FX_DListIterator(const FX_DList<T>& l)
                : FX_GenDListIterator(l) {};
            void operator = (const FX_DList<T>& l)
                { FX_GenDListIterator::operator=(l); };
            void operator = (const FX_DListIterator<T>& i)
                { FX_GenDListIterator::operator=(i); };
            T* node()
                { return (T*)FX_GenDListIterator::node(); };
            T* operator ++ ()
                { return (T*)FX_GenDListIterator::operator++(); };
            T* operator ++ (int)
                { return (T*)FX_GenDListIterator::operator++(1); };
            T* operator -- ()
                { return (T*)FX_GenDListIterator::operator--(); };
            T* operator -- (int i)
                { return (T*)FX_GenDListIterator::operator--(i); };
    };

/*------------------------ Inline member functions ------------------------*/

inline FX_DListNode* FX_GenDList::peekHead() const
/****************************************************************************
*
* Function:     FX_GenDList::peekHead
* Returns:      Returns a pointer to the head node on the list, or NULL if
*               the list is empty.
*
****************************************************************************/
{
    return (head->next == z ? NULL : head->next);
}

inline FX_DListNode* FX_GenDList::peekTail() const
/****************************************************************************
*
* Function:     FX_GenDList::peekTail
* Returns:      Returns a pointer to the tail node on the list, or NULL if
*               the list is empty.
*
****************************************************************************/
{
    return (z->prev == head ? NULL : z->prev);
}

inline FX_DListNode* FX_GenDList::next(FX_DListNode *node) const
/****************************************************************************
*
* Function:     FX_GenDList::next
* Parameters:   node    - Node to obtain next from
* Returns:      Pointer to the next node in the list, NULL if none.
*
****************************************************************************/
{
    return (node->next == z ? NULL : node->next);
}

inline FX_DListNode* FX_GenDList::prev(FX_DListNode *node) const
/****************************************************************************
*
* Function:     FX_GenDList::prev
* Parameters:   node    - Node to obtain prev from
* Returns:      Pointer to the previous node in the list, NULL if none.
*
****************************************************************************/
{
    return (node->prev == head ? NULL : node->prev);
}

inline void FX_GenDList::addAfter(FX_DListNode* node,FX_DListNode* after)
/****************************************************************************
*
* Function:     FX_GenDList::addAfter
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

inline void FX_GenDList::addBefore(FX_DListNode* node,FX_DListNode* before)
/****************************************************************************
*
* Function:     FX_GenDList::addBefore
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

inline void FX_GenDList::addToHead(FX_DListNode* node)
/****************************************************************************
*
* Function:     FX_GenDList::addToHead
* Parameters:   node    - Node to add to list
*
* Description:  Attaches the node to the head of the list.
*
****************************************************************************/
{
    addAfter(node,head);
}

inline void FX_GenDList::addToTail(FX_DListNode* node)
/****************************************************************************
*
* Function:     FX_GenDList::addToTail
* Parameters:   node    - Node to add to list
*
* Description:  Attaches the node to the tail of the list.
*
****************************************************************************/
{
    addAfter(node,z->prev);
}

inline FX_DListNode* FX_GenDList::remove(FX_DListNode* node)
/****************************************************************************
*
* Function:     FX_GenDList::remove
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

inline FX_DListNode* FX_GenDList::removeNext(FX_DListNode* prev)
/****************************************************************************
*
* Function:     FX_GenDList::removeNext
* Parameters:   prev    - Pointer to the previous node in the list
* Returns:      Node removed from list, or NULL if prev is the last node.
*
* Description:  Removes the specified node from the list.
*
****************************************************************************/
{
    FX_DListNode*   node;

    if ((node = prev->next) != z)
        return remove(node);
    else
        return NULL;
}

inline FX_DListNode* FX_GenDList::removeFromHead()
/****************************************************************************
*
* Function:     FX_GenDList::removeFromHead
* Returns:      Pointer to the node removed from the head of the list,
*               or NULL if the list is empty.
*
****************************************************************************/
{
    return removeNext(head);
}

inline FX_DListNode* FX_GenDList::removeFromTail()
/****************************************************************************
*
* Function:     FX_GenDList::removeFromTail
* Returns:      Pointer to the node removed from the tail of the list,
*               or NULL if the list is empty.
*
****************************************************************************/
{
    return removeNext(z->prev->prev);
}

inline FX_GenDListIterator::FX_GenDListIterator()
/****************************************************************************
*
* Function:     FX_GenDListIterator::FX_GenDListIterator
*
* Description:  Default constructor for a dlist iterator.
*
****************************************************************************/
{
    cursor = NULL;
    beingIterated = NULL;
}

inline FX_GenDListIterator::FX_GenDListIterator(const FX_GenDList& l)
/****************************************************************************
*
* Function:     FX_GenDListIterator::FX_GenDListIterator
* Parameters:   l   - DList to construct iterator from
*
* Description:  Constructor for a FX_GenDListIterator given a reference to a list
*               to iterate.
*
****************************************************************************/
{
    beingIterated = (FX_GenDList*)&l;
    cursor = l.head->next;
}

inline void FX_GenDListIterator::operator = (const FX_GenDList& l)
/****************************************************************************
*
* Function:     FX_GenDListIterator::operator =
* Parameters:   l   - FX_GenDList to assign to iterator
*
* Description:  Assignment operator for a DListIterator given a reference to
*               a list to iterate.
*
****************************************************************************/
{
    beingIterated = (FX_GenDList*)&l;
    cursor = l.head->next;
}

inline void FX_GenDListIterator::operator = (const FX_GenDListIterator& i)
/****************************************************************************
*
* Function:     FX_GenDListIterator::operator =
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

inline FX_GenDListIterator::operator int()
/****************************************************************************
*
* Function:     FX_GenDListIterator::operator int
*
* Description:  Overloaded cast to integer for the list iterator. Evaluates
*               to 0 when the end of the list is reached.
*
****************************************************************************/
{
    return (cursor != beingIterated->z && cursor != beingIterated->head);
}

inline FX_DListNode* FX_GenDListIterator::node()
/****************************************************************************
*
* Function:     FX_GenDListIterator::node
* Returns:      Returns a reference to the node in the list.
*
****************************************************************************/
{
    return ((int)*this ? cursor : NULL);
}

inline FX_DListNode* FX_GenDListIterator::operator ++ ()
/****************************************************************************
*
* Function:     FX_GenDListIterator::operator ++
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

inline FX_DListNode* FX_GenDListIterator::operator ++ (int)
/****************************************************************************
*
* Function:     FX_GenDListIterator::operator ++ (int)
* Returns:      Pointer to node before incrementing
*
* Description:  Increments the iterator by moving it to the next object
*               in the list. We return a pointer to the node pointed to
*               before the increment takes place.
*
****************************************************************************/
{
    FX_DListNode    *prev = cursor;

    cursor = cursor->next;
    return (prev == beingIterated->z ? NULL : prev);
}

inline FX_DListNode* FX_GenDListIterator::operator -- ()
/****************************************************************************
*
* Function:     FX_GenDListIterator::operator --
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

inline FX_DListNode* FX_GenDListIterator::operator -- (int)
/****************************************************************************
*
* Function:     FX_GenDListIterator::operator -- (int)
* Returns:      Pointer to node before decrementing
*
* Description:  Decrements the iterator by moving it to the next object
*               in the list. We return a pointer to the node pointed to
*               before the decrement takes place.
*
****************************************************************************/
{
    FX_DListNode    *prev = cursor;

    cursor = cursor->prev;
    return (prev == beingIterated->head ? NULL : prev);
}

inline void FX_GenDListIterator::restart()
/****************************************************************************
*
* Function:     FX_GenDListIterator::restart
*
* Description:  Restart the iterator at the beginning of the list.
*
****************************************************************************/
{
    cursor = beingIterated->head->next;
}

inline void FX_GenDListIterator::restartTail()
/****************************************************************************
*
* Function:     FX_GenDListIterator::restartTail
*
* Description:  Restart the iterator at the end of the list.
*
****************************************************************************/
{
    cursor = beingIterated->z->prev;
}

#endif  // __GM_DLIST_HPP

