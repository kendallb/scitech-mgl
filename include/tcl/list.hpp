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
*               singly linked list.
*
****************************************************************************/

#ifndef __TCL_LIST_HPP
#define __TCL_LIST_HPP

#ifndef __SCITECH_H
#include "scitech.h"
#endif

/*--------------------------- Class Definition ----------------------------*/

//---------------------------------------------------------------------------
// The TCListNode class is a simple class used to link the objects in the list
// together. To put anything useful into the list, you must derive the
// object placed into the list from TCListNode.
//---------------------------------------------------------------------------

class TCListNode {
protected:
    TCListNode  *next;

    friend class TCGenList;
    friend class TCGenListIterator;
    friend class TCSimpleGenList;
    friend class TCSimpleGenListIterator;
public:
            // Constructor to statisfy some compilers :-)
            TCListNode()    {};

            // Virtual destructor to delete a list node
    virtual ~TCListNode();
    };

//---------------------------------------------------------------------------
// The TCGenList class is designed to manipulate a list of TCListNode objects.
// In the simple form, TCListNode objects contain nothing special. To add
// an arbitrary class to the list, you must derive the class from TCListNode
// (either through single or multiple inheritance).
//---------------------------------------------------------------------------

typedef int (*_TCGenListCmp)(TCListNode*,TCListNode*);

class TCGenList {
protected:
    ulong       count;      // Number of objects in list
    TCListNode  *head;      // Pointer to first node in list
    TCListNode  *z;         // Pointer to last node in list
    TCListNode  hz[2];      // Space for head and z nodes

    static  _TCGenListCmp cmp;
    static  TCListNode  *_z;

            // Protected member to merge two lists together
    static  TCListNode* merge(TCListNode* a,TCListNode* b, TCListNode*& end);

public:
            // Constructor
            TCGenList();

            // Destructor
            ~TCGenList();

            // Method to examine the first node in the TCList
            TCListNode* peekHead() const;

            // Method to return the next node in the list
            TCListNode* next(TCListNode* node) const;

            // Method to add a node to the head of the list
            void addToHead(TCListNode* node);

            // Method to add a node after another node in the list
            void addAfter(TCListNode* node,TCListNode* after);

            // Method to detach a specified TCListNode from the list.
            TCListNode* removeNext(TCListNode* prev);

            // Method to detach the first node from the list.
            TCListNode* removeFromHead();

            // Sort the linked list of objects
            void sort(_TCGenListCmp cmp);

            // Empties the entire list by destroying all nodes
            void empty();

            // Returns the number of items in the list
            ulong numberOfItems() const { return count; };

            // Returns true if the list is empty
            ibool isEmpty() const   { return count == 0; };

private:
    friend class TCGenListIterator;
    };

//---------------------------------------------------------------------------
// The TCSimpleGenList class is designed to manipulate a list of TCListNode
// objects. The TCSimpleGenList class only maintains a single pointer to the
// head of the list rather than using the dummy node system, so is useful
// when you need to maintain an array of linked lists, and the list type
// itself needs to be as small as possible for memory efficiency.
//---------------------------------------------------------------------------

class TCSimpleGenList {
protected:
    TCListNode  *head;      // Pointer to first node in list

public:
            // Constructor
            TCSimpleGenList()   { head = NULL; };

            // Destructor
            ~TCSimpleGenList();

            // Method to examine the first node in the TCList
            TCListNode* peekHead() const
                { return head; };

            // Method to return the next node in the list
            TCListNode* next(TCListNode* node) const
                { return node->next; };

            // Method to add a node to the head of the list
            void addToHead(TCListNode* node);

            // Method to add a node after another node in the list
            void addAfter(TCListNode* node,TCListNode* after);

            // Method to detach a specified TCListNode from the list.
            TCListNode* removeNext(TCListNode* prev);

            // Method to detach the first node from the list.
            TCListNode* removeFromHead();

            // Empties the entire list by destroying all nodes
            void empty();

            // Returns the number of items in the list
            ulong numberOfItems() const;

            // Returns true if the list is empty
            ibool isEmpty() const
                { return head == NULL; };

private:
    friend class TCSimpleGenListIterator;
    };

//---------------------------------------------------------------------------
// The TCGenListIterator is the class of iterator that is used to step through
// the elements in the list.
//---------------------------------------------------------------------------

