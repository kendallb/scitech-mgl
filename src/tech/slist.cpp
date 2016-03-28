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
* Description:  Member functions for the list class, a class designed to
*               link a series of objects together into a singly linked
*               list. All items placed in the list MUST be derived from
*               the class TCListNode.
*
****************************************************************************/

#include "tcl/list.hpp"

/*--------------------------- Member functions ----------------------------*/

TCSimpleGenList::~TCSimpleGenList()
/****************************************************************************
*
* Function:     TCSimpleGenList::~TCSimpleGenList
*
* Description:  Destructor for the SimpleList class. All we do here is ask
*               the SimpleList to empty itself.
*
****************************************************************************/
{
    empty();
}

void TCSimpleGenList::empty(void)
/****************************************************************************
*
* Function:     TCSimpleGenList::empty
*
* Description:  Empties the SimpleList of all elements. We do this by
*               stepping through the SimpleList deleting all the elements
*               as we go.
*
****************************************************************************/
{
    TCListNode *temp;

    while (head != NULL) {
        temp = head;
        head = head->next;
        delete temp;
        }
}

ulong TCSimpleGenList::numberOfItems(void) const
/****************************************************************************
*
* Function:     TCSimpleGenList::numberOfItems
* Returns:      Number of items in the list.
*
* Description:  Since we do not maintain a count for the list, we simply
*               whiz through the list counting the number of items in it.
*
****************************************************************************/
{
    ulong   count = 0;

    for (TCSimpleGenListIterator i(*this); i; i++)
        count++;

    return count;
}
