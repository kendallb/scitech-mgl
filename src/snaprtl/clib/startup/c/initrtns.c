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
* Description:  Code to run initialization/termination routines.
*
****************************************************************************/


#include "variety.h"
#include "initfini.h"
#include "rtinit.h"

#define PNEAR ((__type_rtp)0)
#define PFAR  ((__type_rtp)1)
#define PDONE ((__type_rtp)2)

#if ( COMP_CFG_COFF == 1 ) || defined(__AXP__)
    // following is an attempt to drop the need for an assembler
    // segment definitions file
    // unfortunately, the use of XIB,XIE,YIB,YIE doesn't get the
    // right sort of segments by default
    #pragma data_seg( ".rtl$xib", "DATA" );
    YIXI( TS_SEG_XIB, _Start_XI, 0, 0 )
    #pragma data_seg( ".rtl$xie", "DATA" );
    YIXI( TS_SEG_XIE, _End_XI, 0, 0 )
    #pragma data_seg( ".rtl$yib", "DATA" );
    YIXI( TS_SEG_YIB, _Start_YI, 0, 0 )
    #pragma data_seg( ".rtl$yie", "DATA" );
    YIXI( TS_SEG_YIE, _End_YI, 0, 0 )
    #pragma data_seg( ".data", "DATA" );
#elif defined(_M_IX86)
    extern struct rt_init _Start_XI;
    extern struct rt_init _End_XI;

    extern struct rt_init _Start_YI;
    extern struct rt_init _End_YI;
#elif defined(__PPC__) || defined(__X86_64__) || defined(__MIPS__)
    extern struct rt_init _Start_XI;
    extern struct rt_init _End_XI;

    extern struct rt_init _Start_YI;
    extern struct rt_init _End_YI;
#else
    #error unsupported platform
#endif

typedef void (*pfn)(void);
typedef void (_WCI86FAR * _WCI86FAR fpfn)(void);
typedef void (_WCI86NEAR * _WCI86NEAR npfn)(void);

static void callit( pfn *f ) {
    // don't call a null pointer
    if( *f ) {
        // call function
        (void)(**f)();
    }
}

/*
; - takes priority limit parm in eax, code will run init routines whose
;       priority is < eax (really al [0-255])
;       eax==255 -> run all init routines
;       eax==15  -> run init routines whose priority is <= 15
;
*/
void __InitRtns( unsigned limit ) {
    __type_rtp local_limit;
    struct rt_init _WCI86NEAR *pnext;

    local_limit = (__type_rtp)limit;
    for(;;) {
        {
            __type_rtp working_limit;
            struct rt_init _WCI86NEAR *pcur;

            pcur = (struct rt_init _WCI86NEAR*)&_Start_XI;
            #if defined(COMP_CFG_COFF)
                pcur++;
            #endif
            pnext = (struct rt_init _WCI86NEAR*)&_End_XI;
            working_limit = local_limit;

            // walk list of routines
            while( pcur < (struct rt_init _WCI86NEAR*)&_End_XI ) {
                // if this one hasn't been called
                if( pcur->rtn_type != PDONE ) {
                    // if the priority is better than best so far
                    if( pcur->priority <= working_limit ) {
                        // remember this one
                        pnext = pcur;
                        working_limit = pcur->priority;
                    }
                }
                // advance to next entry
                pcur++;
            }
            // check to see if all done, if we didn't find any
            // candidates then we can return
            if( pnext == (struct rt_init _WCI86NEAR*)&_End_XI ) {
                break;
            }
        }
        callit( &pnext->rtn );
        // mark entry as invoked
        pnext->rtn_type = PDONE;
    }
}

/*
; - takes priority range parms in eax, edx, code will run fini routines whose
;       priority is >= eax (really al [0-255]) and <= edx (really dl [0-255])
;       eax==0,  edx=255 -> run all fini routines
;       eax==16, edx=255 -> run fini routines in range 16..255
;       eax==16, edx=40  -> run fini routines in range 16..40
*/
void __FiniRtns( unsigned min_limit, unsigned max_limit ) {
    __type_rtp local_min_limit;
    __type_rtp local_max_limit;
    struct rt_init _WCI86NEAR *pnext;

    local_min_limit = (__type_rtp)min_limit;
    local_max_limit = (__type_rtp)max_limit;
    for(;;) {
        {
            __type_rtp working_limit;
            struct rt_init _WCI86NEAR *pcur;

            pcur = (struct rt_init _WCI86NEAR*)&_Start_YI;
            #if defined(COMP_CFG_COFF)
                pcur++;
            #endif
            pnext = (struct rt_init _WCI86NEAR*)&_End_YI;
            working_limit = local_min_limit;

            // walk list of routines
            while( pcur < (struct rt_init _WCI86NEAR*)&_End_YI ) {
                // if this one hasn't been called
                if( pcur->rtn_type != PDONE ) {
                    // if the priority is better than best so far
                    if( pcur->priority >= working_limit ) {
                        // remember this one
                        pnext = pcur;
                        working_limit = pcur->priority;
                    }
                }
                // advance to next entry
                pcur++;
            }
            // check to see if all done, if we didn't find any
            // candidates then we can return
            if( pnext == (struct rt_init _WCI86NEAR*)&_End_YI ) {
                break;
            }
        }
        if( pnext->priority <= local_max_limit ) {
            callit( &pnext->rtn );
        }
        // mark entry as invoked even if we don't call it
        // if we didn't call it, it is because we don't want to
        // call finirtns with priority > max_limit, in that case
        // marking the function as called, won't hurt anything
        pnext->rtn_type = PDONE;
    }
}
