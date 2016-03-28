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
* Description:  Header file for a stack class made of objects linked in
*               a singly linked list.
*
****************************************************************************/

#ifndef __TCL_STACK_HPP
#define __TCL_STACK_HPP

#ifndef __TCL_LIST_HPP
#include "tcl/list.hpp"
#endif

/*--------------------------- Class Definition ----------------------------*/

//---------------------------------------------------------------------------
// The stack class is an abstraction of the list class that provides the
// operations normally performed on a stack. Internally it is simply a
// singly linked list, so all items placed on the stack MUST be derived
// from TCListNode.
//---------------------------------------------------------------------------

class TCGenStack : private TCGenList {
public:
            // Method to examine the top item on the stack
            TCListNode* top() const { return peekHead(); };

            // Method to push an item onto the stack
            void push(TCListNode* node) { addToHead(node); };

            // Method to pop an item from the stack
            TCListNode* pop()           { return removeFromHead(); };

            // Empties the entire stack by destroying all nodes
            void empty()                { TCGenList::empty(); };

            // Returns the number of items on the stack
            ulong numberOfItems() const
                { return TCGenList::numberOfItems(); };

            // Returns true if the stack is empty
            ibool isEmpty() const
                { return TCGenList::isEmpty(); };
    };

//---------------------------------------------------------------------------
// Template wrapper class for declaring Type Safe linked stacks.
//---------------------------------------------------------------------------

template <class T> class TCStack : public TCGenStack {
public:
            T* top() const
                { return (T*)TCGenStack::top(); };
            T* pop()
                { return (T*)TCGenStack::pop(); };
    };

#endif  // __TCL_STACK_HPP
