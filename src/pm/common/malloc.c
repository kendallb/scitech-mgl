/****************************************************************************
*
*                   SciTech OS Portability Manager Library
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
* Language:     ANSI C
* Environment:  Any
*
* Description:  Module for implementing the PM library overrideable memory
*               allocator functions.
*
****************************************************************************/

#include "pmapi.h"

/*--------------------------- Global variables ----------------------------*/

/* {secret} */
void * (*__PM_malloc)(size_t size)              = NULL;
/* {secret} */
void * (*__PM_calloc)(size_t nelem,size_t size) = NULL;
/* {secret} */
void * (*__PM_realloc)(void *ptr,size_t size)   = NULL;
/* {secret} */
void (*__PM_free)(void *p)                      = NULL;

/*----------------------------- Implementation ----------------------------*/

/****************************************************************************
DESCRIPTION:
Use local memory allocation routines.

HEADER:
pmapi.h

PARAMETERS:
malloc  - Pointer to new malloc routine to use
calloc  - Pointer to new caalloc routine to use
realloc - Pointer to new realloc routine to use
free    - Pointer to new free routine to use

REMARKS:
Tells the PM library to use a set of user specified memory allocation
routines instead of using the normal malloc/calloc/realloc/free standard
C library functions. This is useful if you wish to use a third party
debugging malloc library or perhaps a set of faster memory allocation
functions with the PM library, or any apps that use the PM library (such as
the MGL). Once you have registered your memory allocation routines, all
calls to PM_malloc, PM_calloc, PM_realloc and PM_free will be revectored to
your local memory allocation routines.

NOTE: This function should be called right at the start of your application,
      before you initialise any other components or libraries.

NOTE: Code compiled into Binary Portable DLL's and Drivers automatically
      end up calling these functions via the BPD C runtime library.

SEE ALSO:
PM_malloc, PM_calloc, PM_realloc, PM_free
****************************************************************************/
void PMAPI PM_useLocalMalloc(
    void * (*malloc)(size_t size),
    void * (*calloc)(size_t nelem,size_t size),
    void * (*realloc)(void *ptr,size_t size),
    void (*free)(void *p))
{
    __PM_malloc = malloc;
    __PM_calloc = calloc;
    __PM_realloc = realloc;
    __PM_free = free;
}

/****************************************************************************
DESCRIPTION:
Allocate a block of memory.

HEADER:
pmapi.h

PARAMETERS:
size    - Size of block to allocate in bytes

RETURNS:
Pointer to allocated block, or NULL if out of memory.

REMARKS:
Allocates a block of memory of length size. If you have changed the memory
allocation routines with the PM_useLocalMalloc function, then calls to this
function will actually make calls to the local memory allocation routines
that you have registered.

SEE ALSO:
PM_calloc, PM_realloc, PM_free, PM_useLocalMalloc
****************************************************************************/
void * PMAPI PM_malloc(
    size_t size)
{
    if (__PM_malloc)
        return __PM_malloc(size);
    return malloc(size);
}

/****************************************************************************
DESCRIPTION:
Allocate and clear a large memory block.

HEADER:
pmapi.h

PARAMETERS:
nelem   - number of contiguous size-byte units to allocate
size    - size of unit in bytes

RETURNS:
Pointer to allocated memory if successful, NULL if out of memory.

REMARKS:
Allocates a block of memory of length (size * nelem), and clears the
allocated area with zeros (0). If you have changed the memory allocation
routines with the PM_useLocalMalloc function, then calls to this function
will actually make calls to the local memory allocation routines that you
have registered.

SEE ALSO:
PM_malloc, PM_realloc, PM_free, PM_useLocalMalloc
****************************************************************************/
void * PMAPI PM_calloc(
    size_t nelem,
    size_t size)
{
    if (__PM_calloc)
        return __PM_calloc(nelem,size);
    return calloc(nelem,size);
}

/****************************************************************************
DESCRIPTION:
Re-allocate a block of memory

HEADER:
pmapi.h

PARAMETERS:
ptr     - Pointer to block to resize
size    - size of unit in bytes

RETURNS:
Pointer to allocated memory if successful, NULL if out of memory.

REMARKS:
This function reallocates a block of memory that has been previously been
allocated to the new of size. The new size may be smaller or larger than
the original block of memory. If you have changed the memory allocation
routines with the PM_useLocalMalloc function, then calls to this function
will actually make calls to the local memory allocation routines that you
have registered.

SEE ALSO:
PM_malloc, PM_calloc, PM_free, PM_useLocalMalloc
****************************************************************************/
void * PMAPI PM_realloc(
    void *ptr,
    size_t size)
{
    if (__PM_realloc)
        return __PM_realloc(ptr,size);
    return realloc(ptr,size);
}

/****************************************************************************
DESCRIPTION:
Frees a block of memory.

HEADER:
pmapi.h

PARAMETERS:
p   - Pointer to memory block to free

REMARKS:
Frees a block of memory previously allocated with either PM_malloc,
PM_calloc or PM_realloc.

SEE ALSO:
PM_malloc, PM_calloc, PM_realloc, PM_useLocalMalloc
****************************************************************************/
void PMAPI PM_free(
    void *p)
{
    if (__PM_free)
        __PM_free(p);
    else
        free(p);
}

