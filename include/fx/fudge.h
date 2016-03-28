/****************************************************************************
*
*                 High Speed Fixed/Floating Point Library
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
* Language:     ANSI C/C++
* Environment:  any
*
* Description:  Header file for defining a number of macros for performing
*               fuzzy arithmetic on single and double precision
*               floating point values.
*
*               To avoid namespace clashes, all inline functions defined
*               in this header are in uppercase. They can be used to
*               replace previous macro versions of the functions (in fact
*               these are automatically #undef'd to avoid problems).
*
****************************************************************************/

#ifndef __FX_FUDGE_H
#define __FX_FUDGE_H

/*------------------------- Internal Macro's etc --------------------------*/

/* Undefine any possible macros that may already be defined to avoid
 * complications.
 */

#undef  FZERO
#undef  FEQ
#undef  FLE
#undef  FGE
#undef  FNE
#undef  FLT
#undef  FGT
#undef  LERP

/* Define macros versions of the routines which are quicker when the inline
 * function calls get deeply nested (the optimiser does a better job since
 * the arguments of an inline function call must be fully evaluated before
 * the next call). However the inline function versions are completely
 * side effect free.
 */

#define _FZERO(a,FUDGE) ((a) < FUDGE && (a) > -FUDGE)

#define _FEQ(a,b,FUDGE) _FZERO((a)-(b),FUDGE)
#define _FLE(a,b,FUDGE) (((a) < (b)) || _FEQ(a,b,FUDGE))
#define _FGE(a,b,FUDGE) (((a) > (b)) || _FEQ(a,b,FUDGE))

#define _FNE(a,b,FUDGE) (!_FZERO((a)-(b),FUDGE))
#define _FLT(a,b,FUDGE) (((a) < (b)) && _FNE(a,b,FUDGE))
#define _FGT(a,b,FUDGE) (((a) > (b)) && _FNE(a,b,FUDGE))
#define _LERP(lo,hi,alpha)  ((lo) + FXmul((hi)-(lo),alpha))
#define _SIGN(a)        ((a) > 0 ? 1 : -1)

#endif  /* __FX_FUDGE_H */
