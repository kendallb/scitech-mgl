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
* Language:     ANSI C
* Environment:  Any
*
* Description:  Header file for linked list manipulation code for the MGL.
*
****************************************************************************/

#ifndef __MGL_LIST_H
#define __MGL_LIST_H

#include "scitech.h"

/*---------------------- Macros and type definitions ----------------------*/

typedef struct LST_BUCKET {
    struct LST_BUCKET   *next;
    } LST_BUCKET;

typedef struct {
    int         count;          /* Number of elements currently in list */
    LST_BUCKET  *head;          /* Pointer to head element of list      */
    LST_BUCKET  *z;             /* Pointer to last node of list         */
    LST_BUCKET  hz[2];          /* Space for head and z nodes           */
    } LIST;

/* Return a pointer to the user space given the address of the header of
 * a node.
 */

#define LST_USERSPACE(h)    ((void*)((LST_BUCKET*)(h) + 1))

/* Return a pointer to the header of a node, given the address of the
 * user space.
 */

#define LST_HEADER(n)       ((LST_BUCKET*)(n) - 1)

/* Return a pointer to the user space of the list's head node. This user
 * space does not actually exist, but it is useful to be able to address
 * it to enable insertion at the start of the list.
 */

#define LST_HEAD(l)         LST_USERSPACE((l)->head)

/* Determine if a list is empty
 */

#define LST_EMPTY(l)        ((l)->count == 0)

/*-------------------------- Function Prototypes --------------------------*/

#ifdef __cplusplus
extern "C" {
#endif

void *_LST_newNode(int size);
void *_LST_newEmptyNode(int size);
void _LST_freeNode(void *node);
LIST *_LST_create(void);
void _LST_destroy(LIST *l,void (*freeNode)(void *node));
void _LST_addAfter(LIST *l,void *node,void *after);
void _LST_addToHead(LIST *l,void *node);
void _LST_addToTail(LIST *l,void *node);
void *_LST_removeNext(LIST *l,void *node);
void *_LST_first(LIST *l);
void *_LST_next(void *prev);
void *_LST_findNode(LIST *l,void *n);

#ifdef __cplusplus
}
#endif

#endif  /* __MGL_LIST_H */
