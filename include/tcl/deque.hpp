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
* Description:  Header file for a double ended queue class made of objects
*               linked in a doubly linked list.
*
****************************************************************************/

#ifndef __TCL_DEQUE_HPP
#define __TCL_DEQUE_HPP

#ifndef __TCL_DLIST_HPP
#include "tcl/dlist.hpp"
#endif

/*--------------------------- Class Definition ----------------------------*/

//---------------------------------------------------------------------------
// The Deque class is an abstraction of the dlist class to provide operations
// normally performed on a double ended queue data structure. Internally it
// is just a doubly linked list, so anything placed on the list MUST be
// derived from TCDListNode. By default items are added to the left of the
// deque and removed from the right of the deque.
//---------------------------------------------------------------------------

class TCGenDeque : private TCGenDList {
public:
            // Methods to examine the left or right items in the deque
            TCDListNode* peek()                 { return peekHead(); };
            TCDListNode* peekLeft()             { return peekHead(); };
            TCDListNode* peekRight()            { return peekTail(); };

            // Methods to add an item to the deque
            void put(TCDListNode* node)         { addToTail(node); };
            void putLeft(TCDListNode* node)     { addToHead(node); };
            void putRight(TCDListNode* node)    { addToTail(node); };

            // Methods to remove items from the deque
            TCDListNode* get()      { return removeFromHead(); };
            TCDListNode* getLeft()  { return removeFromHead(); };
            TCDListNode* getRight() { return removeFromTail(); };

            // Empties the entire list by destroying all nodes
            void empty()    { TCGenDList::empty(); };

            // Returns the number of items in the list
            ulong numberOfItems()   { return TCGenDList::numberOfItems(); };

            // Returns true if the list is empty
            ibool isEmpty() { return TCGenDList::isEmpty(); };
    };

//---------------------------------------------------------------------------
// Template wrapper class for declaring Type Safe linked queues.
//---------------------------------------------------------------------------

template <class T> class TCDeque : public TCGenDeque {
public:
            T* peek()
                { return (T*)TCGenDeque::peek(); };
            T* peekLeft()
                { return (T*)TCGenDeque::peekLeft(); };
            T* peekRight()
                { return (T*)TCGenDeque::peekRight(); };
            T* get()
                { return (T*)TCGenDeque::get(); };
            T* getLeft()
                { return (T*)TCGenDeque::getLeft(); };
            T* getRight()
                { return (T*)TCGenDeque::getRight(); };
    };

#endif  // __TCL_DEQUE_HPP
