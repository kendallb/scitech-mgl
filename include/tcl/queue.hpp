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
* Description:  Header file for a queue class made of objects linked in
*               a singly linked list.
*
****************************************************************************/

#ifndef __TCL_QUEUE_HPP
#define __TCL_QUEUE_HPP

#ifndef __TCL_LIST_HPP
#include "tcl/list.hpp"
#endif

/*--------------------------- Class Definition ----------------------------*/

//---------------------------------------------------------------------------
// The TCQueue class is an abstraction of the list class to provide operations
// normally performed on a queue data structure. Internally it is just a
// linked list, so anything placed on the list MUST be derived from
// TCListNode.
//---------------------------------------------------------------------------

class TCGenQueue : private TCGenList {
protected:
    TCListNode  *tail;      // Pointer to tail node of queue

public:
            // Constructor
            TCGenQueue() : TCGenList()  { tail = head; };

            // Method to examine the next item waiting in the queue
            TCListNode* peek() const    { return peekHead(); };

            // Method to add an item to the queue
            void put(TCListNode* node);

            // Method to remove the next item waiting in the queue
            TCListNode* get();

            // Empties the queue by destroying all nodes
            void empty()
                { TCGenList::empty(); tail = head; };

            // Returns the number of items in the list
            ulong numberOfItems() const
                { return TCGenList::numberOfItems(); };

            // Returns true if the list is empty
            ibool isEmpty() const
                { return TCGenList::isEmpty(); };
    };

//---------------------------------------------------------------------------
// Template wrapper class for declaring Type Safe linked queues.
//---------------------------------------------------------------------------

template <class T> class TCQueue : public TCGenQueue {
public:
            T* peek() const
                { return (T*)TCGenQueue::peek(); };
            T* get()
                { return (T*)TCGenQueue::get(); };
    };

/*------------------------ Inline member functions ------------------------*/

inline void TCGenQueue::put(TCListNode* node)
/****************************************************************************
*
* Function:     TCGenQueue::put
* Parameters:   node    - Node to add to the queue
*
* Description:  Adds the node to the queue by tacking it onto the end of
*               the list.
*
****************************************************************************/
{
    addAfter(node,tail);        // Add to the tail of the list
    tail = node;                // Maintain tail pointer
}

inline TCListNode* TCGenQueue::get()
/****************************************************************************
*
* Function:     TCGenQueue::get
* Returns:      Pointer to the item removed from the queue.
*
****************************************************************************/
{
    if (count == 1)
        tail = head;            // Maintain tail pointer
    return removeFromHead();
}

#endif  // __TCL_QUEUE_HPP
