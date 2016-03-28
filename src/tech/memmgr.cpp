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
* Description:  Implementation for the Memory management classes.
*
****************************************************************************/

#include <stdlib.h>
#include "tcl/memmgr.hpp"

/*---------------------------- Implementation -----------------------------*/

ibool TCMemStack::allocatePoolBlock()
/****************************************************************************
*
* Function:     TCMemStack::allocatePoolBlock
* Returns:      True if block was allocated, false on failure
*
* Description:  Allocates a new large pool block of the specified size,
*               and links it onto the list of allocated pool blocks.
*
****************************************************************************/
{
    BlockList *temp = (BlockList*)malloc(sizeof(BlockList) + blockSize);
    if (temp) {
        temp->next = curBlock;
        curBlock = temp;
        blockCount++;
        return true;
        }
    else
        return false;
}

void TCMemStack::freeFrom(uint block)
/****************************************************************************
*
* Function:     TCMemStack::freeFrom
* Parameters:   block   - Pool block number to start freeing from
*
* Description:  Free's all the pool blocks from 'block' to the current
*               maximum.
*
****************************************************************************/
{
    CHECK(block <= blockCount);
    while (block < blockCount) {
        BlockList *temp = curBlock;
        curBlock = temp->next;
        free(temp);
        blockCount--;
        }
}

void *TCMemStack::allocate(size_t size)
/****************************************************************************
*
* Function:     TCMemStack::allocate
* Parameters:   size    - Size of memory block to allocate
* Returns:      Pointer to the allocated block, or NULL if allocation failed
*
* Description:  Attempts to allocate space for the requested block from
*               the current memory pool. If this fails, we attempt to
*               allocate another memory pool and allocate the memory
*               from there.
*
****************************************************************************/
{
    CHECK(size != 0);
    if (size > blockSize - curLoc) {
        if (allocatePoolBlock())
            curLoc = 0;
        else
            return NULL;
        }
    void *temp = poolStart() + curLoc;
    curLoc += size;
    return temp;
}
