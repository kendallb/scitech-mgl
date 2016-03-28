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
* Description:  Header file to include either fx/fixed.h or fx/float.h
*               depending on the setting of compile time switches.
*               Whenever it is possible that you will used either
*               floating point or fixed point, include this header file.
*
*               It makes sure that the correct typedef is defined for
*               the 'real' type.
*
****************************************************************************/

#ifndef __FX_REAL_H
#define __FX_REAL_H

#ifndef __SCITECH_H
#include "scitech.h"
#endif

/*------------------------- Internal Macro's etc --------------------------*/

/* Determine what type to use for floating point numbers, either float
 * double or long double. We default to single precision floating point if
 * nothing is specified.
 */

#if !defined(FX_FLOAT) && !defined(FX_DOUBLE) && !defined(FX_FIXED)
#define FX_FLOAT
#endif

#if defined(FX_FLOAT) || defined(FX_DOUBLE)
#include "fx/float.h"
#endif

#ifdef  FX_FIXED
#include "fx/fixed.h"
typedef FXFixed     real;
typedef FXFixed     realdbl;
#undef  _FUDGE
#define _FUDGE      (4L)
#endif

#ifndef __FUDGE_H
#include "fx/fudge.h"
#endif

/* Decide what fudge factor to use in equality and tests for single and
 * double precision floating point numbers.
 *
 * We use a default factor defined below if none is set. Another factor
 * can be set by defining the fudge factor before including this file.
 */

#ifndef _FUDGE
#define _FUDGE  (real)(1e-6)
#endif

#endif  /* __FX_REAL_H */

