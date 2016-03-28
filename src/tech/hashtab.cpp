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
* Description:  Member functions for the HashTable class, a class designed
*               to map a bunch of unsorted objects into a hash table for
*               fast and efficient access.
*
****************************************************************************/

#include "tcl/hashtab.hpp"
#include "tcl/error.hpp"

/*--------------------------- Member functions ----------------------------*/

TCGenHashTable::TCGenHashTable(uint tabsize)
/****************************************************************************
*
* Function:     TCGenHashTable::TCGenHashTable
* Parameters:   tabsize - Size of the hash table to construct
*
* Description:  Creates a new hash table of the given size. Note that
*               for best performance, tabsize should be a prime number.
*
****************************************************************************/
{
    if (tabsize == 0)
        size = 127;         // Default size
    else
        size = tabsize;

    // Allocate memory for the table.

    table = new TCHashTableNode*[size];
    for (uint i = 0; i < size; i++)
        table[i] = NULL;
    count = 0;
}

TCGenHashTable::~TCGenHashTable()
/****************************************************************************
*
* Function:     TCGenHashTable::~TCGenHashTable
*
* Description:  Destructor for hash table. We simply empty the hash table
*               of all symbols and then delete any memory used.
*
****************************************************************************/
{
    empty();
    delete [] table;
}

void TCGenHashTable::empty(void)
/****************************************************************************
*
* Function:     TCGenHashTable::empty
*
* Description:  Empties the hash table of all symbols by deleting all the
*               symbols stored in the table.
*
****************************************************************************/
{
    TCHashTableNode *node,*p;

    for (uint i = 0; i < size; i++) {
        node = table[i];
        while (node) {
            p = node;
            node = node->next;
            delete p;
            }
        }
}

void TCGenHashTable::add(TCHashTableNode* node)
/****************************************************************************
*
* Function:     TCGenHashTable::add
* Parameters:   node    - Node to add to the table
*
* Description:  Adds a symbol to the hash table. The new symbol is linked
*               onto the head of the chain at it's particular hash location.
*
****************************************************************************/
{
    TCHashTableNode **p;
    TCHashTableNode *temp;

    p = &(table[node->hash() % size]);
    temp = *p;
    *p = node;
    node->prev = p;
    node->next = temp;
    if (temp)
        temp->prev = &node->next;
    count++;
}

TCHashTableNode* TCGenHashTable::remove(TCHashTableNode* node)
/****************************************************************************
*
* Function:     TCGenHashTable::remove
* Parameters:   node    - Node to remove from the table
*
* Description:  Removes a symbol from the hash table. "node" is a pointer
*               from a previous find() call.
*
****************************************************************************/
{
    if ((*(node->prev) = node->next) != NULL)
        node->next->prev = node->prev;
    count--;
    return node;
}

TCHashTableNode* TCGenHashTable::find(TCHashTableNode* key) const
/****************************************************************************
*
* Function:     TCGenHashTable::find
* Parameters:   key     - Key of node to find
* Returns:      Pointer to the node if found, NULL if none exist.
*
* Description:  Returns a pointer to the hash table node having a
*               particular key, or NULL if the node isn't in the table.
*
****************************************************************************/
{
    TCHashTableNode* p = table[key->hash() % size];

    while (p) {
        if (*p == *key)
            break;
        p = p->next;
        }

    return p;
}

TCHashTableNode* TCGenHashTable::findCached(TCHashTableNode* key)
/****************************************************************************
*
* Function:     TCGenHashTable::findCached
* Parameters:   key     - Key of node to find
* Returns:      Pointer to the node if found, NULL if none exist.
*
* Description:  Returns a pointer to the hash table node having a
*               particular key, or NULL if the node isn't in the table.
*
*               This routine caches the value that was found, by moving it
*               to the start of the chain that the item was found on.
*
****************************************************************************/
{
    TCHashTableNode *q,*temp;
    TCHashTableNode **p = &(table[key->hash() % size]);

    q = *p;

    while (q) {
        if (*q == *key)
            break;
        q = q->next;
        }

    // If we have found the key, and it is not at the start of the chain,
    // then cache the value by moving to the start of the chain.

    if (q != NULL && q != *p) {

        // Remove it from the chain

        if ((*(q->prev) = q->next) != NULL)
            q->next->prev = q->prev;

        // Add it to the front of the chain (at least one left in chain)

        temp = *p;
        *p = q;
        q->prev = p;
        q->next = temp;
        temp->prev = &q->next;
        }

    return q;
}

TCHashTableNode* TCGenHashTable::next(TCHashTableNode* last) const
/****************************************************************************
*
* Function:     TCGenHashTable::next
* Parameters:   last    - Last node found with find()
* Returns:      Pointer to the next node with the same key as last, or
*               NULL is none exist.
*
* Description:  Returns a pointer to the next node in the current chain that
*               has the same key as the last node found (or NULL if there
*               is no such node). "last" is a pointer returned from a
*               previous find() or next() call.
*
****************************************************************************/
{
    TCHashTableNode* p = last;

    while (last) {
        last = last->next;
        if (*p == *last)
            break;
        }
    return last;
}

ostream& operator << (ostream& o,TCGenHashTable& h)
/****************************************************************************
*
* Function:     operator <<
* Parameters:   o   - Stream to send output to
*               h   - Hash Table to display
* Returns:      Stream to send output to
*
* Description:  Dumps the contents of the hash table to the stream 'o' by
*               dumping each individual node of the table one by one.
*
****************************************************************************/
{
    TCHashTableNode *p;

    cout << "Hash Table (" << h.count << " items):\n";
    for (uint i = 0; i < h.size; i++) {
        p = h.table[i];
        while (p) {
            o << *p;
            p = p->next;
            }
        }
    return o;
}

// Virtual destructor for TCHashTableNode's - does nothing.

TCHashTableNode::~TCHashTableNode()
{
}
