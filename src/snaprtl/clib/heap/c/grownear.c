/****************************************************************************
*
*                            Open Watcom Project
*
*    Portions Copyright (c) 1983-2002 Sybase, Inc. All Rights Reserved.
*
*  ========================================================================
*
*    This file contains Original Code and/or Modifications of Original
*    Code as defined in and that are subject to the Sybase Open Watcom
*    Public License version 1.0 (the 'License'). You may not use this file
*    except in compliance with the License. BY USING THIS FILE YOU AGREE TO
*    ALL TERMS AND CONDITIONS OF THE LICENSE. A copy of the License is
*    provided with the Original Code and Modifications, and is also
*    available at www.sybase.com/developer/opensource.
*
*    The Original Code and all software distributed under the License are
*    distributed on an 'AS IS' basis, WITHOUT WARRANTY OF ANY KIND, EITHER
*    EXPRESS OR IMPLIED, AND SYBASE AND ALL CONTRIBUTORS HEREBY DISCLAIM
*    ALL SUCH WARRANTIES, INCLUDING WITHOUT LIMITATION, ANY WARRANTIES OF
*    MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE, QUIET ENJOYMENT OR
*    NON-INFRINGEMENT. Please see the License for the specific language
*    governing rights and limitations under the License.
*
*  ========================================================================
*
* Description:  Heap growing routines - allocate near heap memory from OS.
*
****************************************************************************/


#include "dll.h"        // needs to be first
#include "variety.h"
#include <stddef.h>
#include <stdlib.h>
#include <malloc.h>
#include "heapacc.h"
#include "heap.h"
#include <errno.h>
#if defined(__SNAP__)
 #include "scitech.h"
 #include <os/imports.h>
#endif


static frlptr __LinkUpNewMHeap( mheapptr );
static int __AdjustAmount( size_t *amount );

static frlptr __LinkUpNewMHeap( mheapptr p1 ) // originally __AddNewHeap()
{
    mheapptr    p2;
    mheapptr    p2_prev;
    tag         *last_tag;
    size_t      amount;

    /* insert into ordered heap list (14-jun-91 AFS) */
    /* logic wasn't inserting heaps in proper ascending order */
    /* (09-nov-93 Fred) */
    p2_prev = NULL;
    for( p2 = __nheapbeg; p2 != NULL; p2 = p2->next ) {
        if( p1 < p2 ) break;
        p2_prev = p2;
    }
    /* ascending order should be: p2_prev < p1 < p2  */
    /* except for special cases when p2_prev and/or p2 are NULL */
    p1->prev = p2_prev;
    p1->next = p2;
    if( p2_prev != NULL ) {
        p2_prev->next = p1;
    } else {            /* add p1 to beginning of heap */
        __nheapbeg = p1;
    }
    if( p2 != NULL ) {
        /* insert before 'p2' (list is non-empty) */
        p2->prev = p1;
    }
    amount = p1->len - sizeof( struct miniheapblkp );
    /* Fill out the new miniheap descriptor */
    p1->freehead.len = 0;
    p1->freehead.prev = &p1->freehead;
    p1->freehead.next = &p1->freehead;
    p1->rover = &p1->freehead;
    p1->b4rover = 0;
    p1->numalloc = 0;
    p1->numfree  = 0;
    p1++;
    ((frlptr)p1)->len = amount;
    /* fix up end of heap links */
    last_tag = (tag *) ( (PTR)p1 + amount );
    *last_tag = END_TAG;
    return( (frlptr) p1 );
}

static int __CreateNewNHeap( size_t amount )
{
    mheapptr        p1;
    frlptr          flp;
    unsigned long   brk_value;

    if( !__heap_enabled ) return( 0 );
    if( _curbrk == ~1u ) return( 0 );
    if( __AdjustAmount( &amount ) == 0 ) return( 0 );

    brk_value = (unsigned long) xmalloc( amount );
    if( brk_value == 0 ) {
        return( 0 );
    }

    if( amount - TAG_SIZE > amount ) {
        return( 0 );
    } else {
        amount -= TAG_SIZE;
    }
    if( amount < sizeof( struct miniheapblkp ) + sizeof( frl ) ) {
        /* there isn't enough for a heap block (struct miniheapblkp) and
           one free block (frl) */
        return( 0 );
    }
    /* we've got a new heap block */
    p1 = (mheapptr) brk_value;
    p1->len = amount;
    // Now link it up
    flp = __LinkUpNewMHeap( p1 );
    amount = flp->len;
    /* build a block for _nfree() */
    flp->len = amount | 1;
    ++p1->numalloc;                         /* 28-dec-90 */
    p1->largest_blk = 0;
    _nfree( (PTR)flp + TAG_SIZE );
    return( 1 );
}

int __ExpandDGROUP( size_t amount )
{
    // first try to free any available storage
    _nheapshrink();
    return( __CreateNewNHeap( amount ) );
}

static int __AdjustAmount( size_t *amount )
{
    size_t old_amount = *amount;
    size_t amt;

    amt = old_amount;
    amt = ( amt + TAG_SIZE + ROUND_SIZE) & ~ROUND_SIZE;
    if( amt < old_amount ) {
        return( 0 );
    }
    /* amount is even here */
    /*
      extra amounts        (22-feb-91 AFS)

       (1) adding a new heap needs:
           frl                    free block req'd for _nmalloc request
                                  (frl is the MINIMUM because the block
                                  may be freed)
           tag                    end of miniheap descriptor
           struct miniheapblkp    start of miniheap descriptor
       (2) extending heap needs:
           tag               free block req'd for _nmalloc request
    */
    *amount = amt;
    amt += ( (TAG_SIZE) + sizeof(frl) + sizeof(struct miniheapblkp) );
    if( amt < *amount ) return( 0 );
    if( amt < _amblksiz ) {
        /*
          _amblksiz may not be even so round down to an even number
          nb. pathological case: where _amblksiz == 0xffff, we don't
                                 want the usual round up to even
        */
        amt = _amblksiz & ~1u;
    }
    /* make sure amount is a multiple of 4k */
    *amount = amt;
    amt += 0x0fff;
    if( amt < *amount ) return( 0 );
    amt &= ~0x0fff;
    *amount = amt;
    return( *amount != 0 );
}