class TCGenListIterator {
protected:
    TCListNode      *cursor;
    const TCGenList *beingIterated;

public:
            // Constructor
            TCGenListIterator();

            // Constructor given a list reference
            TCGenListIterator(const TCGenList& l);

            // Intialise a list iterator from a list
            void operator = (const TCGenList& l);

            // assignment operator between two listIterators
            void operator = (const TCGenListIterator& i);

            // Overloaded cast to an integer
            operator int ();

            // Convert the iterator to the corresponding node
            TCListNode* node();

            // Pre-increment operator for the iterator
            TCListNode* operator ++ ();

            // Post-increment operator for the iterator
            TCListNode* operator ++ (int);

            // Method to restart the iterator
            void restart();
    };

//---------------------------------------------------------------------------
// The TCSimpleGenListIterator is the class of iterator that is used to step
// through the elements in the list.
//---------------------------------------------------------------------------

class TCSimpleGenListIterator {
protected:
    TCListNode              *cursor;
    const TCSimpleGenList   *beingIterated;

public:
            // Constructor
            TCSimpleGenListIterator()
                { cursor = NULL; beingIterated = NULL; };

            // Constructor given a list reference
            TCSimpleGenListIterator(const TCSimpleGenList& l)
                { beingIterated = &l; cursor = l.head; };

            // Intialise a list iterator from a list
            void operator = (const TCSimpleGenList& l)
                { beingIterated = &l; cursor = l.head; };

            // assignment operator between two listIterators
            void operator = (const TCSimpleGenListIterator& i)
                { beingIterated = i.beingIterated; cursor = i.cursor; };

            // Overloaded cast to an integer
            operator int ()
                { return cursor != NULL; };

            // Convert the iterator to the corresponding node
            TCListNode* node()
                { return cursor; };

            // Pre-increment operator for the iterator
            TCListNode* operator ++ ();

            // Post-increment operator for the iterator
            TCListNode* operator ++ (int);

            // Method to restart the iterator
            void restart()
                { cursor = beingIterated->head; };
    };

//---------------------------------------------------------------------------
// Set of template wrapper classes for declaring Type Safe linked lists.
// Note that the elements of the linked list must still be derived from
// TCListNode.
//---------------------------------------------------------------------------

template <class T> class TCList : public TCGenList {
public:
            T* peekHead() const
                { return (T*)TCGenList::peekHead(); };
            T* next(T* node) const
                { return (T*)TCGenList::next(node); };
            T* removeNext(T* prev)
                { return (T*)TCGenList::removeNext(prev); };
            T* removeFromHead()
                { return (T*)TCGenList::removeFromHead(); };
            void sort(int (*cmp)(T*,T*))
                { TCGenList::sort((_TCGenListCmp)cmp); };
    };

template <class T> class TCListIterator : public TCGenListIterator {
public:
            TCListIterator()
                : TCGenListIterator() {};
            TCListIterator(const TCList<T>& l)
                : TCGenListIterator(l) {};
            void operator = (const TCList<T>& l)
                { TCGenListIterator::operator=(l); };
            void operator = (const TCListIterator<T>& i)
                { TCGenListIterator::operator=(i); };
            T* node()
                { return (T*)TCGenListIterator::node(); };
            T* operator ++ ()
                { return (T*)TCGenListIterator::operator++(); };
            T* operator ++ (int)
                { return (T*)TCGenListIterator::operator++(1); };
    };

template <class T> class TCSimpleList : public TCSimpleGenList {
public:
            T* peekHead() const
                { return (T*)TCSimpleGenList::peekHead(); };
            T* next(T* node) const
                { return (T*)TCSimpleGenList::next(node); };
            T* removeNext(T* prev)
                { return (T*)TCSimpleGenList::removeNext(prev); };
            T* removeFromHead()
                { return (T*)TCSimpleGenList::removeFromHead(); };
    };

template <class T> class TCSimpleListIterator : public TCSimpleGenListIterator {
public:
            TCSimpleListIterator()
                : TCSimpleGenListIterator() {};
            TCSimpleListIterator(const TCSimpleList<T>& l)
                : TCSimpleGenListIterator(l) {};
            void operator = (const TCSimpleList<T>& l)
                { TCSimpleGenListIterator::operator=(l); };
            void operator = (const TCSimpleListIterator<T>& i)
                { TCSimpleGenListIterator::operator=(i); };
            T* node()
                { return (T*)TCSimpleGenListIterator::node(); };
            T* operator ++ ()
                { return (T*)TCSimpleGenListIterator::operator++(); };
            T* operator ++ (int)
                { return (T*)TCSimpleGenListIterator::operator++(1); };
    };

