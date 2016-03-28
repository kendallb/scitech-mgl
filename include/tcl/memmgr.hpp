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
* Description:  Header file for specialised memory management routines.
*               Two systems are provided. The TCMemStack provides fast
*               and efficient mark and release style memory management
*               (note that destructors are _not_ called by this type of
*               memory management). The TCMemBlocks is a specialisation of
*               the TCMemStack class that maintains a free list of all
*               freed blocks for quick reallocation, but requires all
*               objects allocated to be of the same size.
*
****************************************************************************/

#ifndef __TCL_MEMMGR_HPP
#define __TCL_MEMMGR_HPP

#ifndef __TCL_TECHLIB_HPP
#include "tcl/techlib.hpp"
#endif

#ifndef __STDDEF_H
#include <stddef.h>
#endif

/*--------------------------- Class Definitions ---------------------------*/

//---------------------------------------------------------------------------
// The following class manages a stack of arbitrary sized blocks, allocating
// memory from a list of large sized blocks for efficiency. Fast mark
// and release memory management can be performed using the TCMemMarker class.
//
// Note that blocks can only be allocated, not freed using the TCMemStack. The
// allocated memory will be freed when a TCMemMarker object goes out of
// scope.
//
// Note that when the memory is freed by the TCMemMarker, no destructors will
// be called, so use with care.
//---------------------------------------------------------------------------

class TCMemStack {
protected:
    struct BlockList {
        BlockList   *next;          // Pointer to next block in list
        }       *curBlock;          // List of memory pool blocks
    size_t      blockSize;          // Size of memory pool blocks
    size_t      blockCount;         // Number of memory pool blocks allocated
    size_t      curLoc;             // Location within current pool block

            // Return a char * to start of current memory pool
            char *poolStart()   { return (char*)(curBlock+1); };

            // Allocate a new memory pool block
            ibool allocatePoolBlock();

            // Free all pool blocks from 'block' to current maximum
            void freeFrom(uint block);

public:
            // Constructor given size of pool to allocate
            TCMemStack(size_t size = 8192)
                : curBlock(NULL), blockSize(size), blockCount(0),
                  curLoc(size)
                { CHECK(size != 0); };

            // Destructor - free's all memory
            ~TCMemStack()   { freeFrom(0); };

            // Allocate a block from the pool of specified size
            void *allocate(size_t size);

private:
    friend  class TCMemMarker;
    };

//---------------------------------------------------------------------------
// The following class is used to mark the current position in a TCMemStack.
// When the TCMemMarker goes out of scope or is deleted, all memory allocated
// after the TCMemMarker was constructed will be automatically freed.
//---------------------------------------------------------------------------

class TCMemMarker {
protected:
    uint        block;          // Number of currently allocated pool block
    size_t      curLoc;         // Location within current pool block
    TCMemStack& mem;            // TCMemStack begin marked

public:
            // Constructor
            TCMemMarker(TCMemStack& mem)
                : block(mem.blockCount), curLoc(mem.curLoc), mem(mem) {};

            // Destructor
            ~TCMemMarker()
            {   CHECK(block < mem.blockCount ||
                    (block == mem.blockCount && curLoc <= mem.curLoc));
                mem.freeFrom(block);
                mem.curLoc = curLoc;
            };
    };

//---------------------------------------------------------------------------
// The following class manages a number of fixed sized blocks very
// efficiently using a simple free list mechanism. Classes can use this
// class internally to overload the default memory management for speed
// (be careful with derived classes however!).
//---------------------------------------------------------------------------

class TCMemBlocks : public TCMemStack {
protected:
    void    *freeList;              // Head of the free list
    uint    size;                   // Size of allocated blocks

public:
            // Constructor given the size and number of blocks to allocate
            TCMemBlocks(uint size,uint count = 100)
                : TCMemStack(size * count), freeList(NULL),
                  size(MAX(size,sizeof(void*)))
            {   CHECK(size >= sizeof(void*));
                CHECK(size != 0 && count != 0); };

            // Method to allocate a new block
            void *allocate(size_t sz)
            {
                sz = sz;
                CHECK(size == sz);
                if (freeList == NULL)
                    return TCMemStack::allocate(size);
                else {
                    void *temp = freeList;
                    freeList = *(void **)temp;
                    return temp;
                    }
            };


            // Method to free a used block
            void free(void *p)
            {   *(void **)p = freeList;
                freeList = p;
            };

            // Method to free all used blocks at once (including free list)
            void freeAll()
            {   freeFrom(0);
                curLoc = blockSize;
                freeList = NULL;
            };
    };

/*--------------------------- Inline Functions ----------------------------*/

//---------------------------------------------------------------------------
// Overloaded operator new to allocate memory from a memory stack
// (rather than having to call TCMemStack::allocate()) directly
//---------------------------------------------------------------------------

inline void *operator new(size_t size,TCMemStack& m)
{
    return m.allocate(size);
}

#endif  // __TCL_MEMMGR_HPP