/*------------------------ Inline member functions ------------------------*/

inline TCListNode* TCGenList::peekHead() const
/****************************************************************************
*
* Function:     TCGenList::peekHead
* Parameters:
* Returns:      Returns a pointer to the head node on the list, or NULL if
*               the list is empty.
*
****************************************************************************/
{
    return (head->next == z ? NULL : head->next);
}

inline TCListNode* TCGenList::next(TCListNode *node) const
/****************************************************************************
*
* Function:     TCGenList::next
* Parameters:   node    - Node to obtain next from
* Returns:      Pointer to the next node in the list, NULL if none.
*
****************************************************************************/
{
    return (node->next == z ? NULL : node->next);
}

inline void TCGenList::addAfter(TCListNode* node,TCListNode* after)
/****************************************************************************
*
* Function:     TCGenList::addAfter
* Parameters:   node    - Node to attach new node after in list
*               after   - New node to attach to list
*
* Description:  Attaches a new node after a specified node in the list.
*               The list must contain at least one node, and after may
*               be the tail node of the list.
*
****************************************************************************/
{
    node->next = after->next;
    after->next = node;
    count++;
}

inline void TCGenList::addToHead(TCListNode* node)
/****************************************************************************
*
* Function:     TCGenList::addToHead
* Parameters:   node    - Node to add to list
*
* Description:  Attaches the node to the head of the list, maintaining the
*               head and tail pointers.
*
****************************************************************************/
{
    addAfter(node,head);
}

inline TCListNode* TCGenList::removeNext(TCListNode* prev)
/****************************************************************************
*
* Function:     TCGenList::removeNext
* Parameters:   node    - Pointer to node remove from the list
*               prev    - Pointer to the previous node in the list
* Returns:      Node removed from list, or NULL if prev is the last node in
*               the list.
*
* Description:  Attempts to remove the specified node from the list. 'prev'
*               should point to the previous node in the list.
*
****************************************************************************/
{
    TCListNode  *node;

    if ((node = prev->next) != z) {
        prev->next = prev->next->next;
        count--;
        return node;
        }
    else
        return NULL;
}

inline TCListNode* TCGenList::removeFromHead()
/****************************************************************************
*
* Function:     TCGenList::removeFromHead
* Returns:      Pointer to the node removed from the head of the list,
*               or NULL if the list is empty.
*
****************************************************************************/
{
    return removeNext(head);
}

inline TCGenListIterator::TCGenListIterator()
/****************************************************************************
*
* Function:     TCGenListIterator::TCGenListIterator
*
* Description:  Default constructor for a list iterator.
*
****************************************************************************/
{
    cursor = NULL;
    beingIterated = NULL;
}

inline TCGenListIterator::TCGenListIterator(const TCGenList& l)
/****************************************************************************
*
* Function:     TCGenListIterator::TCGenListIterator
* Parameters:   l   - TCList to construct iterator from
*
* Description:  Constructor for a TCListIterator given a reference to a list
*               to iterate.
*
****************************************************************************/
{
    beingIterated = (TCGenList*)&l;
    cursor = l.head->next;
}

inline void TCGenListIterator::operator = (const TCGenList& l)
/****************************************************************************
*
* Function:     TCGenListIterator::operator =
* Parameters:   l   - TCList to assign to iterator
*
* Description:  Assignment operator for a TCListIterator given a reference to
*               a list to iterate.
*
****************************************************************************/
{
    beingIterated = &l;
    cursor = l.head->next;
}

inline void TCGenListIterator::operator = (const TCGenListIterator& i)
/****************************************************************************
*
* Function:     TCGenListIterator::operator =
* Parameters:   i   - Iterator to assign from
*
* Description:  Assignment operator for a TCListIterator given a reference to
*               another TCListIterator.
*
****************************************************************************/
{
    beingIterated = i.beingIterated;
    cursor = i.cursor;
}

inline TCGenListIterator::operator int()
/****************************************************************************
*
* Function:     TCGenListIterator::operator int
*
* Description:  Overloaded cast to integer for the list iterator. Evaluates
*               to 0 when the end of the list is reached.
*
****************************************************************************/
{
    return (cursor != beingIterated->z);
}

inline TCListNode* TCGenListIterator::node()
/****************************************************************************
*
* Function:     TCGenListIterator::node
* Returns:      Returns a reference to the node in the list.
*
****************************************************************************/
{
    return (cursor == beingIterated->z ? NULL : cursor);
}

inline TCListNode* TCGenListIterator::operator ++ ()
/****************************************************************************
*
* Function:     TCGenListIterator::operator ++
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

inline TCListNode* TCGenListIterator::operator ++ (int)
/****************************************************************************
*
* Function:     TCGenListIterator::operator ++ (int)
* Returns:      Pointer to node before incrementing
*
* Description:  Increments the iterator by moving it to the next object
*               in the list. We return a pointer to the node pointed to
*               before the increment takes place.
*
****************************************************************************/
{
    TCListNode  *prev = cursor;

    cursor = cursor->next;
    return (prev == beingIterated->z ? NULL : prev);
}

inline void TCGenListIterator::restart()
/****************************************************************************
*
* Function:     TCGenListIterator::restart
*
* Description:  Restart the iterator at the beginning of the list.
*
****************************************************************************/
{
    cursor = beingIterated->head->next;
}

inline void TCSimpleGenList::addToHead(TCListNode* node)
/****************************************************************************
*
* Function:     TCSimpleGenList::addToHead
* Parameters:   node    - Node to add to TCSimpleList
*
* Description:  Attaches the node to the head of the TCSimpleList.
*
****************************************************************************/
{
    node->next = head;
    head = node;
}

inline void TCSimpleGenList::addAfter(TCListNode* node,TCListNode* after)
/****************************************************************************
*
* Function:     TCSimpleGenList::addAfter
* Parameters:   node    - Node to attach new node after in TCSimpleList
*               after   - New node to attach to TCSimpleList
*
* Description:  Attaches a new node after a specified node in the TCSimpleList.
*               The TCSimpleList must contain at least one node, and after may
*               be the tail node of the TCSimpleList.
*
****************************************************************************/
{
    node->next = after->next;
    after->next = node;
}

inline TCListNode* TCSimpleGenList::removeNext(TCListNode* prev)
/****************************************************************************
*
* Function:     TCSimpleGenList::removeNext
* Parameters:   node    - Pointer to node remove from the TCSimpleList
*               prev    - Pointer to the previous node in the TCSimpleList
* Returns:      Node removed from TCSimpleList, or NULL if prev is the last
*               node in the TCSimpleList.
*
* Description:  Attempts to remove the specified node from the TCSimpleList.
*               'prev' should point to the previous node in the TCSimpleList.
*
****************************************************************************/
{
    TCListNode  *node;

    if ((node = prev->next) != NULL) {
        prev->next = prev->next->next;
        return node;
        }
    else
        return NULL;
}

inline TCListNode* TCSimpleGenList::removeFromHead()
/****************************************************************************
*
* Function:     TCSimpleGenList::removeFromHead
* Returns:      Pointer to the node removed from the head of the TCSimpleList,
*               or NULL if the TCSimpleList is empty.
*
****************************************************************************/
{
    TCListNode  *node = head;

    if (head)
        head = head->next;
    return node;
}

inline TCListNode* TCSimpleGenListIterator::operator ++ ()
/****************************************************************************
*
* Function:     TCSimpleGenListIterator::operator ++
* Returns:      Pointer to node after incrementing
*
* Description:  Increments the iterator by moving it to the next object
*               in the TCSimpleList. We return a pointer to the node pointed to
*               after the increment takes place.
*
****************************************************************************/
{
    return cursor ? (cursor = cursor->next) : NULL;
}

inline TCListNode* TCSimpleGenListIterator::operator ++ (int)
/****************************************************************************
*
* Function:     TCSimpleGenListIterator::operator ++ (int)
* Returns:      Pointer to node before incrementing
*
* Description:  Increments the iterator by moving it to the next object
*               in the TCSimpleList. We return a pointer to the node pointed to
*               before the increment takes place.
*
****************************************************************************/
{
    TCListNode  *prev = cursor;

    if (cursor)
        cursor = cursor->next;
    return prev;
}

#endif  // __TCL_LIST_HPP
